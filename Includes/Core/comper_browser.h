#ifndef COMPER_BROWSER_H
#define COMPER_BROWSER_H

/*
 * Comper Engine Core - Browser Module
 * 
 * Tarayıcı yönetimi, sekme sistemi, yer işaretleri, geçmiş
 * Modern tarayıcı özellikleri ve UI yönetimi
 * 
 * Features:
 * - Tab management system
 * - Bookmark management
 * - History tracking
 * - Live URL updates
 * - Navigation controls
 * - Session management
 * - UI state management
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>
#include <stack>

#include "comper_network.h"
#include "comper_render.h"
#include "comper_html.h"
#include "comper_download.h"

namespace ComperCore {

// Tab structure
struct Tab {
    std::string id;                           // Unique tab ID
    std::string title;                        // Tab title
    std::string url;                           // Current URL
    std::string favicon_url;                   // Favicon URL
    std::shared_ptr<DOMDocument> document;     // DOM document
    NetworkResponse last_response;              // Last network response
    std::chrono::time_point<std::chrono::high_resolution_clock> created_at;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_accessed;
    
    // Navigation state
    std::stack<std::string> back_stack;        // Back navigation stack
    std::stack<std::string> forward_stack;     // Forward navigation stack
    std::string current_url;                   // Current URL in navigation
    
    // Tab state
    bool is_loading = false;                   // Loading state
    bool is_pinned = false;                    // Pinned tab
    bool is_muted = false;                     // Muted state
    double load_progress = 0.0;                // Load progress (0-1)
    std::string error_message;                 // Error message if any
    
    // UI state
    bool is_active = false;                    // Currently active tab
    bool is_selected = false;                  // Currently selected
    int position = -1;                         // Position in tab bar
    
    // Event callbacks
    std::function<void(const Tab&)> on_title_changed;
    std::function<void(const Tab&)> on_url_changed;
    std::function<void(const Tab&)> on_load_started;
    std::function<void(const Tab&)> on_load_completed;
    std::function<void(const Tab&, const std::string&)> on_load_error;
    std::function<void(const Tab&, double)> on_progress_updated;
    
    Tab(const std::string& tab_id, const std::string& initial_url = "") 
        : id(tab_id), url(initial_url), current_url(initial_url) {
        created_at = std::chrono::high_resolution_clock::now();
        last_accessed = created_at;
        
        if (!url.empty()) {
            back_stack.push(url);
        }
    }
    
    // Navigation methods
    void Navigate(const std::string& new_url) {
        if (!new_url.empty() && new_url != current_url) {
            // Add current URL to back stack if it exists and is different
            if (!current_url.empty() && current_url != new_url) {
                back_stack.push(current_url);
            }
            
            // Clear forward stack when navigating to new URL
            while (!forward_stack.empty()) {
                forward_stack.pop();
            }
            
            current_url = new_url;
            url = new_url;
            last_accessed = std::chrono::high_resolution_clock::now();
            
            if (on_url_changed) {
                on_url_changed(*this);
            }
        }
    }
    
    bool CanGoBack() const {
        return !back_stack.empty();
    }
    
    bool CanGoForward() const {
        return !forward_stack.empty();
    }
    
    std::string GoBack() {
        if (CanGoBack()) {
            // Add current URL to forward stack
            if (!current_url.empty()) {
                forward_stack.push(current_url);
            }
            
            // Get URL from back stack
            std::string previous_url = back_stack.top();
            back_stack.pop();
            
            current_url = previous_url;
            url = previous_url;
            last_accessed = std::chrono::high_resolution_clock::now();
            
            if (on_url_changed) {
                on_url_changed(*this);
            }
            
            return previous_url;
        }
        return "";
    }
    
    std::string GoForward() {
        if (CanGoForward()) {
            // Add current URL to back stack
            if (!current_url.empty()) {
                back_stack.push(current_url);
            }
            
            // Get URL from forward stack
            std::string next_url = forward_stack.top();
            forward_stack.pop();
            
            current_url = next_url;
            url = next_url;
            last_accessed = std::chrono::high_resolution_clock::now();
            
            if (on_url_changed) {
                on_url_changed(*this);
            }
            
            return next_url;
        }
        return "";
    }
    
    void Reload() {
        if (!current_url.empty()) {
            Navigate(current_url);
        }
    }
    
    // State management
    void SetLoading(bool loading) {
        is_loading = loading;
        if (loading) {
            if (on_load_started) {
                on_load_started(*this);
            }
        }
    }
    
    void SetLoadProgress(double progress) {
        load_progress = progress;
        if (on_progress_updated) {
            on_progress_updated(*this, progress);
        }
    }
    
    void SetCompleted() {
        is_loading = false;
        load_progress = 1.0;
        if (on_load_completed) {
            on_load_completed(*this);
        }
    }
    
    void SetError(const std::string& error) {
        is_loading = false;
        load_progress = 0.0;
        error_message = error;
        if (on_load_error) {
            on_load_error(*this, error);
        }
    }
    
    // Utility methods
    std::string GetFormattedTitle() const {
        return title.empty() ? "Yeni Sekme" : title;
    }
    
    std::string GetDisplayURL() const {
        if (url.empty()) return "about:blank";
        
        // Shorten long URLs for display
        if (url.length() > 50) {
            return url.substr(0, 47) + "...";
        }
        return url;
    }
    
    double GetAgeInSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - created_at);
        return duration.count();
    }
    
    double GetLastAccessAgeInSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_accessed);
        return duration.count();
    }
};

// Bookmark structure
struct Bookmark {
    std::string id;                           // Unique bookmark ID
    std::string title;                        // Bookmark title
    std::string url;                           // Bookmark URL
    std::string favicon_url;                   // Favicon URL
    std::string folder_id;                      // Parent folder ID
    std::chrono::time_point<std::chrono::high_resolution_clock> created_at;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_visited;
    int visit_count = 0;                       // Number of visits
    std::vector<std::string> tags;              // Tags
    std::string description;                    // Description
    
    Bookmark(const std::string& bookmark_id, const std::string& bookmark_title, 
             const std::string& bookmark_url)
        : id(bookmark_id), title(bookmark_title), url(bookmark_url) {
        created_at = std::chrono::high_resolution_clock::now();
    }
    
    void AddVisit() {
        last_visited = std::chrono::high_resolution_clock::now();
        visit_count++;
    }
    
    std::string GetFormattedDate() const {
        auto time_t = std::chrono::system_clock::to_time_t(created_at);
        std::tm tm = *std::localtime(&time_t);
        
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &tm);
        return std::string(buffer);
    }
};

// Bookmark folder structure
struct BookmarkFolder {
    std::string id;                           // Unique folder ID
    std::string name;                          // Folder name
    std::string parent_id;                     // Parent folder ID
    std::vector<std::string> bookmark_ids;     // Bookmark IDs in this folder
    std::vector<std::string> subfolder_ids;    // Subfolder IDs
    std::chrono::time_point<std::chrono::high_resolution_clock> created_at;
    bool is_expanded = true;                   // Expanded state in UI
    
    BookmarkFolder(const std::string& folder_id, const std::string& folder_name)
        : id(folder_id), name(folder_name) {
        created_at = std::chrono::high_resolution_clock::now();
    }
};

// History entry structure
struct HistoryEntry {
    std::string id;                           // Unique entry ID
    std::string url;                           // Visited URL
    std::string title;                        // Page title
    std::chrono::time_point<std::chrono::high_resolution_clock> visit_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_visit_time;
    int visit_count = 1;                      // Number of visits
    std::string favicon_url;                   // Favicon URL
    std::string session_id;                    // Session ID
    
    HistoryEntry(const std::string& entry_id, const std::string& entry_url, 
                const std::string& entry_title)
        : id(entry_id), url(entry_url), title(entry_title) {
        visit_time = std::chrono::high_resolution_clock::now();
        last_visit_time = visit_time;
        session_id = GenerateSessionId();
    }
    
    void AddVisit() {
        last_visit_time = std::chrono::high_resolution_clock::now();
        visit_count++;
    }
    
    std::string GetFormattedDateTime() const {
        auto time_t = std::chrono::system_clock::to_time_t(last_visit_time);
        std::tm tm = *std::localtime(&time_t);
        
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buffer);
    }
    
private:
    std::string GenerateSessionId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000, 999999);
        return "session_" + std::to_string(dis(gen));
    }
};

// Browser session structure
struct BrowserSession {
    std::string id;                           // Session ID
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    std::vector<std::string> tab_ids;          // Tab IDs in this session
    std::vector<std::string> history_entry_ids; // History entries
    bool is_active = true;                      // Session is active
    
    BrowserSession() {
        id = GenerateSessionId();
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void EndSession() {
        is_active = false;
        end_time = std::chrono::high_resolution_clock::now();
    }
    
    double GetDurationInSeconds() const {
        auto end = is_active ? std::chrono::high_resolution_clock::now() : end_time;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);
        return duration.count();
    }
    
private:
    std::string GenerateSessionId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000, 999999);
        return "session_" + std::to_string(dis(gen));
    }
};

// Browser class
class Browser {
private:
    // Core components
    std::shared_ptr<NetworkManager> network_manager;
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<HTMLParser> html_parser;
    std::shared_ptr<DownloadManager> download_manager;
    
    // Tab management
    std::map<std::string, std::shared_ptr<Tab>> tabs;
    std::string active_tab_id;
    std::vector<std::string> tab_order;             // Tab order in UI
    int next_tab_id = 1;
    
    // Bookmark management
    std::map<std::string, std::shared_ptr<Bookmark>> bookmarks;
    std::map<std::string, std::shared_ptr<BookmarkFolder>> bookmark_folders;
    std::string root_folder_id = "root";
    int next_bookmark_id = 1;
    int next_folder_id = 1;
    
    // History management
    std::map<std::string, std::shared_ptr<HistoryEntry>> history;
    std::vector<std::string> history_order;          // Chronological order
    int next_history_id = 1;
    size_t max_history_size = 1000;
    
    // Session management
    std::map<std::string, std::shared_ptr<BrowserSession>> sessions;
    std::string current_session_id;
    int next_session_id = 1;
    
    // UI state
    std::string current_url;                       // URL input field value
    bool is_url_editing = false;                   // URL field is being edited
    bool show_bookmarks_bar = true;                // Bookmarks bar visibility
    bool show_history_sidebar = false;              // History sidebar visibility
    bool show_downloads_sidebar = false;            // Downloads sidebar visibility
    
    // Event callbacks
    std::function<void(const std::string&)> on_tab_added;
    std::function<void(const std::string&)> on_tab_removed;
    std::function<void(const std::string&)> on_tab_activated;
    std::function<void(const std::string&)> on_bookmark_added;
    std::function<void(const std::string&)> on_history_added;
    std::function<void(const std::string&)> on_url_changed;
    
public:
    Browser();
    ~Browser();
    
    // Initialization
    bool Initialize();
    void Shutdown();
    
    // Tab management
    std::string CreateTab(const std::string& url = "");
    bool CloseTab(const std::string& tab_id);
    bool ActivateTab(const std::string& tab_id);
    std::shared_ptr<Tab> GetActiveTab() const;
    std::shared_ptr<Tab> GetTab(const std::string& tab_id) const;
    std::vector<std::shared_ptr<Tab>> GetAllTabs() const;
    std::vector<std::shared_ptr<Tab>> GetTabsInOrder() const;
    
    // Tab navigation
    bool NavigateTab(const std::string& tab_id, const std::string& url);
    bool GoBackInTab(const std::string& tab_id);
    bool GoForwardInTab(const std::string& tab_id);
    bool ReloadTab(const std::string& tab_id);
    
    // Tab manipulation
    bool PinTab(const std::string& tab_id, bool pin = true);
    bool MuteTab(const std::string& tab_id, bool mute = true);
    bool MoveTab(const std::string& tab_id, int new_position);
    std::string DuplicateTab(const std::string& tab_id);
    
    // Bookmark management
    std::string AddBookmark(const std::string& title, const std::string& url, 
                           const std::string& folder_id = "");
    bool RemoveBookmark(const std::string& bookmark_id);
    bool UpdateBookmark(const std::string& bookmark_id, const std::string& title, 
                      const std::string& url);
    std::shared_ptr<Bookmark> GetBookmark(const std::string& bookmark_id) const;
    std::vector<std::shared_ptr<Bookmark>> GetAllBookmarks() const;
    std::vector<std::shared_ptr<Bookmark>> GetBookmarksInFolder(const std::string& folder_id) const;
    
    // Bookmark folder management
    std::string CreateBookmarkFolder(const std::string& name, const std::string& parent_id = "");
    bool RemoveBookmarkFolder(const std::string& folder_id);
    std::shared_ptr<BookmarkFolder> GetBookmarkFolder(const std::string& folder_id) const;
    std::vector<std::shared_ptr<BookmarkFolder>> GetAllBookmarkFolders() const;
    
    // History management
    std::string AddToHistory(const std::string& url, const std::string& title);
    bool RemoveFromHistory(const std::string& history_id);
    std::shared_ptr<HistoryEntry> GetHistoryEntry(const std::string& history_id) const;
    std::vector<std::shared_ptr<HistoryEntry>> GetHistory(size_t limit = 100) const;
    std::vector<std::shared_ptr<HistoryEntry>> SearchHistory(const std::string& query, size_t limit = 50) const;
    void ClearHistory();
    
    // Session management
    std::string CreateSession();
    bool EndSession(const std::string& session_id);
    std::shared_ptr<BrowserSession> GetCurrentSession() const;
    std::vector<std::shared_ptr<BrowserSession>> GetAllSessions() const;
    
    // URL management
    void SetCurrentURL(const std::string& url);
    std::string GetCurrentURL() const;
    void SetURLEditing(bool editing);
    bool IsURLEditing() const;
    
    // UI state management
    void SetBookmarksBarVisible(bool visible);
    bool IsBookmarksBarVisible() const;
    void SetHistorySidebarVisible(bool visible);
    bool IsHistorySidebarVisible() const;
    void SetDownloadsSidebarVisible(bool visible);
    bool IsDownloadsSidebarVisible() const;
    
    // Event callbacks
    void SetTabAddedCallback(std::function<void(const std::string&)> callback);
    void SetTabRemovedCallback(std::function<void(const std::string&)> callback);
    void SetTabActivatedCallback(std::function<void(const std::string&)> callback);
    void SetBookmarkAddedCallback(std::function<void(const std::string&)> callback);
    void SetHistoryAddedCallback(std::function<void(const std::string&)> callback);
    void SetURLChangedCallback(std::function<void(const std::string&)> callback);
    
    // Utility methods
    std::string GenerateTabTitle(const std::string& url, const std::string& content);
    std::string GetFaviconURL(const std::string& url) const;
    bool IsValidURL(const std::string& url) const;
    std::string NormalizeURL(const std::string& url) const;
    
    // Statistics
    struct BrowserStats {
        size_t total_tabs_created = 0;
        size_t total_tabs_closed = 0;
        size_t total_bookmarks_created = 0;
        size_t total_history_entries = 0;
        size_t total_sessions_created = 0;
        double total_browsing_time = 0.0;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    };
    
    BrowserStats GetStatistics() const;
    void ResetStatistics();
    
private:
    // Internal methods
    void InitializeDefaultBookmarks();
    void InitializeDefaultFolders();
    void SetupTabEventHandlers(const std::shared_ptr<Tab>& tab);
    void UpdateTabTitle(const std::string& tab_id, const std::string& title);
    void CleanupOldHistory();
    void SaveSessionData();
    void LoadSessionData();
    
    // ID generation
    std::string GenerateTabId();
    std::string GenerateBookmarkId();
    std::string GenerateFolderId();
    std::string GenerateHistoryId();
    std::string GenerateSessionId();
    
    // URL utilities
    std::string ExtractDomain(const std::string& url) const;
    std::string ExtractPath(const std::string& url) const;
    bool IsInternalURL(const std::string& url) const;
    
    // Statistics tracking
    BrowserStats stats;
    void UpdateStatistics();
};

// Factory functions
namespace BrowserFactory {
    std::shared_ptr<Browser> CreateBrowser();
}

// Utility functions
namespace BrowserUtils {
    std::string GetTabIcon(const std::string& url);
    std::string GetBookmarkIcon(const std::string& url);
    std::string FormatURL(const std::string& url);
    std::string GetDomainFromURL(const std::string& url);
    std::string GetTitleFromContent(const std::string& content);
    bool IsBookmarkableURL(const std::string& url);
    std::vector<std::string> SuggestURLs(const std::string& partial_url);
}

} // namespace ComperCore

#endif // COMPER_BROWSER_H
