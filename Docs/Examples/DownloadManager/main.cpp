/*
 * Comper Engine SDK - Download Manager Example
 * 
 * İndirme yönetimi sistemi örneği
 * Multi-threaded indirme, progress tracking, pause/resume desteği
 * 
 * Features:
 * - Multi-threaded downloading
 * - Progress tracking with ETA
 * - Pause/Resume support
 * - File integrity verification
 * - Malware scanning
 * - Download queue management
 * - Interactive UI
 * - Download history
 */

#include "../../../Includes/Core/comper_core.h"
#include "../../../Includes/Core/comper_download.h"

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace ComperCore;

class DownloadManagerExample {
private:
    std::shared_ptr<DownloadManager> download_manager;
    std::shared_ptr<DownloadPage> download_page;
    bool is_running = true;
    
    // UI state
    enum class DisplayMode {
        MAIN_MENU,
        ACTIVE_DOWNLOADS,
        DOWNLOAD_HISTORY,
        ADD_DOWNLOAD,
        DOWNLOAD_DETAILS
    } current_mode = DisplayMode::MAIN_MENU;
    
    std::string selected_download_id;
    
public:
    DownloadManagerExample() {
        std::cout << "🚀 Comper Download Manager başlatılıyor..." << std::endl;
        
        // Download manager'ı oluştur
        download_manager = DownloadFactory::CreateDownloadManager();
        
        // Download page'i oluştur
        download_page = DownloadFactory::CreateDownloadPage(download_manager);
        
        // Konfigürasyonu yap
        SetupConfiguration();
        
        // Event handlers'ı ayarla
        SetupEventHandlers();
        
        std::cout << "✅ Download Manager hazır!" << std::endl;
    }
    
    ~DownloadManagerExample() {
        std::cout << "\n📊 Final İstatistikler:" << std::endl;
        ShowStatistics();
        std::cout << "👋 Download Manager kapatılıyor..." << std::endl;
    }
    
private:
    void SetupConfiguration() {
        std::cout << "⚙️ Konfigürasyon ayarlanıyor..." << std::endl;
        
        // İndirme yolu
        std::string download_path = "./Downloads";
        download_manager->SetDefaultDownloadPath(download_path);
        
        // Eşzamanlı indirme limiti
        download_manager->SetMaxConcurrentDownloads(3);
        
        // Hız limiti (1MB/s)
        download_manager->SetMaxDownloadSpeed(1024 * 1024);
        
        // Güvenlik ayarları
        download_manager->SetVerifyDownloads(true);
        download_manager->SetScanDownloads(true);
        
        std::cout << "✅ Konfigürasyon tamamlandı!" << std::endl;
    }
    
    void SetupEventHandlers() {
        std::cout << "🔧 Event handlers ayarlanıyor..." << std::endl;
        
        // Progress callback
        download_manager->on_download_progress = [this](const DownloadItem& download) {
            // Progress'i güncelle (sadece aktif indirme modunda göster)
            if (current_mode == DisplayMode::ACTIVE_DOWNLOADS) {
                // Ekranı temizle ve progress'i göster
                ShowActiveDownloads();
            }
        };
        
        // Completed callback
        download_manager->on_download_completed = [this](const DownloadItem& download) {
            std::cout << "\n✅ İndirme tamamlandı: " << download.filename 
                      << " (" << download.GetFormattedSize() << ")" << std::endl;
            std::cout << "📁 Konum: " << download.destination_path << std::endl;
            std::cout << "⏱️ Süre: " << GetFormattedDuration(download.start_time) << std::endl;
        };
        
        // Failed callback
        download_manager->on_download_failed = [this](const DownloadItem& download, const std::string& error) {
            std::cout << "\n❌ İndirme başarısız: " << download.filename << std::endl;
            std::cout << "🔧 Hata: " << error << std::endl;
        };
        
        std::cout << "✅ Event handlers hazır!" << std::endl;
    }
    
public:
    void RunInteractive() {
        std::cout << "\n🎯 Comper Download Manager - Interactive Mode" << std::endl;
        std::cout << "Multi-threaded İndirme Yönetimi" << std::endl;
        std::cout << "Night Red Ultra Teması" << std::endl;
        std::cout << "Yardım için 'help' yazın" << std::endl;
        
        // Örnek indirmeler ekle
        AddSampleDownloads();
        
        // Ana döngü
        std::string command;
        while (is_running) {
            ShowMainMenu();
            
            std::cout << "\nDownloadManager> ";
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            ProcessCommand(command);
        }
    }
    
private:
    void AddSampleDownloads() {
        std::cout << "\n📦 Örnek indirmeler ekleniyor..." << std::endl;
        
        // Örnek dosyalar
        std::vector<std::pair<std::string, std::string>> sample_downloads = {
            {"https://www.learningcontainer.com/wp-content/uploads/2020/05/sample-mp4-file.mp4", "sample-video.mp4"},
            {"https://www.learningcontainer.com/wp-content/uploads/2020/05/sample-zip-file.zip", "sample-archive.zip"},
            {"https://www.learningcontainer.com/wp-content/uploads/2020/04/sample-pdf-file.pdf", "sample-document.pdf"}
        };
        
        for (const auto& download : sample_downloads) {
            std::string download_id = download_manager->AddDownload(download.first, download.second, DownloadPriority::NORMAL);
            if (!download_id.empty()) {
                std::cout << "✅ İndirme eklendi: " << download_id << " -> " << download.second << std::endl;
            }
        }
        
        std::cout << "📦 " << sample_downloads.size() << " örnek indirme eklendi" << std::endl;
    }
    
    void ShowMainMenu() {
        ClearScreen();
        
        std::cout << "🌟 COMPER DOWNLOAD MANAGER 🌟" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        // İstatistikler
        ShowQuickStats();
        
        std::cout << "\n📋 MENÜ:" << std::endl;
        std::cout << "  1. Aktif İndirmeler" << std::endl;
        std::cout << "  2. İndirme Geçmişi" << std::endl;
        std::cout << "  3. İndirme Ekle" << std::endl;
        std::cout << "  4. İndirme Detayları" << std::endl;
        std::cout << "  5. İstatistikler" << std::endl;
        std::cout << "  6. Ayarlar" << std::endl;
        std::cout << "  7. Yardım" << std::endl;
        std::cout << "  8. Çıkış" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    void ShowQuickStats() {
        auto stats = download_manager->GetStatistics();
        auto active_downloads = download_manager->GetActiveDownloads();
        auto completed_downloads = download_manager->GetCompletedDownloads();
        
        std::cout << "📊 Hızlı İstatistikler:" << std::endl;
        std::cout << "  Toplam İndirme: " << stats.total_downloads.load() << std::endl;
        std::cout << "  Tamamlanan: " << stats.completed_downloads.load() << std::endl;
        std::cout << "  Başarısız: " << stats.failed_downloads.load() << std::endl;
        std::cout << "  Aktif: " << active_downloads.size() << std::endl;
        std::cout << "  Toplam Boyut: " << DownloadUtils::GetFileSizeString(stats.total_bytes_downloaded.load()) << std::endl;
    }
    
    void ShowActiveDownloads() {
        current_mode = DisplayMode::ACTIVE_DOWNLOADS;
        ClearScreen();
        
        std::cout << "🔄 AKTİF İNDEMELER 🔄" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto active_downloads = download_manager->GetActiveDownloads();
        
        if (active_downloads.empty()) {
            std::cout << "📭 Aktif indirme bulunmamaktadır." << std::endl;
        } else {
            for (const auto& download : active_downloads) {
                RenderDownloadItem(*download, true);
            }
        }
        
        std::cout << "\n🔧 KOMUTLAR:" << std::endl;
        std::cout << "  pause <id>     - İndirmeyi duraklat" << std::endl;
        std::cout << "  resume <id>    - İndirmeyi devam ettir" << std::endl;
        std::cout << "  cancel <id>    - İndirmeyi iptal et" << std::endl;
        std::cout << "  retry <id>     - İndirmeyi yeniden dene" << std::endl;
        std::cout << "  details <id>   - İndirme detayları" << std::endl;
        std::cout << "  menu           - Ana menü" << std::endl;
        std::cout << "  refresh        - Listeyi yenile" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowDownloadHistory() {
        current_mode = DisplayMode::DOWNLOAD_HISTORY;
        ClearScreen();
        
        std::cout << "📚 İNDİRME GEÇMİŞİ 📚" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto completed_downloads = download_manager->GetCompletedDownloads();
        
        if (completed_downloads.empty()) {
            std::cout << "📭 Tamamlanmış indirme bulunmamaktadır." << std::endl;
        } else {
            for (const auto& download : completed_downloads) {
                RenderDownloadItem(*download, false);
            }
        }
        
        std::cout << "\n🔧 KOMUTLAR:" << std::endl;
        std::cout << "  details <id>   - İndirme detayları" << std::endl;
        std::cout << "  remove <id>    - İndirmeyi sil" << std::endl;
        std::cout << "  menu           - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowAddDownload() {
        current_mode = DisplayMode::ADD_DOWNLOAD;
        ClearScreen();
        
        std::cout << "➕ İNDİRME EKLE ➕" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        std::string url, filename;
        int priority_choice = 2; // Normal
        
        std::cout << "📎 URL: ";
        std::getline(std::cin, url);
        
        if (url.empty()) {
            std::cout << "❌ URL boş olamaz!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        
        if (!download_manager->IsValidURL(url)) {
            std::cout << "❌ Geçersiz URL!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        
        std::cout << "📁 Dosya Adı (boş bırakırsanız otomatik): ";
        std::getline(std::cin, filename);
        
        std::cout << "🎯 Öncelik:" << std::endl;
        std::cout << "  1. Düşük" << std::endl;
        std::cout << "  2. Normal" << std::endl;
        std::cout << "  3. Yüksek" << std::endl;
        std::cout << "  4. Acil" << std::endl;
        std::cout << "Seçiminiz (1-4): ";
        std::string priority_str;
        std::getline(std::cin, priority_str);
        
        try {
            int choice = std::stoi(priority_str);
            if (choice >= 1 && choice <= 4) {
                priority_choice = choice;
            }
        } catch (...) {
            // Default to Normal
        }
        
        DownloadPriority priority = static_cast<DownloadPriority>(priority_choice);
        
        // İndirmeyi ekle
        std::string download_id = download_manager->AddDownload(url, filename, priority);
        
        if (!download_id.empty()) {
            std::cout << "✅ İndirme eklendi: " << download_id << std::endl;
            
            // İndirme detaylarını göster
            auto download = download_manager->GetDownload(download_id);
            if (download) {
                std::cout << "📎 URL: " << download->url << std::endl;
                std::cout << "📁 Dosya: " << download->filename << std::endl;
                std::cout << "🎯 Öncelik: " << GetPriorityString(priority) << std::endl;
            }
        } else {
            std::cout << "❌ İndirme eklenemedi!" << std::endl;
        }
        
        std::cout << "\nDevam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    void ShowDownloadDetails(const std::string& download_id) {
        current_mode = DisplayMode::DOWNLOAD_DETAILS;
        ClearScreen();
        
        std::cout << "📋 İNDİRME DETAYLARI 📋" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto download = download_manager->GetDownload(download_id);
        if (!download) {
            std::cout << "❌ İndirme bulunamadı: " << download_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return;
        }
        
        RenderDownloadDetails(*download);
        
        std::cout << "\n🔧 KOMUTLAR:" << std::endl;
        
        if (download->IsDownloading()) {
            std::cout << "  pause          - İndirmeyi duraklat" << std::endl;
        } else if (download->IsPaused()) {
            std::cout << "  resume         - İndirmeyi devam ettir" << std::endl;
        }
        
        if (download->IsFailed()) {
            std::cout << "  retry          - İndirmeyi yeniden dene" << std::endl;
        }
        
        std::cout << "  cancel         - İndirmeyi iptal et" << std::endl;
        std::cout << "  remove         - İndirmeyi sil" << std::endl;
        std::cout << "  menu           - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        selected_download_id = download_id;
    }
    
    void ShowStatistics() {
        ClearScreen();
        
        std::cout << "📊 İSTATİSTİKLER 📊" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        auto stats = download_manager->GetStatistics();
        
        std::cout << "🎯 Genel İstatistikler:" << std::endl;
        std::cout << "  Toplam İndirme: " << stats.total_downloads.load() << std::endl;
        std::cout << "  Tamamlanan: " << stats.completed_downloads.load() << std::endl;
        std::cout << "  Başarısız: " << stats.failed_downloads.load() << std::endl;
        std::cout << "  Başarı Oranı: " << CalculateSuccessRate(stats) << "%" << std::endl;
        
        std::cout << "\n📏 Boyut İstatistikleri:" << std::endl;
        std::cout << "  Toplam İndirilen: " << DownloadUtils::GetFileSizeString(stats.total_bytes_downloaded.load()) << std::endl;
        
        auto all_downloads = download_manager->GetAllDownloads();
        if (!all_downloads.empty()) {
            size_t total_size = 0;
            for (const auto& download : all_downloads) {
                total_size += download->total_size;
            }
            
            std::cout << "  Toplam Boyut: " << DownloadUtils::GetFileSizeString(total_size) << std::endl;
        }
        
        std::cout << "\n⏱️ Zaman İstatistikleri:" << std::endl;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - stats.start_time);
        std::cout << "  Çalışma Süresi: " << duration.count() << " dakika" << std::endl;
        
        std::cout << "\n🔄 Aktif İndirmeler:" << std::endl;
        auto active_downloads = download_manager->GetActiveDownloads();
        std::cout << "  Aktif Sayısı: " << active_downloads.size() << std::endl;
        
        if (!active_downloads.empty()) {
            double total_speed = 0;
            for (const auto& download : active_downloads) {
                total_speed += download->speed;
            }
            std::cout << "  Toplam Hız: " << DownloadUtils::GetSpeedString(total_speed) << std::endl;
        }
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Devam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    void ShowSettings() {
        ClearScreen();
        
        std::cout << "⚙️ AYARLAR ⚙️" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "📁 İndirme Ayarları:" << std::endl;
        std::cout << "  İndirme Yolu: " << download_manager->GetDefaultDownloadPath() << std::endl;
        std::cout << "  Eşzamanlı İndirme: 3" << std::endl;
        std::cout << "  Hız Limiti: 1MB/s" << std::endl;
        
        std::cout << "\n🛡️ Güvenlik Ayarları:" << std::endl;
        std::cout << "  Dosya Doğrulama: ✅ Aktif" << std::endl;
        std::cout << "  Malware Taraması: ✅ Aktif" << std::endl;
        
        std::cout << "\n🎨 UI Ayarları:" << std::endl;
        std::cout << "  Otomatik Yenileme: ✅ Aktif" << std::endl;
        std::cout << "  Progress Gösterimi: ✅ Aktif" << std::endl;
        
        std::cout << "\n🔧 KOMUTLAR:" << std::endl;
        std::cout << "  reset_stats    - İstatistikleri sıfırla" << std::endl;
        std::cout << "  clear_failed   - Başarısız indirmeleri temizle" << std::endl;
        std::cout << "  clear_completed - Tamamlananları temizle" << std::endl;
        std::cout << "  menu           - Ana menü" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    void ShowHelp() {
        ClearScreen();
        
        std::cout << "❓ YARDIM ❓" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::cout << "🎋 Ana Menü Komutları:" << std::endl;
        std::cout << "  1              - Aktif indirmeleri göster" << std::endl;
        std::cout << "  2              - İndirme geçmişini göster" << std::endl;
        std::cout << "  3              - Yeni indirme ekle" << std::endl;
        std::cout << "  4              - İndirme detayları" << std::endl;
        std::cout << "  5              - İstatistikleri göster" << std::endl;
        std::cout << "  6              - Ayarları göster" << std::endl;
        std::cout << "  7              - Yardımı göster" << std::endl;
        std::cout << "  8              - Çıkış" << std::endl;
        
        std::cout << "\n🔄 İndirme Komutları:" << std::endl;
        std::cout << "  pause <id>     - İndirmeyi duraklat" << std::endl;
        std::cout << "  resume <id>    - İndirmeyi devam ettir" << std::endl;
        std::cout << "  cancel <id>    - İndirmeyi iptal et" << std::endl;
        std::cout << "  retry <id>     - İndirmeyi yeniden dene" << std::endl;
        std::cout << "  remove <id>    - İndirmeyi sil" << std::endl;
        std::cout << "  details <id>   - İndirme detayları" << std::endl;
        
        std::cout << "\n⚙️ Ayar Komutları:" << std::endl;
        std::cout << "  reset_stats    - İstatistikleri sıfırla" << std::endl;
        std::cout << "  clear_failed   - Başarısızları temizle" << std::endl;
        std::cout << "  clear_completed - Tamamlananları temizle" << std::endl;
        
        std::cout << "\n🎯 URL Formatları:" << std::endl;
        std::cout << "  http://example.com/file.zip" << std::endl;
        std::cout << "  https://example.com/file.pdf" << std::endl;
        std::cout << "  ftp://example.com/file.exe" << std::endl;
        
        std::cout << "\n🔥 İpuçları:" << std::endl;
        std::cout << "  • Birden fazla indirme aynı anda çalışabilir" << std::endl;
        std::cout << "  • İndirilen dosyalar otomatik doğrulanır" << std::endl;
        std::cout << "  • İndirmeler duraklatılıp devam ettirilebilir" << std::endl;
        std::cout << "  • İndirme geçmişi saklanır" << std::endl;
        std::cout << "  • Malware taraması güvenlik sağlar" << std::endl;
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Devam etmek için Enter'a basın..." << std::endl;
        std::cin.get();
    }
    
    void RenderDownloadItem(const DownloadItem& download, bool show_progress) {
        std::cout << "\n📦 " << download.id << std::endl;
        std::cout << "📎 URL: " << download.url << std::endl;
        std::cout << "📁 Dosya: " << download.filename << std::endl;
        std::cout << "📊 Durum: " << GetStatusString(download.status) << std::endl;
        
        if (show_progress && (download.IsDownloading() || download.IsPaused())) {
            std::cout << "📈 Progress: " << GetProgressBar(download.progress, 30) << std::endl;
            std::cout << "📏 Boyut: " << download.GetFormattedSize() << std::endl;
            std::cout << "⚡ Hız: " << download.GetFormattedSpeed() << std::endl;
            std::cout << "⏱️ ETA: " << download.GetETA() << std::endl;
        }
        
        if (download.IsCompleted()) {
            std::cout << "✅ Tamamlandı: " << GetFormattedDuration(download.start_time) << std::endl;
            std::cout << "📁 Konum: " << download.destination_path << std::endl;
        }
        
        if (download.IsFailed()) {
            std::cout << "❌ Başarısız" << std::endl;
        }
        
        std::cout << std::string(60, '-') << std::endl;
    }
    
    void RenderDownloadDetails(const DownloadItem& download) {
        std::cout << "📋 İndirme Bilgileri:" << std::endl;
        std::cout << "  ID: " << download.id << std::endl;
        std::cout << "  URL: " << download.url << std::endl;
        std::cout << "  Dosya Adı: " << download.filename << std::endl;
        std::cout << "  Geçici Dosya: " << download.temp_filename << std::endl;
        std::cout << "  Hedef Yol: " << download.destination_path << std::endl;
        std::cout << "  Referer: " << download.referer << std::endl;
        std::cout << "  User Agent: " << download.user_agent << std::endl;
        
        std::cout << "\n📊 Durum Bilgileri:" << std::endl;
        std::cout << "  Durum: " << GetStatusString(download.status) << std::endl;
        std::cout << "  Öncelik: " << GetPriorityString(download.priority) << std::endl;
        std::cout << "  Başlangıç: " << GetFormattedDateTime(download.start_time) << std::endl;
        
        if (download.total_size > 0) {
            std::cout << "  Toplam Boyut: " << download.GetFormattedSize() << std::endl;
        }
        
        if (download.downloaded_size > 0) {
            std::cout << "  İndirilen: " << DownloadUtils::GetFileSizeString(download.downloaded_size) << std::endl;
            std::cout << "  Progress: " << download.GetFormattedProgress() << std::endl;
        }
        
        if (download.speed > 0) {
            std::cout << "  Hız: " << download.GetFormattedSpeed() << std::endl;
            std::cout << "  ETA: " << download.GetETA() << std::endl;
        }
        
        std::cout << "\n🛡️ Güvenlik Bilgileri:" << std::endl;
        std::cout << "  Bütünlük Doğrulama: " << (download.verify_integrity ? "✅ Aktif" : "❌ Pasif") << std::endl;
        std::cout << "  Malware Taraması: " << (download.scan_for_malware ? "✅ Aktif" : "❌ Pasif") << std::endl;
        
        if (!download.expected_hash.empty()) {
            std::cout << "  Beklenen Hash: " << download.expected_hash << std::endl;
        }
        
        if (!download.actual_hash.empty()) {
            std::cout << "  Gerçek Hash: " << download.actual_hash << std::endl;
        }
        
        std::cout << "\n🔄 Resume Bilgileri:" << std::endl;
        std::cout << "  Resume Destek: " << (download.supports_resume ? "✅ Evet" : "❌ Hayır") << std::endl;
        std::cout << "  Resume Pozisyonu: " << DownloadUtils::GetFileSizeString(download.resume_position) << std::endl;
    }
    
    void ProcessCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "1" || cmd == "active") {
            ShowActiveDownloads();
        } else if (cmd == "2" || cmd == "history") {
            ShowDownloadHistory();
        } else if (cmd == "3" || cmd == "add") {
            ShowAddDownload();
        } else if (cmd == "4" || cmd == "details") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                ShowDownloadDetails(id);
            } else {
                std::cout << "Kullanım: details <id>" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        } else if (cmd == "5" || cmd == "stats") {
            ShowStatistics();
        } else if (cmd == "6" || cmd == "settings") {
            ShowSettings();
        } else if (cmd == "7" || cmd == "help") {
            ShowHelp();
        } else if (cmd == "8" || cmd == "exit" || cmd == "quit") {
            is_running = false;
        } else if (current_mode == DisplayMode::ACTIVE_DOWNLOADS || 
                   current_mode == DisplayMode::DOWNLOAD_DETAILS) {
            ProcessDownloadCommand(cmd, iss);
        } else if (current_mode == DisplayMode::DOWNLOAD_HISTORY) {
            ProcessHistoryCommand(cmd, iss);
        } else if (current_mode == DisplayMode::ADD_DOWNLOAD) {
            current_mode = DisplayMode::MAIN_MENU;
        } else if (current_mode == DisplayMode::SETTINGS) {
            ProcessSettingsCommand(cmd, iss);
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::cout << "Yardım için 'help' yazın" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    
    void ProcessDownloadCommand(const std::string& cmd, std::istringstream& iss) {
        std::string id;
        iss >> id;
        
        if (cmd == "pause") {
            if (!id.empty()) {
                if (download_manager->PauseDownload(id)) {
                    std::cout << "✅ İndirme duraklatıldı: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme duraklatılamadı: " << id << std::endl;
                }
            }
        } else if (cmd == "resume") {
            if (!id.empty()) {
                if (download_manager->ResumeDownload(id)) {
                    std::cout << "✅ İndirme devam ettirildi: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme devam ettirilemedi: " << id << std::endl;
                }
            }
        } else if (cmd == "cancel") {
            if (!id.empty()) {
                if (download_manager->CancelDownload(id)) {
                    std::cout << "✅ İndirme iptal edildi: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme iptal edilemedi: " << id << std::endl;
                }
            }
        } else if (cmd == "retry") {
            if (!id.empty()) {
                if (download_manager->RetryDownload(id)) {
                    std::cout << "✅ İndirme yeniden dene: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme yeniden denenemedi: " << id << std::endl;
                }
            }
        } else if (cmd == "remove") {
            if (!id.empty()) {
                if (download_manager->RemoveDownload(id)) {
                    std::cout << "✅ İndirme silindi: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme silinemedi: " << id << std::endl;
                }
            }
        } else if (cmd == "details") {
            if (!id.empty()) {
                ShowDownloadDetails(id);
            }
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else if (cmd == "refresh") {
            ShowActiveDownloads();
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ProcessHistoryCommand(const std::string& cmd, std::istringstream& iss) {
        std::string id;
        iss >> id;
        
        if (cmd == "details") {
            if (!id.empty()) {
                ShowDownloadDetails(id);
            }
        } else if (cmd == "remove") {
            if (!id.empty()) {
                if (download_manager->RemoveDownload(id)) {
                    std::cout << "✅ İndirme silindi: " << id << std::endl;
                } else {
                    std::cout << "❌ İndirme silinemedi: " << id << std::endl;
                }
            }
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    void ProcessSettingsCommand(const std::string& cmd, std::istringstream& iss) {
        if (cmd == "reset_stats") {
            download_manager->ResetStatistics();
            std::cout << "✅ İstatistikler sıfırlandı" << std::endl;
        } else if (cmd == "clear_failed") {
            // Başarısız indirmeleri temizle
            auto all_downloads = download_manager->GetAllDownloads();
            int removed_count = 0;
            
            for (const auto& download : all_downloads) {
                if (download->IsFailed()) {
                    if (download_manager->RemoveDownload(download->id)) {
                        removed_count++;
                    }
                }
            }
            
            std::cout << "✅ " << removed_count << " başarısız indirme silindi" << std::endl;
        } else if (cmd == "clear_completed") {
            // Tamamlanan indirmeleri temizle
            auto all_downloads = download_manager->GetAllDownloads();
            int removed_count = 0;
            
            for (const auto& download : all_downloads) {
                if (download->IsCompleted()) {
                    if (download_manager->RemoveDownload(download->id)) {
                        removed_count++;
                    }
                }
            }
            
            std::cout << "✅ " << removed_count << " tamamlanan indirme silindi" << std::endl;
        } else if (cmd == "menu") {
            current_mode = DisplayMode::MAIN_MENU;
        } else {
            std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    // Utility methods
    void ClearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    
    std::string GetStatusString(DownloadStatus status) {
        switch (status) {
            case DownloadStatus::PENDING: return "⏳ Bekliyor";
            case DownloadStatus::DOWNLOADING: return "⬇️ İndiriliyor";
            case DownloadStatus::PAUSED: return "⏸️ Duraklatıldı";
            case DownloadStatus::COMPLETED: return "✅ Tamamlandı";
            case DownloadStatus::FAILED: return "❌ Başarısız";
            case DownloadStatus::CANCELLED: return "🚫 İptal Edildi";
            case DownloadStatus::VERIFYING: return "🔍 Doğrulanıyor";
            default: return "❓ Bilinmeyen";
        }
    }
    
    std::string GetPriorityString(DownloadPriority priority) {
        switch (priority) {
            case DownloadPriority::LOW: return "Düşük";
            case DownloadPriority::NORMAL: return "Normal";
            case DownloadPriority::HIGH: return "Yüksek";
            case DownloadPriority::URGENT: return "Acil";
            default: return "Normal";
        }
    }
    
    std::string GetProgressBar(double progress, int width) {
        std::string bar;
        int filled = static_cast<int>((progress / 100.0) * width);
        
        for (int i = 0; i < width; ++i) {
            if (i < filled) {
                bar += "=";
            } else {
                bar += " ";
            }
        }
        
        return "[" + bar + "] " + std::to_string(static_cast<int>(progress)) + "%";
    }
    
    std::string GetFormattedDuration(std::chrono::time_point<std::chrono::high_resolution_clock> start) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        
        if (duration.count() < 60) {
            return std::to_string(duration.count()) + "s";
        } else if (duration.count() < 3600) {
            return std::to_string(duration.count() / 60) + "m " + std::to_string(duration.count() % 60) + "s";
        } else {
            return std::to_string(duration.count() / 3600) + "s " + 
                   std::to_string((duration.count() % 3600) / 60) + "m";
        }
    }
    
    std::string GetFormattedDateTime(std::chrono::time_point<std::chrono::high_resolution_clock> time_point) {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        std::tm tm = *std::localtime(&time_t);
        
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buffer);
    }
    
    double CalculateSuccessRate(const DownloadManager::Statistics& stats) {
        size_t total = stats.completed_downloads.load() + stats.failed_downloads.load();
        if (total == 0) return 0.0;
        return (double)stats.completed_downloads.load() / total * 100.0;
    }
    
    void ShowStatistics() {
        auto stats = download_manager->GetStatistics();
        
        std::cout << "📊 Son İstatistikler:" << std::endl;
        std::cout << "  Toplam İndirme: " << stats.total_downloads.load() << std::endl;
        std::cout << "  Tamamlanan: " << stats.completed_downloads.load() << std::endl;
        std::cout << "  Başarısız: " << stats.failed_downloads.load() << std::endl;
        std::cout << "  Başarı Oranı: " << std::fixed << std::setprecision(1) 
                  << CalculateSuccessRate(stats) << "%" << std::endl;
        std::cout << "  Toplam Boyut: " << DownloadUtils::GetFileSizeString(stats.total_bytes_downloaded.load()) << std::endl;
    }
};

int main() {
    try {
        std::cout << "🎯 Comper Engine SDK - Download Manager Example" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "Multi-threaded İndirme Yönetimi" << std::endl;
        std::cout << "Progress Tracking & Pause/Resume" << std::endl;
        std::cout << "File Integrity Verification" << std::endl;
        std::cout << "Malware Scanning" << std::endl;
        std::cout << "==================================================" << std::endl;
        
        // Download manager'ı oluştur ve çalıştır
        DownloadManagerExample download_manager;
        download_manager.RunInteractive();
        
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
 * ✅ Download Manager API kullanımı
 * ✅ Multi-threaded indirme
 * ✅ Progress tracking ve ETA hesaplama
 * ✅ Pause/Resume desteği
 * ✅ File integrity verification
 * ✅ Malware scanning
 * ✅ Download queue management
 * ✅ Interactive UI oluşturma
 * ✅ Event handling
 * ✅ Error handling ve recovery
 * 
 * 🚀 DOWNLOAD MANAGER ÖZELLİKLERİ:
 * 
 * 🧵 Multi-threading: 3 worker threads
 * ⏸️ Pause/Resume: İndirme kontrolü
 * 📊 Progress: Real-time progress tracking
 * 🛡️ Security: File verification & malware scanning
 * 📁 Management: Queue ve history
 * 🎨 UI: Interactive command line interface
 * 📈 Statistics: Performance tracking
 * 
 * 🔧 KULLANIM:
 * 
 * 1. Download manager'ı başlat
 * 2. İndirmeler ekle
 * 3. Progress'i izle
 * 4. İndirmeleri yönet (pause/resume/cancel)
 * 5. İstatistikleri görüntüle
 * 6. Geçmişi yönet
 * 
 * 📚 DAHA FAZLA BİLGİ İÇİN:
 * 
 * - [Download API Documentation](../../API/Download_API.md)
 * - [Core API Documentation](../../API/Core_API.md)
 * - [Advanced Browser Example](../AdvancedBrowser/)
 * - [Professional Browser Example](../ProfessionalBrowser/)
 */
