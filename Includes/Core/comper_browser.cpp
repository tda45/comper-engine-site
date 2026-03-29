/*
 * Comper Engine Core - Browser Module Implementation
 * 
 * Tarayıcı yönetimi, sekme sistemi, yer işaretleri, geçmiş
 * Modern tarayıcı özellikleri ve UI yönetimi
 */

#include "comper_browser.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
#include <shlobj.h>
#include <winerror.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace ComperCore {

// Browser Implementation
Browser::Browser() {
    std::cout << "[Browser] Browser instance created" << std::endl;
    
    // Initialize statistics
    stats.start_time = std::chrono::high_resolution_clock::now();
    
    // Create root folder
    root_folder_id = "root";
    bookmark_folders[root_folder_id] = std::make_shared<BookmarkFolder>(root_folder_id, "Root");
}

Browser::~Browser() {
    std::cout << "[Browser] Browser instance destroyed" << std::endl;
    Shutdown();
}

bool Browser::Initialize() {
    std::cout << "[Browser] Initializing browser..." << std::endl;
    
    // Initialize core components
    network_manager = std::make_shared<NetworkManager>();
    renderer = std::make_shared<Renderer>();
    html_parser = std::make_shared<HTMLParser>();
    download_manager = DownloadFactory::CreateDownloadManager();
    
    // Initialize components
    if (!network_manager->Initialize()) {
        std::cerr << "[Browser] Failed to initialize network manager" << std::endl;
        return false;
    }
    
    if (!renderer->Initialize(nullptr)) {
        std::cerr << "[Browser] Failed to initialize renderer" << std::endl;
        return false;
    }
    
    if (!download_manager->Initialize()) {
        std::cerr << "[Browser] Failed to initialize download manager" << std::endl;
        return false;
    }
    
    // Initialize default data
    InitializeDefaultFolders();
    InitializeDefaultBookmarks();
    
    // Create current session
    current_session_id = CreateSession();
    
    // Load session data
    LoadSessionData();
    
    std::cout << "[Browser] Browser initialized successfully" << std::endl;
    return true;
}

void Browser::Shutdown() {
    std::cout << "[Browser] Shutting down browser..." << std::endl;
    
    // Save session data
    SaveSessionData();
    
    // End current session
    if (!current_session_id.empty()) {
        EndSession(current_session_id);
    }
    
    // Shutdown components
    if (download_manager) {
        download_manager->Shutdown();
    }
    
    if (renderer) {
        renderer->Shutdown();
    }
    
    if (network_manager) {
        network_manager->Shutdown();
    }
    
    std::cout << "[Browser] Browser shutdown completed" << std::endl;
}

// Tab Management
std::string Browser::CreateTab(const std::string& url) {
    std::string tab_id = GenerateTabId();
    auto tab = std::make_shared<Tab>(tab_id, url);
    
    // Setup event handlers
    SetupTabEventHandlers(tab);
    
    // Add to tabs
    tabs[tab_id] = tab;
    tab_order.push_back(tab_id);
    
    // Add to current session
    auto session = GetCurrentSession();
    if (session) {
        session->tab_ids.push_back(tab_id);
    }
    
    // Update statistics
    stats.total_tabs_created++;
    UpdateStatistics();
    
    // Notify callback
    if (on_tab_added) {
        on_tab_added(tab_id);
    }
    
    // Auto-activate if this is the first tab
    if (tabs.size() == 1) {
        ActivateTab(tab_id);
    }
    
    std::cout << "[Browser] Tab created: " << tab_id << " (" << url << ")" << std::endl;
    return tab_id;
}

bool Browser::CloseTab(const std::string& tab_id) {
    auto it = tabs.find(tab_id);
    if (it == tabs.end()) {
        return false;
    }
    
    auto tab = it->second;
    
    // Don't close the last tab
    if (tabs.size() <= 1) {
        std::cout << "[Browser] Cannot close last tab" << std::endl;
        return false;
    }
    
    // If closing active tab, activate another tab
    if (active_tab_id == tab_id) {
        // Find next tab to activate
        auto order_it = std::find(tab_order.begin(), tab_order.end(), tab_id);
        if (order_it != tab_order.end()) {
            // Try to activate next tab, or previous if no next
            if (order_it + 1 != tab_order.end()) {
                ActivateTab(*(order_it + 1));
            } else if (order_it != tab_order.begin()) {
                ActivateTab(*(order_it - 1));
            }
        }
    }
    
    // Remove from tabs and order
    tabs.erase(it);
    tab_order.erase(std::remove(tab_order.begin(), tab_order.end(), tab_id), tab_order.end());
    
    // Update statistics
    stats.total_tabs_closed++;
    UpdateStatistics();
    
    // Notify callback
    if (on_tab_removed) {
        on_tab_removed(tab_id);
    }
    
    std::cout << "[Browser] Tab closed: " << tab_id << std::endl;
    return true;
}

bool Browser::ActivateTab(const std::string& tab_id) {
    auto it = tabs.find(tab_id);
    if (it == tabs.end()) {
        return false;
    }
    
    // Deactivate previous active tab
    if (!active_tab_id.empty()) {
        auto prev_tab = tabs[active_tab_id];
        if (prev_tab) {
            prev_tab->is_active = false;
            prev_tab->is_selected = false;
        }
    }
    
    // Activate new tab
    auto tab = it->second;
    tab->is_active = true;
    tab->is_selected = true;
    tab->last_accessed = std::chrono::high_resolution_clock::now();
    
    active_tab_id = tab_id;
    current_url = tab->url;
    
    // Notify callback
    if (on_tab_activated) {
        on_tab_activated(tab_id);
    }
    
    std::cout << "[Browser] Tab activated: " << tab_id << std::endl;
    return true;
}

std::shared_ptr<Tab> Browser::GetActiveTab() const {
    if (active_tab_id.empty()) {
        return nullptr;
    }
    
    auto it = tabs.find(active_tab_id);
    return (it != tabs.end()) ? it->second : nullptr;
}

std::shared_ptr<Tab> Browser::GetTab(const std::string& tab_id) const {
    auto it = tabs.find(tab_id);
    return (it != tabs.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Tab>> Browser::GetAllTabs() const {
    std::vector<std::shared_ptr<Tab>> result;
    for (const auto& pair : tabs) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<Tab>> Browser::GetTabsInOrder() const {
    std::vector<std::shared_ptr<Tab>> result;
    for (const std::string& tab_id : tab_order) {
        auto it = tabs.find(tab_id);
        if (it != tabs.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

// Tab Navigation
bool Browser::NavigateTab(const std::string& tab_id, const std::string& url) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    // Normalize URL
    std::string normalized_url = NormalizeURL(url);
    if (normalized_url.empty()) {
        return false;
    }
    
    // Set loading state
    tab->SetLoading(true);
    tab->SetLoadProgress(0.0);
    
    // Add to history
    AddToHistory(normalized_url, "");
    
    // Navigate
    tab->Navigate(normalized_url);
    
    // Start network request
    network_manager->AsyncGet(normalized_url, [this, tab_id](const NetworkResponse& response) {
        auto tab = GetTab(tab_id);
        if (!tab) {
            return;
        }
        
        if (response.IsSuccess()) {
            // Parse HTML
            auto document = html_parser->Parse(response.body);
            if (document) {
                tab->document = document;
                tab->last_response = response;
                
                // Update title
                std::string title = GenerateTabTitle(normalized_url, response.body);
                UpdateTabTitle(tab_id, title);
                
                tab->SetCompleted();
            } else {
                tab->SetError("HTML parsing failed");
            }
        } else {
            tab->SetError("Network error: " + response.error_message);
        }
    });
    
    std::cout << "[Browser] Navigating tab " << tab_id << " to: " << normalized_url << std::endl;
    return true;
}

bool Browser::GoBackInTab(const std::string& tab_id) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    if (!tab->CanGoBack()) {
        return false;
    }
    
    std::string previous_url = tab->GoBack();
    if (!previous_url.empty()) {
        return NavigateTab(tab_id, previous_url);
    }
    
    return false;
}

bool Browser::GoForwardInTab(const std::string& tab_id) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    if (!tab->CanGoForward()) {
        return false;
    }
    
    std::string next_url = tab->GoForward();
    if (!next_url.empty()) {
        return NavigateTab(tab_id, next_url);
    }
    
    return false;
}

bool Browser::ReloadTab(const std::string& tab_id) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    return NavigateTab(tab_id, tab->url);
}

// Tab Manipulation
bool Browser::PinTab(const std::string& tab_id, bool pin) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    tab->is_pinned = pin;
    std::cout << "[Browser] Tab " << tab_id << " " << (pin ? "pinned" : "unpinned") << std::endl;
    return true;
}

bool Browser::MuteTab(const std::string& tab_id, bool mute) {
    auto tab = GetTab(tab_id);
    if (!tab) {
        return false;
    }
    
    tab->is_muted = mute;
    std::cout << "[Browser] Tab " << tab_id << " " << (mute ? "muted" : "unmuted") << std::endl;
    return true;
}

bool Browser::MoveTab(const std::string& tab_id, int new_position) {
    auto it = std::find(tab_order.begin(), tab_order.end(), tab_id);
    if (it == tab_order.end()) {
        return false;
    }
    
    // Remove from current position
    tab_order.erase(it);
    
    // Insert at new position
    new_position = std::max(0, std::min(new_position, (int)tab_order.size()));
    tab_order.insert(tab_order.begin() + new_position, tab_id);
    
    // Update tab positions
    for (size_t i = 0; i < tab_order.size(); ++i) {
        auto tab = tabs[tab_order[i]];
        if (tab) {
            tab->position = i;
        }
    }
    
    std::cout << "[Browser] Tab " << tab_id << " moved to position " << new_position << std::endl;
    return true;
}

std::string Browser::DuplicateTab(const std::string& tab_id) {
    auto original_tab = GetTab(tab_id);
    if (!original_tab) {
        return "";
    }
    
    // Create new tab with same URL
    std::string new_tab_id = CreateTab(original_tab->url);
    
    // Copy some properties
    auto new_tab = GetTab(new_tab_id);
    if (new_tab) {
        new_tab->is_pinned = original_tab->is_pinned;
        new_tab->is_muted = original_tab->is_muted;
    }
    
    std::cout << "[Browser] Tab duplicated: " << tab_id << " -> " << new_tab_id << std::endl;
    return new_tab_id;
}

// Bookmark Management
std::string Browser::AddBookmark(const std::string& title, const std::string& url, 
                               const std::string& folder_id) {
    std::string bookmark_id = GenerateBookmarkId();
    auto bookmark = std::make_shared<Bookmark>(bookmark_id, title, url);
    
    // Set folder
    bookmark->folder_id = folder_id.empty() ? root_folder_id : folder_id;
    
    // Add to bookmarks
    bookmarks[bookmark_id] = bookmark;
    
    // Add to folder
    auto folder = GetBookmarkFolder(bookmark->folder_id);
    if (folder) {
        folder->bookmark_ids.push_back(bookmark_id);
    }
    
    // Update statistics
    stats.total_bookmarks_created++;
    UpdateStatistics();
    
    // Notify callback
    if (on_bookmark_added) {
        on_bookmark_added(bookmark_id);
    }
    
    std::cout << "[Browser] Bookmark added: " << bookmark_id << " (" << title << " -> " << url << ")" << std::endl;
    return bookmark_id;
}

bool Browser::RemoveBookmark(const std::string& bookmark_id) {
    auto it = bookmarks.find(bookmark_id);
    if (it == bookmarks.end()) {
        return false;
    }
    
    auto bookmark = it->second;
    
    // Remove from folder
    auto folder = GetBookmarkFolder(bookmark->folder_id);
    if (folder) {
        auto& bookmark_ids = folder->bookmark_ids;
        bookmark_ids.erase(std::remove(bookmark_ids.begin(), bookmark_ids.end(), bookmark_id), bookmark_ids.end());
    }
    
    // Remove from bookmarks
    bookmarks.erase(it);
    
    std::cout << "[Browser] Bookmark removed: " << bookmark_id << std::endl;
    return true;
}

bool Browser::UpdateBookmark(const std::string& bookmark_id, const std::string& title, 
                           const std::string& url) {
    auto bookmark = GetBookmark(bookmark_id);
    if (!bookmark) {
        return false;
    }
    
    bookmark->title = title;
    bookmark->url = url;
    
    std::cout << "[Browser] Bookmark updated: " << bookmark_id << std::endl;
    return true;
}

std::shared_ptr<Bookmark> Browser::GetBookmark(const std::string& bookmark_id) const {
    auto it = bookmarks.find(bookmark_id);
    return (it != bookmarks.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Bookmark>> Browser::GetAllBookmarks() const {
    std::vector<std::shared_ptr<Bookmark>> result;
    for (const auto& pair : bookmarks) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<Bookmark>> Browser::GetBookmarksInFolder(const std::string& folder_id) const {
    std::vector<std::shared_ptr<Bookmark>> result;
    
    auto folder = GetBookmarkFolder(folder_id);
    if (folder) {
        for (const std::string& bookmark_id : folder->bookmark_ids) {
            auto bookmark = GetBookmark(bookmark_id);
            if (bookmark) {
                result.push_back(bookmark);
            }
        }
    }
    
    return result;
}

// Bookmark Folder Management
std::string Browser::CreateBookmarkFolder(const std::string& name, const std::string& parent_id) {
    std::string folder_id = GenerateFolderId();
    auto folder = std::make_shared<BookmarkFolder>(folder_id, name);
    
    // Set parent
    folder->parent_id = parent_id.empty() ? root_folder_id : parent_id;
    
    // Add to folders
    bookmark_folders[folder_id] = folder;
    
    // Add to parent folder
    auto parent_folder = GetBookmarkFolder(folder->parent_id);
    if (parent_folder) {
        parent_folder->subfolder_ids.push_back(folder_id);
    }
    
    std::cout << "[Browser] Bookmark folder created: " << folder_id << " (" << name << ")" << std::endl;
    return folder_id;
}

bool Browser::RemoveBookmarkFolder(const std::string& folder_id) {
    auto it = bookmark_folders.find(folder_id);
    if (it == bookmark_folders.end()) {
        return false;
    }
    
    auto folder = it->second;
    
    // Remove from parent folder
    auto parent_folder = GetBookmarkFolder(folder->parent_id);
    if (parent_folder) {
        auto& subfolder_ids = parent_folder->subfolder_ids;
        subfolder_ids.erase(std::remove(subfolder_ids.begin(), subfolder_ids.end(), folder_id), subfolder_ids.end());
    }
    
    // Remove bookmarks in this folder
    for (const std::string& bookmark_id : folder->bookmark_ids) {
        RemoveBookmark(bookmark_id);
    }
    
    // Remove subfolders
    for (const std::string& subfolder_id : folder->subfolder_ids) {
        RemoveBookmarkFolder(subfolder_id);
    }
    
    // Remove folder
    bookmark_folders.erase(it);
    
    std::cout << "[Browser] Bookmark folder removed: " << folder_id << std::endl;
    return true;
}

std::shared_ptr<BookmarkFolder> Browser::GetBookmarkFolder(const std::string& folder_id) const {
    auto it = bookmark_folders.find(folder_id);
    return (it != bookmark_folders.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<BookmarkFolder>> Browser::GetAllBookmarkFolders() const {
    std::vector<std::shared_ptr<BookmarkFolder>> result;
    for (const auto& pair : bookmark_folders) {
        result.push_back(pair.second);
    }
    return result;
}

// History Management
std::string Browser::AddToHistory(const std::string& url, const std::string& title) {
    // Check if URL already exists in history
    for (const auto& pair : history) {
        if (pair.second->url == url) {
            // Update existing entry
            pair.second->AddVisit();
            return pair.second->id;
        }
    }
    
    std::string history_id = GenerateHistoryId();
    auto entry = std::make_shared<HistoryEntry>(history_id, url, title);
    
    // Add to history
    history[history_id] = entry;
    history_order.push_back(history_id);
    
    // Add to current session
    auto session = GetCurrentSession();
    if (session) {
        session->history_entry_ids.push_back(history_id);
    }
    
    // Update statistics
    stats.total_history_entries++;
    UpdateStatistics();
    
    // Cleanup old history if needed
    CleanupOldHistory();
    
    // Notify callback
    if (on_history_added) {
        on_history_added(history_id);
    }
    
    std::cout << "[Browser] History entry added: " << history_id << " (" << url << ")" << std::endl;
    return history_id;
}

bool Browser::RemoveFromHistory(const std::string& history_id) {
    auto it = history.find(history_id);
    if (it == history.end()) {
        return false;
    }
    
    // Remove from history
    history.erase(it);
    
    // Remove from order
    history_order.erase(std::remove(history_order.begin(), history_order.end(), history_id), history_order.end());
    
    std::cout << "[Browser] History entry removed: " << history_id << std::endl;
    return true;
}

std::shared_ptr<HistoryEntry> Browser::GetHistoryEntry(const std::string& history_id) const {
    auto it = history.find(history_id);
    return (it != history.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<HistoryEntry>> Browser::GetHistory(size_t limit) const {
    std::vector<std::shared_ptr<HistoryEntry>> result;
    
    // Get most recent entries
    size_t start = (history_order.size() > limit) ? history_order.size() - limit : 0;
    
    for (size_t i = start; i < history_order.size(); ++i) {
        auto it = history.find(history_order[i]);
        if (it != history.end()) {
            result.push_back(it->second);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<HistoryEntry>> Browser::SearchHistory(const std::string& query, size_t limit) const {
    std::vector<std::shared_ptr<HistoryEntry>> result;
    
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    for (const auto& pair : history) {
        const auto& entry = pair.second;
        
        // Search in URL and title
        std::string lower_url = entry->url;
        std::string lower_title = entry->title;
        std::transform(lower_url.begin(), lower_url.end(), lower_url.begin(), ::tolower);
        std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);
        
        if (lower_url.find(lower_query) != std::string::npos || 
            lower_title.find(lower_query) != std::string::npos) {
            result.push_back(entry);
            
            if (result.size() >= limit) {
                break;
            }
        }
    }
    
    return result;
}

void Browser::ClearHistory() {
    history.clear();
    history_order.clear();
    
    std::cout << "[Browser] History cleared" << std::endl;
}

// Session Management
std::string Browser::CreateSession() {
    std::string session_id = GenerateSessionId();
    auto session = std::make_shared<BrowserSession>();
    
    sessions[session_id] = session;
    
    // Update statistics
    stats.total_sessions_created++;
    UpdateStatistics();
    
    std::cout << "[Browser] Session created: " << session_id << std::endl;
    return session_id;
}

bool Browser::EndSession(const std::string& session_id) {
    auto it = sessions.find(session_id);
    if (it == sessions.end()) {
        return false;
    }
    
    it->second->EndSession();
    
    std::cout << "[Browser] Session ended: " << session_id << std::endl;
    return true;
}

std::shared_ptr<BrowserSession> Browser::GetCurrentSession() const {
    auto it = sessions.find(current_session_id);
    return (it != sessions.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<BrowserSession>> Browser::GetAllSessions() const {
    std::vector<std::shared_ptr<BrowserSession>> result;
    for (const auto& pair : sessions) {
        result.push_back(pair.second);
    }
    return result;
}

// URL Management
void Browser::SetCurrentURL(const std::string& url) {
    current_url = url;
    
    if (on_url_changed) {
        on_url_changed(url);
    }
}

std::string Browser::GetCurrentURL() const {
    return current_url;
}

void Browser::SetURLEditing(bool editing) {
    is_url_editing = editing;
}

bool Browser::IsURLEditing() const {
    return is_url_editing;
}

// UI State Management
void Browser::SetBookmarksBarVisible(bool visible) {
    show_bookmarks_bar = visible;
}

bool Browser::IsBookmarksBarVisible() const {
    return show_bookmarks_bar;
}

void Browser::SetHistorySidebarVisible(bool visible) {
    show_history_sidebar = visible;
}

bool Browser::IsHistorySidebarVisible() const {
    return show_history_sidebar;
}

void Browser::SetDownloadsSidebarVisible(bool visible) {
    show_downloads_sidebar = visible;
}

bool Browser::IsDownloadsSidebarVisible() const {
    return show_downloads_sidebar;
}

// Event Callbacks
void Browser::SetTabAddedCallback(std::function<void(const std::string&)> callback) {
    on_tab_added = callback;
}

void Browser::SetTabRemovedCallback(std::function<void(const std::string&)> callback) {
    on_tab_removed = callback;
}

void Browser::SetTabActivatedCallback(std::function<void(const std::string&)> callback) {
    on_tab_activated = callback;
}

void Browser::SetBookmarkAddedCallback(std::function<void(const std::string&)> callback) {
    on_bookmark_added = callback;
}

void Browser::SetHistoryAddedCallback(std::function<void(const std::string&)> callback) {
    on_history_added = callback;
}

void Browser::SetURLChangedCallback(std::function<void(const std::string&)> callback) {
    on_url_changed = callback;
}

// Utility Methods
std::string Browser::GenerateTabTitle(const std::string& url, const std::string& content) {
    // Try to extract title from content
    if (!content.empty()) {
        std::string title = BrowserUtils::GetTitleFromContent(content);
        if (!title.empty()) {
            return title;
        }
    }
    
    // Fallback to URL
    return BrowserUtils::FormatURL(url);
}

std::string Browser::GetFaviconURL(const std::string& url) const {
    return BrowserUtils::GetTabIcon(url);
}

bool Browser::IsValidURL(const std::string& url) const {
    return BrowserUtils::IsBookmarkableURL(url);
}

std::string Browser::NormalizeURL(const std::string& url) const {
    if (url.empty()) {
        return "";
    }
    
    // Check if it's already a valid URL
    if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://") {
        return url;
    }
    
    // Check if it's a domain name
    if (url.find('.') != std::string::npos && !url.find(' ')) {
        return "https://" + url;
    }
    
    // Check if it's a search query
    if (url.find(' ') != std::string::npos) {
        return "https://www.google.com/search?q=" + url;
    }
    
    // Default to https
    return "https://" + url;
}

// Private Methods
void Browser::InitializeDefaultBookmarks() {
    // Add some default bookmarks
    AddBookmark("Google", "https://www.google.com");
    AddBookmark("GitHub", "https://github.com");
    AddBookmark("Stack Overflow", "https://stackoverflow.com");
    AddBookmark("MDN Web Docs", "https://developer.mozilla.org");
    
    std::cout << "[Browser] Default bookmarks initialized" << std::endl;
}

void Browser::InitializeDefaultFolders() {
    // Create some default folders
    CreateBookmarkFolder("Development", root_folder_id);
    CreateBookmarkFolder("Social", root_folder_id);
    CreateBookmarkFolder("News", root_folder_id);
    
    std::cout << "[Browser] Default bookmark folders initialized" << std::endl;
}

void Browser::SetupTabEventHandlers(const std::shared_ptr<Tab>& tab) {
    // Title changed callback
    tab->on_title_changed = [this](const Tab& t) {
        // Update UI if this is the active tab
        if (t.id == active_tab_id) {
            // Update window title or UI
        }
    };
    
    // URL changed callback
    tab->on_url_changed = [this](const Tab& t) {
        // Update address bar if this is the active tab
        if (t.id == active_tab_id) {
            SetCurrentURL(t.url);
        }
    };
    
    // Load started callback
    tab->on_load_started = [this](const Tab& t) {
        std::cout << "[Browser] Tab " << t.id << " started loading: " << t.url << std::endl;
    };
    
    // Load completed callback
    tab->on_load_completed = [this](const Tab& t) {
        std::cout << "[Browser] Tab " << t.id << " completed loading: " << t.url << std::endl;
        
        // Add to history
        if (!t.title.empty()) {
            AddToHistory(t.url, t.title);
        }
    };
    
    // Load error callback
    tab->on_load_error = [this](const Tab& t, const std::string& error) {
        std::cout << "[Browser] Tab " << t.id << " load error: " << error << std::endl;
    };
    
    // Progress updated callback
    tab->on_progress_updated = [this](const Tab& t, double progress) {
        // Update UI progress indicator
        if (t.id == active_tab_id) {
            // Update progress bar
        }
    };
}

void Browser::UpdateTabTitle(const std::string& tab_id, const std::string& title) {
    auto tab = GetTab(tab_id);
    if (tab) {
        tab->title = title;
        
        if (tab->on_title_changed) {
            tab->on_title_changed(*tab);
        }
    }
}

void Browser::CleanupOldHistory() {
    if (history_order.size() > max_history_size) {
        size_t to_remove = history_order.size() - max_history_size;
        
        for (size_t i = 0; i < to_remove; ++i) {
            std::string history_id = history_order[i];
            history.erase(history_id);
        }
        
        history_order.erase(history_order.begin(), history_order.begin() + to_remove);
        
        std::cout << "[Browser] Cleaned up " << to_remove << " old history entries" << std::endl;
    }
}

void Browser::SaveSessionData() {
    // In a real implementation, this would save to a file
    std::cout << "[Browser] Session data saved" << std::endl;
}

void Browser::LoadSessionData() {
    // In a real implementation, this would load from a file
    std::cout << "[Browser] Session data loaded" << std::endl;
}

std::string Browser::GenerateTabId() {
    return "tab_" + std::to_string(next_tab_id++);
}

std::string Browser::GenerateBookmarkId() {
    return "bookmark_" + std::to_string(next_bookmark_id++);
}

std::string Browser::GenerateFolderId() {
    return "folder_" + std::to_string(next_folder_id++);
}

std::string Browser::GenerateHistoryId() {
    return "history_" + std::to_string(next_history_id++);
}

std::string Browser::GenerateSessionId() {
    return "session_" + std::to_string(next_session_id++);
}

std::string Browser::ExtractDomain(const std::string& url) const {
    return BrowserUtils::GetDomainFromURL(url);
}

std::string Browser::ExtractPath(const std::string& url) const {
    return BrowserUtils::GetTabIcon(url);
}

bool Browser::IsInternalURL(const std::string& url) const {
    return url.substr(0, 9) == "about:blank" || 
           url.substr(0, 7) == "chrome://" ||
           url.substr(0, 9) == "comper://";
}

void Browser::UpdateStatistics() {
    // Update total browsing time
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats.start_time);
    stats.total_browsing_time = duration.count();
}

Browser::BrowserStats Browser::GetStatistics() const {
    return stats;
}

void Browser::ResetStatistics() {
    stats = BrowserStats();
    stats.start_time = std::chrono::high_resolution_clock::now();
    
    std::cout << "[Browser] Statistics reset" << std::endl;
}

// Factory Functions
namespace BrowserFactory {
    std::shared_ptr<Browser> CreateBrowser() {
        return std::make_shared<Browser>();
    }
}

// Utility Functions
namespace BrowserUtils {
    std::string GetTabIcon(const std::string& url) {
        // In a real implementation, this would return the favicon URL
        if (url.find("google.com") != std::string::npos) {
            return "https://www.google.com/favicon.ico";
        } else if (url.find("github.com") != std::string::npos) {
            return "https://github.com/favicon.ico";
        }
        
        return "/icons/default-tab-icon.png";
    }
    
    std::string GetBookmarkIcon(const std::string& url) {
        return GetTabIcon(url);
    }
    
    std::string FormatURL(const std::string& url) {
        if (url.length() > 50) {
            return url.substr(0, 47) + "...";
        }
        return url;
    }
    
    std::string GetDomainFromURL(const std::string& url) {
        size_t start = url.find("://");
        if (start != std::string::npos) {
            start += 3;
        } else {
            start = 0;
        }
        
        size_t end = url.find('/', start);
        if (end == std::string::npos) {
            end = url.length();
        }
        
        return url.substr(start, end - start);
    }
    
    std::string GetTitleFromContent(const std::string& content) {
        // Simple title extraction
        size_t title_start = content.find("<title>");
        if (title_start != std::string::npos) {
            title_start += 7;
            size_t title_end = content.find("</title>", title_start);
            if (title_end != std::string::npos) {
                std::string title = content.substr(title_start, title_end - title_start);
                
                // Trim whitespace
                title.erase(0, title.find_first_not_of(" \t\n\r"));
                title.erase(title.find_last_not_of(" \t\n\r") + 1);
                
                return title;
            }
        }
        
        return "";
    }
    
    bool IsBookmarkableURL(const std::string& url) {
        // Basic URL validation
        if (url.empty()) return false;
        
        // Check for valid protocols
        if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://") {
            return true;
        }
        
        // Check for internal URLs
        if (url.substr(0, 9) == "about:blank" || url.substr(0, 9) == "comper://") {
            return false;
        }
        
        return true;
    }
    
    std::vector<std::string> SuggestURLs(const std::string& partial_url) {
        std::vector<std::string> suggestions;
        
        // In a real implementation, this would use history and bookmarks
        // For now, return some basic suggestions
        if (partial_url.find("google") != std::string::npos) {
            suggestions.push_back("https://www.google.com");
            suggestions.push_back("https://google.com/search?q=" + partial_url);
        }
        
        return suggestions;
    }
}

} // namespace ComperCore
