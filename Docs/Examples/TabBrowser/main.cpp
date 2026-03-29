/*
 * Comper Engine SDK - Tab Browser Example
 * 
 * Modern tarayıcı sekme sistemi, yer işaretleri, geçmiş yönetimi
 * Canlı URL değişimi, navigasyon kontrolleri, UI yönetimi
 * 
 * Features:
 * - Multi-tab management
 * - Bookmark system with folders
 * - History tracking and search
 * - Live URL updates
 * - Navigation controls
 * - Session management
 * - Interactive UI
 * - Night Red Ultra theme
 */

#include "../../../Includes/Core/comper_core.h"
#include "../../../Includes/Core/comper_browser.h"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>

using namespace ComperCore;

class TabBrowserExample {
private:
    std::shared_ptr<Browser> browser;
    bool is_running = true;
    
    // UI state
    enum class DisplayMode {
        MAIN_MENU,
        TAB_OVERVIEW,
        BOOKMARKS,
        HISTORY,
        SETTINGS
    } current_mode = DisplayMode::MAIN_MENU;
    
    // Display options
    bool show_active_tab_only = false;
    bool show_tab_details = false;
    bool show_bookmarks_folders = true;
    int history_items_per_page = 10;
    
public:
    TabBrowserExample() {
        std::cout << "🚀 Comper Tab Browser başlatılıyor..." << std::endl;
        
        // Browser'ı oluştur
        browser = BrowserFactory::CreateBrowser();
        
        // Event handlers'ı ayarla
        SetupEventHandlers();
        
        std::cout << "✅ Tab Browser hazır!" << std::endl;
    }
    
    ~TabBrowserExample() {
        std::cout << "\n📊 Final İstatistikler:" << std::endl;
        ShowBrowserStatistics();
        std::cout << "👋 Tab Browser kapatılıyor..." << std::endl;
    }
    
private:
    void SetupEventHandlers() {
        std::cout << "🔧 Event handlers ayarlanıyor..." << std::endl;
        
        // Tab events
        browser->SetTabAddedCallback([this](const std::string& tab_id) {
            std::cout << "\n📑 Yeni sekme eklendi: " << tab_id << std::endl;
            RefreshDisplay();
        });
        
        browser->SetTabRemovedCallback([this](const std::string& tab_id) {
            std::cout << "\n❌ Sekme kapatıldı: " << tab_id << std::endl;
            RefreshDisplay();
        });
        
        browser->SetTabActivatedCallback([this](const std::string& tab_id) {
            std::cout << "\n🎯 Sekme aktif edildi: " << tab_id << std::endl;
            RefreshDisplay();
        });
        
        // Bookmark events
        browser->SetBookmarkAddedCallback([this](const std::string& bookmark_id) {
            std::cout << "\n⭐ Yer işareti eklendi: " << bookmark_id << std::endl;
        });
        
        // History events
        browser->SetHistoryAddedCallback([this](const std::string& history_id) {
            std::cout << "\n📚 Geçmiş eklendi: " << history_id << std::endl;
        });
        
        // URL change events
        browser->SetURLChangedCallback([this](const std::string& url) {
            std::cout << "\n🌐 URL değişti: " << url << std::endl;
            UpdateURLDisplay(url);
        });
        
        std::cout << "✅ Event handlers hazır!" << std::endl;
    }
    
public:
    void RunInteractive() {
        std::cout << "\n🎯 Comper Tab Browser - Interactive Mode" << std::endl;
        std::cout << "Modern Sekme Yönetimi" << std::endl;
        std::cout << "Yer İşaretleri ve Geçmiş" << std::endl;
        std::cout << "Night Red Ultra Teması" << std::endl;
        std::cout << "Yardım için 'help' yazın" << std::endl;
        
        // Başlangıç sekmelerini oluştur
        CreateInitialTabs();
        
        // Ana döngü
        std::string command;
        while (is_running) {
            ShowCurrentDisplay();
            
            std::cout << "\nTabBrowser> ";
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            ProcessCommand(command);
        }
    }
    
private:
    void CreateInitialTabs() {
        std::cout << "\n📑 Başlangıç sekmeleri oluşturuluyor..." << std::endl;
        
        // Ana sayfa sekmesi
        std::string tab1 = browser->CreateTab("https://www.google.com");
        std::cout << "✅ Sekme oluşturuldu: " << tab1 << " (Google)" << std::endl;
        
        // GitHub sekmesi
        std::string tab2 = browser->CreateTab("https://github.com");
        std::cout << "✅ Sekme oluşturuldu: " << tab2 << " (GitHub)" << std::endl;
        
        // Dokümantasyon sekmesi
        std::string tab3 = browser->CreateTab("https://developer.mozilla.org");
        std::cout << "✅ Sekme oluşturuldu: " << tab3 << " (MDN Docs)" << std::endl;
        
        // İlk sekmeyi aktif et
        browser->ActivateTab(tab1);
        
        std::cout << "📑 " << browser->GetAllTabs().size() << " sekme oluşturuldu" << std::endl;
    }
    
    void ShowCurrentDisplay() {
        ClearScreen();
        
        switch (current_mode) {
            case DisplayMode::MAIN_MENU:
                ShowMainMenu();
                break;
            case DisplayMode::TAB_OVERVIEW:
                ShowTabOverview();
                break;
            case DisplayMode::BOOKMARKS:
                ShowBookmarks();
                break;
            case DisplayMode::HISTORY:
                ShowHistory();
                break;
            case DisplayMode::SETTINGS:
                ShowSettings();
                break;
        }
    }
    
    void ShowMainMenu() {
        std::cout << "🌟 COMPER TAB BROWSER 🌟" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Aktif sekme bilgisi
        auto active_tab = browser->GetActiveTab();
        if (active_tab) {
            std::cout << "🎯 Aktif Sekme: " << active_tab->GetFormattedTitle() << std::endl;
            std::cout << "🌐 URL: " << active_tab->GetDisplayURL() << std::endl;
            std::cout << "📊 Durum: " << GetTabStatusString(*active_tab) << std::endl;
        } else {
            std::cout << "📭 Aktif sekme yok" << std::endl;
        }
        
        // Hızlı istatistikler
        ShowQuickStats();
        
        std::cout << "\n📋 MENÜ:" << std::endl;
        std::cout << "  1. Sekme Yönetimi" << std::endl;
        std::cout << "  2. Yer İşaretleri" << std::endl;
        std::cout << "  3. Geçmiş" << std::endl;
        std::cout << "  4. Ayarlar" << std::endl;
        std::cout << "  5. Hızlı Komutlar" << std::endl;
        std::cout << "  6. Yardım" << std::endl;
        std::cout << "  7. Çıkış" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowTabOverview() {
        current_mode = DisplayMode::TAB_OVERVIEW;
        ClearScreen();
        
        std::cout << "📑 SEKME YÖNETİMİ 📑" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto tabs = browser->GetTabsInOrder();
        
        if (tabs.empty()) {
            std::cout << "📭 Açık sekme bulunmamaktadır." << std::endl;
        } else {
            for (size_t i = 0; i < tabs.size(); ++i) {
                RenderTab(*tabs[i], i + 1);
            }
        }
        
        std::cout << "\n🔧 SEKME KOMUTLARI:" << std::endl;
        std::cout << "  new <url>         - Yeni sekme aç" << std::endl;
        std::cout << "  close <id>        - Sekmeyi kapat" << std::endl;
        std::cout << "  activate <id>     - Sekmeyi aktif et" << std::endl;
        std::cout << "  navigate <id> <url> - Sekmede git" << std::endl;
        std::cout << "  back <id>         - Geri git" << std::endl;
        std::cout << "  forward <id>      - İleri git" << std::endl;
        std::cout << "  reload <id>       - Sekmeyi yenile" << std::endl;
        std::cout << "  duplicate <id>    - Sekmeyi kopyala" << std::endl;
        std::cout << "  pin <id>          - Sekmeyi sabitle" << std::endl;
        std::cout << "  mute <id>         - Sekmeyi sessize al" << std::endl;
        std::cout << "  move <id> <pos>  - Sekmeyi taşı" << std::endl;
        std::cout << "  details <id>      - Sekme detayları" << std::endl;
        std::cout << "  menu              - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowBookmarks() {
        current_mode = DisplayMode::BOOKMARKS;
        ClearScreen();
        
        std::cout << "⭐ YER İŞARETLERİ ⭐" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Show folders first
        if (show_bookmarks_folders) {
            auto folders = browser->GetAllBookmarkFolders();
            for (const auto& folder : folders) {
                if (folder->id != "root") { // Skip root folder
                    RenderBookmarkFolder(*folder);
                }
            }
        }
        
        // Show bookmarks in root folder
        auto bookmarks = browser->GetBookmarksInFolder("root");
        if (!bookmarks.empty()) {
            std::cout << "\n📁 Kök Klasör:" << std::endl;
            for (const auto& bookmark : bookmarks) {
                RenderBookmark(*bookmark);
            }
        }
        
        std::cout << "\n🔧 YER İŞARETİ KOMUTLARI:" << std::endl;
        std::cout << "  add <title> <url>    - Yer işareti ekle" << std::endl;
        std::cout << "  remove <id>          - Yer işaretini sil" << std::endl;
        std::cout << "  update <id> <title> <url> - Yer işaretini güncelle" << std::endl;
        std::cout << "  folder <name>        - Klasör oluştur" << std::endl;
        std::cout << "  search <query>       - Arama yap" << std::endl;
        std::cout << "  toggle_folders       - Klasörleri göster/gizle" << std::endl;
        std::cout << "  menu                 - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowHistory() {
        current_mode = DisplayMode::HISTORY;
        ClearScreen();
        
        std::cout << "📚 GEÇMİŞ 📚" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto history = browser->GetHistory(history_items_per_page);
        
        if (history.empty()) {
            std::cout << "📭 Geçmiş bulunmamaktadır." << std::endl;
        } else {
            std::cout << "📅 Son " << history.size() << " ziyaret:" << std::endl;
            for (size_t i = 0; i < history.size(); ++i) {
                RenderHistoryEntry(*history[i], i + 1);
            }
        }
        
        std::cout << "\n🔧 GEÇMİŞ KOMUTLARI:" << std::endl;
        std::cout << "  search <query>        - Geçmişte ara" << std::endl;
        std::cout << "  clear                - Geçmişi temizle" << std::endl;
        std::cout << "  remove <id>          - Girdiyi sil" << std::endl;
        std::cout << "  more                 - Daha fazla göster" << std::endl;
        std::cout << "  less                 - Daha az göster" << std::endl;
        std::cout << "  menu                 - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowSettings() {
        current_mode = DisplayMode::SETTINGS;
        ClearScreen();
        
        std::cout << "⚙️ AYARLAR ⚙️" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "📊 Görüntü Ayarları:" << std::endl;
        std::cout << "  Yer İşareti Klasörleri: " << (show_bookmarks_folders ? "✅ Göster" : "❌ Gizle") << std::endl;
        std::cout << "  Geçmiş Öğe Sayısı: " << history_items_per_page << std::endl;
        
        std::cout << "\n🔧 KOMUTLAR:" << std::endl;
        std::cout << "  toggle_folders        - Klasörleri göster/gizle" << std::endl;
        std::cout << "  set_history_count <n> - Geçmiş sayısını ayarla" << std::endl;
        std::cout << "  reset_stats           - İstatistikleri sıfırla" << std::endl;
        std::cout << "  export_bookmarks      - Yer işaretlerini dışa aktar" << std::endl;
        std::cout << "  import_bookmarks      - Yer işaretlerini içe aktar" << std::endl;
        std::cout << "  menu                  - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void RenderTab(const Tab& tab, int index) {
        std::string status_icon = GetTabStatusIcon(tab);
        std::string active_marker = (tab.is_active) ? "► " : "  ";
        
        std::cout << active_marker << index << ". " << status_icon << " " << tab.GetFormattedTitle() << std::endl;
        std::cout << "    🌐 " << tab.GetDisplayURL() << std::endl;
        
        if (show_tab_details) {
            std::cout << "    📊 Durum: " << GetTabStatusString(tab) << std::endl;
            std::cout << "    📅 Yaş: " << std::fixed << std::setprecision(1) << tab.GetAgeInSeconds() << "s" << std::endl;
            
            if (tab.is_pinned) {
                std::cout << "    📌 Sabitli" << std::endl;
            }
            if (tab.is_muted) {
                std::cout << "    🔇 Sessiz" << std::endl;
            }
        }
        
        std::cout << std::string(60, '-') << std::endl;
    }
    
    void RenderBookmark(const Bookmark& bookmark) {
        std::cout << "  ⭐ " << bookmark.title << std::endl;
        std::cout << "    🌐 " << bookmark.url << std::endl;
        std::cout << "    📅 " << bookmark.GetFormattedDate() << std::endl;
        std::cout << "    🔄 Ziyaret: " << bookmark.visit_count << " kez" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
    }
    
    void RenderBookmarkFolder(const BookmarkFolder& folder) {
        std::string expand_marker = folder.is_expanded ? "▼" : "▶";
        std::cout << "  📁 " << expand_marker << " " << folder.name << std::endl;
        
        if (folder.is_expanded) {
            // Show bookmarks in this folder
            auto bookmarks = browser->GetBookmarksInFolder(folder.id);
            for (const auto& bookmark : bookmarks) {
                std::cout << "    ⭐ " << bookmark->title << std::endl;
            }
            
            // Show subfolders
            for (const std::string& subfolder_id : folder.subfolder_ids) {
                auto subfolder = browser->GetBookmarkFolder(subfolder_id);
                if (subfolder) {
                    std::cout << "    📁 " << subfolder->name << std::endl;
                }
            }
        }
        
        std::cout << std::string(60, '-') << std::endl;
    }
    
    void RenderHistoryEntry(const HistoryEntry& entry, int index) {
        std::cout << "  " << index << ". " << entry.title << std::endl;
        std::cout << "    🌐 " << entry.url << std::endl;
        std::cout << "    📅 " << entry.GetFormattedDateTime() << std::endl;
        std::cout << "    🔄 Ziyaret: " << entry.visit_count << " kez" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
    }
    
    void ShowQuickStats() {
        auto stats = browser->GetStatistics();
        auto tabs = browser->GetAllTabs();
        auto bookmarks = browser->GetAllBookmarks();
        auto history = browser->GetHistory();
        
        std::cout << "📊 Hızlı İstatistikler:" << std::endl;
        std::cout << "  Açık Sekmeler: " << tabs.size() << std::endl;
        std::cout << "  Yer İşaretleri: " << bookmarks.size() << std::endl;
        std::cout << "  Geçmiş Girdileri: " << history.size() << std::endl;
        std::cout << "  Toplam Sekme: " << stats.total_tabs_created << std::endl;
        std::cout << "  Toplam Oturum: " << stats.total_sessions_created << std::endl;
    }
    
    void ShowBrowserStatistics() {
        auto stats = browser->GetStatistics();
        
        std::cout << "📊 Son İstatistikler:" << std::endl;
        std::cout << "  Toplam Sekme Oluşturuldu: " << stats.total_tabs_created << std::endl;
        std::cout << "  Toplam Sekme Kapatıldı: " << stats.total_tabs_closed << std::endl;
        std::cout << "  Toplam Yer İşareti: " << stats.total_bookmarks_created << std::endl;
        std::cout << "  Toplam Geçmiş Girdisi: " << stats.total_history_entries << std::endl;
        std::cout << "  Toplam Oturum: " << stats.total_sessions_created << std::endl;
        std::cout << "  Tarayıcı Süresi: " << std::fixed << std::setprecision(1) 
                  << (stats.total_browsing_time / 60) << " dakika" << std::endl;
    }
    
    void ProcessCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "1" || cmd == "tabs") {
            ShowTabOverview();
        } else if (cmd == "2" || cmd == "bookmarks") {
            ShowBookmarks();
        } else if (cmd == "3" || cmd == "history") {
            ShowHistory();
        } else if (cmd == "4" || cmd == "settings") {
            ShowSettings();
        } else if (cmd == "5" || cmd == "quick") {
            ShowQuickCommands();
        } else if (cmd == "6" || cmd == "help") {
            ShowHelp();
        } else if (cmd == "7" || cmd == "exit" || cmd == "quit") {
            is_running = false;
        } else if (current_mode == DisplayMode::TAB_OVERVIEW) {
            ProcessTabCommand(cmd, iss);
        } else if (current_mode == DisplayMode::BOOKMARKS) {
            ProcessBookmarkCommand(cmd, iss);
        } else if (current_mode == DisplayMode::HISTORY) {
            ProcessHistoryCommand(cmd, iss);
        } else if (current_mode == DisplayMode::SETTINGS) {
            ProcessSettingsCommand(cmd, iss);
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::cout << "Yardım için 'help' yazın" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    void ProcessTabCommand(const std::string& cmd, std::istringstream& iss) {
        if (cmd == "new") {
            std::string url;
            iss >> url;
            if (!url.empty()) {
                std::string tab_id = browser->CreateTab(url);
                std::cout << "✅ Sekme oluşturuldu: " << tab_id << std::endl;
            } else {
                std::cout << "Kullanım: new <url>" << std::endl;
            }
        } else if (cmd == "close") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->CloseTab(id)) {
                    std::cout << "✅ Sekme kapatıldı: " << id << std::endl;
                } else {
                    std::cout << "❌ Sekme kapatılamadı: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: close <tab_id>" << std::endl;
            }
        } else if (cmd == "activate") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->ActivateTab(id)) {
                    std::cout << "✅ Sekme aktif edildi: " << id << std::endl;
                } else {
                    std::cout << "❌ Sekme aktif edilemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: activate <tab_id>" << std::endl;
            }
        } else if (cmd == "navigate") {
            std::string id, url;
            iss >> id >> url;
            if (!id.empty() && !url.empty()) {
                if (browser->NavigateTab(id, url)) {
                    std::cout << "✅ Sekme yönlendirildi: " << id << " -> " << url << std::endl;
                } else {
                    std::cout << "❌ Sekme yönlendirilemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: navigate <tab_id> <url>" << std::endl;
            }
        } else if (cmd == "back") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->GoBackInTab(id)) {
                    std::cout << "✅ Geri gidildi: " << id << std::endl;
                } else {
                    std::cout << "❌ Geri gidilemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: back <tab_id>" << std::endl;
            }
        } else if (cmd == "forward") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->GoForwardInTab(id)) {
                    std::cout << "✅ İleri gidildi: " << id << std::endl;
                } else {
                    std::cout << "❌ İleri gidilemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: forward <tab_id>" << std::endl;
            }
        } else if (cmd == "reload") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->ReloadTab(id)) {
                    std::cout << "✅ Sekme yenilendi: " << id << std::endl;
                } else {
                    std::cout << "❌ Sekme yenilenemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: reload <tab_id>" << std::endl;
            }
        } else if (cmd == "duplicate") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                std::string new_id = browser->DuplicateTab(id);
                if (!new_id.empty()) {
                    std::cout << "✅ Sekme kopyalandı: " << id << " -> " << new_id << std::endl;
                } else {
                    std::cout << "❌ Sekme kopyalanamadı: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: duplicate <tab_id>" << std::endl;
            }
        } else if (cmd == "pin") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->PinTab(id, true)) {
                    std::cout << "✅ Sekme sabitlendi: " << id << std::endl;
                } else {
                    std::cout << "❌ Sekme sabitlenemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: pin <tab_id>" << std::endl;
            }
        } else if (cmd == "mute") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->MuteTab(id, true)) {
                    std::cout << "✅ Sekme sessize alındı: " << id << std::endl;
                } else {
                    std::cout << "❌ Sekme sessize alınamadı: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: mute <tab_id>" << std::endl;
            }
        } else if (cmd == "move") {
            std::string id, pos_str;
            iss >> id >> pos_str;
            if (!id.empty() && !pos_str.empty()) {
                try {
                    int pos = std::stoi(pos_str);
                    if (browser->MoveTab(id, pos)) {
                        std::cout << "✅ Sekme taşındı: " << id << " -> pozisyon " << pos << std::endl;
                    } else {
                        std::cout << "❌ Sekme taşınamadı: " << id << std::endl;
                    }
                } catch (...) {
                    std::cout << "❌ Geçersiz pozisyon: " << pos_str << std::endl;
                }
            } else {
                std::cout << "Kullanım: move <tab_id> <position>" << std::endl;
            }
        } else if (cmd == "details") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                ShowTabDetails(id);
            } else {
                std::cout << "Kullanım: details <tab_id>" << std::endl;
            }
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ProcessBookmarkCommand(const std::string& cmd, std::istringstream& iss) {
        if (cmd == "add") {
            std::string title, url;
            iss >> title >> url;
            if (!title.empty() && !url.empty()) {
                std::string bookmark_id = browser->AddBookmark(title, url);
                std::cout << "✅ Yer işareti eklendi: " << bookmark_id << std::endl;
            } else {
                std::cout << "Kullanım: add <title> <url>" << std::endl;
            }
        } else if (cmd == "remove") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->RemoveBookmark(id)) {
                    std::cout << "✅ Yer işareti silindi: " << id << std::endl;
                } else {
                    std::cout << "❌ Yer işareti silinemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: remove <bookmark_id>" << std::endl;
            }
        } else if (cmd == "update") {
            std::string id, title, url;
            iss >> id >> title >> url;
            if (!id.empty() && !title.empty() && !url.empty()) {
                if (browser->UpdateBookmark(id, title, url)) {
                    std::cout << "✅ Yer işareti güncellendi: " << id << std::endl;
                } else {
                    std::cout << "❌ Yer işareti güncellenemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: update <bookmark_id> <title> <url>" << std::endl;
            }
        } else if (cmd == "folder") {
            std::string name;
            iss >> name;
            if (!name.empty()) {
                std::string folder_id = browser->CreateBookmarkFolder(name);
                std::cout << "✅ Klasör oluşturuldu: " << folder_id << std::endl;
            } else {
                std::cout << "Kullanım: folder <folder_name>" << std::endl;
            }
        } else if (cmd == "search") {
            std::string query;
            std::getline(iss, query);
            if (!query.empty()) {
                // Remove leading space
                if (!query.empty() && query[0] == ' ') {
                    query = query.substr(1);
                }
                
                auto results = browser->GetAllBookmarks();
                std::cout << "🔍 Arama sonuçları: " << query << std::endl;
                std::cout << std::string(60, '-') << std::endl;
                
                int found = 0;
                for (const auto& bookmark : results) {
                    std::string lower_title = bookmark->title;
                    std::string lower_query = query;
                    std::transform(lower_title.begin(), lower_title.end(), lower_title.begin(), ::tolower);
                    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
                    
                    if (lower_title.find(lower_query) != std::string::npos) {
                        std::cout << "  ⭐ " << bookmark->title << std::endl;
                        std::cout << "    🌐 " << bookmark->url << std::endl;
                        found++;
                    }
                }
                
                if (found == 0) {
                    std::cout << "  📭 Sonuç bulunamadı" << std::endl;
                }
                
                std::cout << std::string(60, '-') << std::endl;
            } else {
                std::cout << "Kullanım: search <query>" << std::endl;
            }
        } else if (cmd == "toggle_folders") {
            show_bookmarks_folders = !show_bookmarks_folders;
            std::cout << "✅ Klasör gösterimi " << (show_bookmarks_folders ? "açıldı" : "kapatıldı") << std::endl;
            ShowBookmarks();
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ProcessHistoryCommand(const std::string& cmd, std::istringstream& iss) {
        if (cmd == "search") {
            std::string query;
            std::getline(iss, query);
            if (!query.empty()) {
                // Remove leading space
                if (!query.empty() && query[0] == ' ') {
                    query = query.substr(1);
                }
                
                auto results = browser->SearchHistory(query);
                std::cout << "🔍 Geçmiş arama sonuçları: " << query << std::endl;
                std::cout << std::string(60, '-') << std::endl;
                
                for (size_t i = 0; i < results.size(); ++i) {
                    RenderHistoryEntry(*results[i], i + 1);
                }
                
                if (results.empty()) {
                    std::cout << "  📭 Sonuç bulunamadı" << std::endl;
                }
                
                std::cout << std::string(60, '-') << std::endl;
            } else {
                std::cout << "Kullanım: search <query>" << std::endl;
            }
        } else if (cmd == "clear") {
            browser->ClearHistory();
            std::cout << "✅ Geçmiş temizlendi" << std::endl;
        } else if (cmd == "remove") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                if (browser->RemoveFromHistory(id)) {
                    std::cout << "✅ Geçmiş girdisi silindi: " << id << std::endl;
                } else {
                    std::cout << "❌ Geçmiş girdisi silinemedi: " << id << std::endl;
                }
            } else {
                std::cout << "Kullanım: remove <history_id>" << std::endl;
            }
        } else if (cmd == "more") {
            history_items_per_page += 5;
            std::cout << "✅ Geçmiş sayısı artırıldı: " << history_items_per_page << std::endl;
            ShowHistory();
        } else if (cmd == "less") {
            history_items_per_page = std::max(5, history_items_per_page - 5);
            std::cout << "✅ Geçmiş sayısı azaltıldı: " << history_items_per_page << std::endl;
            ShowHistory();
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ProcessSettingsCommand(const std::string& cmd, std::istringstream& iss) {
        if (cmd == "toggle_folders") {
            show_bookmarks_folders = !show_bookmarks_folders;
            std::cout << "✅ Klasör gösterimi " << (show_bookmarks_folders ? "açıldı" : "kapatıldı") << std::endl;
            ShowSettings();
        } else if (cmd == "set_history_count") {
            std::string count_str;
            iss >> count_str;
            try {
                int count = std::stoi(count_str);
                if (count > 0 && count <= 100) {
                    history_items_per_page = count;
                    std::cout << "✅ Geçmiş sayısı ayarlandı: " << count << std::endl;
                } else {
                    std::cout << "❌ Sayı 1-100 arasında olmalı" << std::endl;
                }
            } catch (...) {
                std::cout << "❌ Geçersiz sayı: " << count_str << std::endl;
            }
            ShowSettings();
        } else if (cmd == "reset_stats") {
            browser->ResetStatistics();
            std::cout << "✅ İstatistikler sıfırlandı" << std::endl;
            ShowSettings();
        } else if (cmd == "export_bookmarks") {
            std::cout << "📤 Yer işaretleri dışa aktarılıyor..." << std::endl;
            // In a real implementation, this would export to a file
            auto bookmarks = browser->GetAllBookmarks();
            std::cout << "Toplam " << bookmarks.size() << " yer işareti dışa aktarıldı" << std::endl;
        } else if (cmd == "import_bookmarks") {
            std::cout << "📥 Yer işaretleri içe aktarılıyor..." << std::endl;
            // In a real implementation, this would import from a file
            std::cout << "Yer işaretleri içe aktarıldı" << std::endl;
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ShowQuickCommands() {
        ClearScreen();
        
        std::cout << "⚡ HIZLI KOMUTLAR ⚡" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "📑 Sekme Komutları:" << std::endl;
        std::cout << "  nt <url>              - Yeni sekme aç" << std::endl;
        std::cout << "  ct <id>               - Sekmeyi kapat" << std::endl;
        std::cout << "  at <id>               - Sekmeyi aktif et" << std::endl;
        std::cout << "  nav <id> <url>        - Sekmede git" << std::endl;
        std::cout << "  back <id>             - Geri git" << std::endl;
        std::cout << "  fwd <id>              - İleri git" << std::endl;
        std::cout << "  reload <id>           - Sekmeyi yenile" << std::endl;
        
        std::cout << "\n⭐ Yer İşareti Komutları:" << std::endl;
        std::cout << "  bm add <title> <url>  - Yer işareti ekle" << std::endl;
        std::cout << "  bm remove <id>        - Yer işaretini sil" << std::endl;
        std::cout << "  bm search <query>     - Arama yap" << std::endl;
        
        std::cout << "\n📚 Geçmiş Komutları:" << std::endl;
        std::cout << "  hist search <query>   - Geçmişte ara" << std::endl;
        std::cout << "  hist clear            - Geçmişi temizle" << std::endl;
        
        std::cout << "\n🎯 Navigasyon Komutları:" << std::endl;
        std::cout << "  go <url>             - Aktif sekmede git" << std::endl;
        std::cout << "  back                  - Geri git" << std::endl;
        std::cout << "  forward               - İleri git" << std::endl;
        std::cout << "  reload                - Sekmeyi yenile" << std::endl;
        
        std::cout << "\n🔧 Sistem Komutları:" << std::endl;
        std::cout << "  stats                 - İstatistikleri göster" << std::endl;
        std::cout << "  clear                 - Ekranı temizle" << std::endl;
        std::cout << "  help                  - Yardım" << std::endl;
        std::cout << "  exit                  - Çıkış" << std::endl;
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Devam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    void ShowTabDetails(const std::string& tab_id) {
        auto tab = browser->GetTab(tab_id);
        if (!tab) {
            std::cout << "❌ Sekme bulunamadı: " << tab_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        
        ClearScreen();
        
        std::cout << "📋 SEKME DETAYLARI 📋" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "🆔 Sekme ID: " << tab->id << std::endl;
        std::cout << "📝 Başlık: " << tab->GetFormattedTitle() << std::endl;
        std::cout << "🌐 URL: " << tab->url << std::endl;
        std::cout << "📊 Durum: " << GetTabStatusString(*tab) << std::endl;
        
        if (tab->is_loading) {
            std::cout << "⏳ Yükleme Progress: " << std::fixed << std::setprecision(1) 
                      << (tab->load_progress * 100) << "%" << std::endl;
        }
        
        std::cout << "📅 Oluşturulma: " << std::fixed << std::setprecision(1) 
                  << tab->GetAgeInSeconds() << " saniye önce" << std::endl;
        std::cout << "🔄 Son Erişim: " << std::fixed << std::setprecision(1) 
                  << tab->GetLastAccessAgeInSeconds() << " saniye önce" << std::endl;
        
        std::cout << "\n🎯 Durum:" << std::endl;
        std::cout << "  Aktif: " << (tab->is_active ? "✅" : "❌") << std::endl;
        std::cout << "  Sabitli: " << (tab->is_pinned ? "✅" : "❌") << std::endl;
        std::cout << "  Sessiz: " << (tab->is_muted ? "✅" : "❌") << std::endl;
        
        std::cout << "\n🔄 Navigasyon:" << std::endl;
        std::cout << "  Geriye gidebilir: " << (tab->CanGoBack() ? "✅" : "❌") << std::endl;
        std::cout << "  İleriye gidebilir: " << (tab->CanGoForward() ? "✅" : "❌") << std::endl;
        
        if (tab->last_response.IsSuccess()) {
            std::cout << "\n📄 Son Yanıt:" << std::endl;
            std::cout << "  Status: " << tab->last_response.status_code << std::endl;
            std::cout << "  Boyut: " << BrowserUtils::GetFileSizeString(tab->last_response.content_length) << std::endl;
            std::cout << "  Content-Type: " << tab->last_response.content_type << std::endl;
        }
        
        if (!tab->error_message.empty()) {
            std::cout << "\n❌ Hata: " << tab->error_message << std::endl;
        }
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Devam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    void ShowHelp() {
        ClearScreen();
        
        std::cout << "❓ YARDIM ❓" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "🎋 Ana Menü Komutları:" << std::endl;
        std::cout << "  1              - Sekme yönetimi" << std::endl;
        std::cout << "  2              - Yer işaretleri" << std::endl;
        std::cout << "  3              - Geçmiş" << std::endl;
        std::cout << "  4              - Ayarlar" << std::endl;
        std::cout << "  5              - Hızlı komutlar" << std::endl;
        std::cout << "  6              - Yardım" << std::endl;
        std::cout << "  7              - Çıkış" << std::endl;
        
        std::cout << "\n📑 Sekme Komutları:" << std::endl;
        std::cout << "  new <url>             - Yeni sekme aç" << std::endl;
        std::cout << "  close <id>            - Sekmeyi kapat" << std::endl;
        std::cout << "  activate <id>         - Sekmeyi aktif et" << std::endl;
        std::cout << "  navigate <id> <url>   - Sekmede git" << std::endl;
        std::cout << "  back <id>             - Geri git" << std::endl;
        std::cout << "  forward <id>          - İleri git" << std::endl;
        std::cout << "  reload <id>           - Sekmeyi yenile" << std::endl;
        std::cout << "  duplicate <id>        - Sekmeyi kopyala" << std::endl;
        std::cout << "  pin <id>              - Sekmeyi sabitle" << std::endl;
        std::cout << "  mute <id>             - Sekmeyi sessize al" << std::endl;
        std::cout << "  move <id> <pos>      - Sekmeyi taşı" << std::endl;
        std::cout << "  details <id>          - Sekme detayları" << std::endl;
        
        std::cout << "\n⭐ Yer İşareti Komutları:" << std::endl;
        std::cout << "  add <title> <url>    - Yer işareti ekle" << std::endl;
        std::cout << "  remove <id>          - Yer işaretini sil" << std::endl;
        std::cout << "  update <id> <title> <url> - Yer işaretini güncelle" << std::endl;
        std::cout << "  folder <name>        - Klasör oluştur" << std::endl;
        std::cout << "  search <query>       - Arama yap" << std::endl;
        std::cout << "  toggle_folders       - Klasörleri göster/gizle" << std::endl;
        
        std::cout << "\n📚 Geçmiş Komutları:" << std::endl;
        std::cout << "  search <query>        - Geçmişte ara" << std::endl;
        std::cout << "  clear                - Geçmişi temizle" << std::endl;
        std::cout << "  remove <id>          - Girdiyi sil" << std::endl;
        std::cout << "  more                 - Daha fazla göster" << std::endl;
        std::cout << "  less                 - Daha az göster" << std::endl;
        
        std::cout << "\n⚡ Hızlı Komutlar:" << std::endl;
        std::cout << "  nt <url>              - Yeni sekme (new tab)" << std::endl;
        std::cout << "  ct <id>               - Sekme kapat (close tab)" << std::endl;
        std::cout << "  at <id>               - Sekme aktif (activate tab)" << std::endl;
        std::cout << "  nav <id> <url>        - Sekmeye git (navigate)" << std::endl;
        std::cout << "  go <url>             - Aktif sekmede git" << std::endl;
        std::cout << "  back                  - Geri git (active tab)" << std::endl;
        std::cout << "  forward               - İleri git (active tab)" << std::endl;
        std::cout << "  reload                - Sekmeyi yenile (active tab)" << std::endl;
        std::cout << "  bm add <title> <url>  - Yer işareti ekle" << std::endl;
        std::cout << "  bm search <query>     - Yer işaretlerinde ara" << std::endl;
        std::cout << "  hist search <query>   - Geçmişte ara" << std::endl;
        std::cout << "  stats                 - İstatistikleri göster" << std::endl;
        
        std::cout << "\n🔥 İpuçları:" << std::endl;
        std::cout << "  • Sekmeler arasında kolayca geçiş yapabilirsiniz" << std::endl;
        std::cout << "  • Yer işaretlerini klasörlerde organize edebilirsiniz" << std::endl;
        std::cout << "  • Geçmiş araması ile hızlıca bulunabilirsiniz" << std::endl;
        std::cout << "  • Sekmeleri sabitleyebilir ve sessize alabilirsiniz" << std::endl;
        std::cout << "  • Hızlı komutlar ile işlemleri hızlandırabilirsiniz" << std::endl;
        std::cout << "  • Detaylı istatistikler ile kullanımı izleyebilirsiniz" << std::endl;
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Devam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    // Utility methods
    void ClearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    
    void RefreshDisplay() {
        // In a real implementation, this would refresh the UI
        // For now, we'll just print a message
        std::cout << "[UI] Display refreshed" << std::endl;
    }
    
    void UpdateURLDisplay(const std::string& url) {
        // In a real implementation, this would update the address bar
        std::cout << "[UI] URL updated: " << url << std::endl;
    }
    
    std::string GetTabStatusString(const Tab& tab) {
        if (tab.is_loading) {
            return "⏳ Yükleniyor (" + std::to_string(static_cast<int>(tab.load_progress * 100)) + "%)";
        } else if (!tab.error_message.empty()) {
            return "❌ Hata";
        } else {
            return "✅ Hazır";
        }
    }
    
    std::string GetTabStatusIcon(const Tab& tab) {
        if (tab.is_loading) {
            return "⏳";
        } else if (!tab.error_message.empty()) {
            return "❌";
        } else if (tab.is_pinned) {
            return "📌";
        } else if (tab.is_muted) {
            return "🔇";
        } else {
            return "📑";
        }
    }
};

int main() {
    try {
        std::cout << "🎯 Comper Engine SDK - Tab Browser Example" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "Modern Sekme Yönetimi" << std::endl;
        std::cout << "Yer İşaretleri ve Geçmiş" << std::endl;
        std::cout << "Canlı URL Değişimi" << std::endl;
        std::cout << "Night Red Ultra Teması" << std::endl;
        std::cout << "=================================================" << std::endl;
        
        // Tab browser'ı oluştur ve çalıştır
        TabBrowserExample tab_browser;
        tab_browser.RunInteractive();
        
        std::cout << "\n👋 Görüşürüz!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
 * 🎯 BU ÖRNEKTE NE ÖĞRENDİNİZ?
 * 
 * ✅ Tab management system
 * ✅ Bookmark management with folders
 * ✅ History tracking and search
 * ✅ Live URL updates
 * ✅ Navigation controls
 * ✅ Session management
 * ✅ Interactive UI
 * ✅ Event handling
 * ✅ Statistics tracking
 * 
 * 🚀 TAB BROWSER ÖZELLİKLERİ:
 * 
 * 📑 Multi-tab: Sınırsız sekme sayısı
 * ⭐ Bookmarks: Klasör sistemi ile
 * 📚 History: Arama ve yönetim
 * 🔄 Navigation: Geri/ileri/refresh
 * 📊 Statistics: Detaylı kullanım verileri
 * 🎨 UI: Interactive command line
 * 🔔 Events: Real-time güncellemeler
 * 
 * 🔧 KULLANIM:
 * 
 * 1. Browser'ı başlat
 * 2. Sekmeleri yönet (oluştur/kapat/aktif et)
 * 3. Yer işaretleri ekle/kaldır
 * 4. Geçmişi yönet
 * 5. Navigasyon kontrollerini kullan
 * 6. İstatistikleri izle
 * 
 * 📚 DAHA FAZLA BİLGİ İÇİN:
 * 
 * - [Browser API Documentation](../../API/Browser_API.md)
 * - [Core API Documentation](../../API/Core_API.md)
 * - [Download Manager Example](../DownloadManager/)
 * - [Professional Browser Example](../ProfessionalBrowser/)
 */
