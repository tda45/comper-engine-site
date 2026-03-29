#ifndef COMPER_DOWNLOAD_H
#define COMPER_DOWNLOAD_H

/*
 * Comper Engine Core - Download Module
 * 
 * İndirme yönetimi, dosya indirme, progress tracking, resume desteği
 * Multi-threaded indirme, pause/resume, güvenlik kontrolleri
 * 
 * Features:
 * - Multi-threaded downloading
 * - Pause/Resume support
 * - Progress tracking
 * - File integrity verification
 * - Download queue management
 * - Bandwidth limiting
 * - Security scanning
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>
#include <fstream>
#include <queue>

#ifdef _WIN32
#include <winhttp.h>
#include <urlmon.h>
#include <shlobj.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ole32.lib")
#else
#include <curl/curl.h>
#include <pthread.h>
#endif

namespace ComperCore {

// Download status enumeration
enum class DownloadStatus {
    PENDING,        // İndirme bekliyor
    DOWNLOADING,    // İndiriliyor
    PAUSED,        // Duraklatıldı
    COMPLETED,      // Tamamlandı
    FAILED,         // Başarısız
    CANCELLED,      // İptal edildi
    VERIFYING       // Doğrulanıyor
};

// Download priority
enum class DownloadPriority {
    LOW = 1,
    NORMAL = 2,
    HIGH = 3,
    URGENT = 4
};

// Download structure
struct DownloadItem {
    std::string id;                           // Unique ID
    std::string url;                           // Download URL
    std::string filename;                       // Local filename
    std::string temp_filename;                  // Temporary filename
    std::string destination_path;               // Destination directory
    std::string referer;                       // Referer URL
    std::string user_agent;                    // User agent
    std::map<std::string, std::string> headers; // Custom headers
    
    // Download info
    DownloadStatus status = DownloadStatus::PENDING;
    DownloadPriority priority = DownloadPriority::NORMAL;
    size_t total_size = 0;                     // Total file size
    size_t downloaded_size = 0;                 // Downloaded size
    double progress = 0.0;                     // Progress percentage
    double speed = 0.0;                       // Download speed (bytes/sec)
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_update_time;
    
    // Resume support
    bool supports_resume = false;
    bool is_resumable = false;
    size_t resume_position = 0;
    
    // Security
    std::string expected_hash;                  // Expected file hash
    std::string actual_hash;                    // Actual file hash
    bool verify_integrity = true;
    bool scan_for_malware = false;
    
    // Callbacks
    std::function<void(const DownloadItem&)> on_progress;
    std::function<void(const DownloadItem&)> on_completed;
    std::function<void(const DownloadItem&, const std::string&)> on_failed;
    std::function<void(const DownloadItem&)> on_paused;
    std::function<void(const DownloadItem&)> on_resumed;
    
    // Internal state
    std::atomic<bool> should_cancel{false};
    std::atomic<bool> should_pause{false};
    std::thread download_thread;
    
    DownloadItem(const std::string& download_id, const std::string& download_url, 
                 const std::string& file_path)
        : id(download_id), url(download_url), filename(file_path) {
        // Extract filename from path if not provided
        if (filename.empty()) {
            filename = ExtractFilenameFromURL(url);
        }
        
        // Generate temp filename
        temp_filename = filename + ".tmp";
        
        // Set default user agent
        user_agent = "Comper-Engine-Downloader/1.0";
        
        start_time = std::chrono::high_resolution_clock::now();
        last_update_time = start_time;
    }
    
private:
    std::string ExtractFilenameFromURL(const std::string& url) {
        size_t last_slash = url.find_last_of('/');
        if (last_slash != std::string::npos) {
            return url.substr(last_slash + 1);
        }
        return "download";
    }
    
    void UpdateProgress() {
        if (total_size > 0) {
            progress = (double)downloaded_size / total_size * 100.0;
        }
        
        auto now = std::chrono::high_resolution_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(now - last_update_time);
        
        if (time_diff.count() > 0) {
            speed = (double)downloaded_size / time_diff.count();
            last_update_time = now;
        }
    }
    
public:
    void Pause() {
        should_pause = true;
        status = DownloadStatus::PAUSED;
        if (on_paused) on_paused(*this);
    }
    
    void Resume() {
        should_pause = false;
        status = DownloadStatus::DOWNLOADING;
        if (on_resumed) on_resumed(*this);
    }
    
    void Cancel() {
        should_cancel = true;
        status = DownloadStatus::CANCELLED;
    }
    
    bool IsCompleted() const {
        return status == DownloadStatus::COMPLETED;
    }
    
    bool IsFailed() const {
        return status == DownloadStatus::FAILED;
    }
    
    bool IsDownloading() const {
        return status == DownloadStatus::DOWNLOADING;
    }
    
    bool IsPaused() const {
        return status == DownloadStatus::PAUSED;
    }
    
    std::string GetFormattedSize() const {
        return FormatFileSize(total_size);
    }
    
    std::string GetFormattedSpeed() const {
        return FormatSpeed(speed);
    }
    
    std::string GetFormattedProgress() const {
        char buffer[32];
        sprintf(buffer, "%.1f%%", progress);
        return std::string(buffer);
    }
    
    std::string GetETA() const {
        if (speed > 0 && total_size > downloaded_size) {
            size_t remaining = total_size - downloaded_size;
            double eta_seconds = remaining / speed;
            
            if (eta_seconds < 60) {
                return std::to_string((int)eta_seconds) + "s";
            } else if (eta_seconds < 3600) {
                return std::to_string((int)(eta_seconds / 60)) + "m";
            } else {
                return std::to_string((int)(eta_seconds / 3600)) + "h";
            }
        }
        return "N/A";
    }
    
private:
    std::string FormatFileSize(size_t size) const {
        const char* units[] = {"B", "KB", "MB", "GB"};
        int unit = 0;
        double size_d = static_cast<double>(size);
        
        while (size_d >= 1024.0 && unit < 3) {
            size_d /= 1024.0;
            unit++;
        }
        
        char buffer[32];
        sprintf(buffer, "%.1f %s", size_d, units[unit]);
        return std::string(buffer);
    }
    
    std::string FormatSpeed(double bytes_per_sec) const {
        return FormatFileSize(static_cast<size_t>(bytes_per_sec)) + "/s";
    }
};

// Download manager class
class DownloadManager {
private:
    std::map<std::string, std::shared_ptr<DownloadItem>> downloads;
    std::queue<std::shared_ptr<DownloadItem>> download_queue;
    std::vector<std::thread> worker_threads;
    std::mutex downloads_mutex;
    std::mutex queue_mutex;
    std::condition_variable queue_condition;
    std::atomic<bool> is_running{true};
    
    // Configuration
    int max_concurrent_downloads = 3;
    int max_download_speed = 0; // 0 = unlimited
    std::string default_download_path;
    bool verify_downloads = true;
    bool scan_downloads = false;
    
    // Statistics
    struct Statistics {
        std::atomic<size_t> total_downloads{0};
        std::atomic<size_t> completed_downloads{0};
        std::atomic<size_t> failed_downloads{0};
        std::atomic<size_t> total_bytes_downloaded{0};
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        
        void Reset() {
            total_downloads = 0;
            completed_downloads = 0;
            failed_downloads = 0;
            total_bytes_downloaded = 0;
            start_time = std::chrono::high_resolution_clock::now();
        }
        
        void UpdateDownloadStats(size_t bytes) {
            total_bytes_downloaded += bytes;
        }
        
        void IncrementCompleted() {
            completed_downloads++;
        }
        
        void IncrementFailed() {
            failed_downloads++;
        }
    } statistics;
    
public:
    DownloadManager();
    ~DownloadManager();
    
    // Initialization
    bool Initialize(const std::string& download_path = "");
    void Shutdown();
    
    // Download management
    std::string AddDownload(const std::string& url, const std::string& filename = "", 
                          DownloadPriority priority = DownloadPriority::NORMAL);
    bool RemoveDownload(const std::string& download_id);
    bool PauseDownload(const std::string& download_id);
    bool ResumeDownload(const std::string& download_id);
    bool CancelDownload(const std::string& download_id);
    bool RetryDownload(const std::string& download_id);
    
    // Download information
    std::shared_ptr<DownloadItem> GetDownload(const std::string& download_id);
    std::vector<std::shared_ptr<DownloadItem>> GetAllDownloads();
    std::vector<std::shared_ptr<DownloadItem>> GetActiveDownloads();
    std::vector<std::shared_ptr<DownloadItem>> GetCompletedDownloads();
    
    // Configuration
    void SetMaxConcurrentDownloads(int max_downloads);
    void SetMaxDownloadSpeed(int speed_bytes_per_sec);
    void SetDefaultDownloadPath(const std::string& path);
    void SetVerifyDownloads(bool verify);
    void SetScanDownloads(bool scan);
    
    // Statistics
    Statistics GetStatistics() const { return statistics; }
    void ResetStatistics();
    
    // Utility methods
    std::string GetDefaultDownloadPath() const;
    bool IsValidURL(const std::string& url);
    std::string GenerateDownloadID() const;
    
    // Callbacks
    std::function<void(const DownloadItem&)> on_download_added;
    std::function<void(const DownloadItem&)> on_download_completed;
    std::function<void(const DownloadItem&, const std::string&)> on_download_failed;
    std::function<void(const DownloadItem&)> on_download_progress;
    
private:
    // Worker thread management
    void StartWorkerThreads();
    void StopWorkerThreads();
    void WorkerThread(int thread_id);
    
    // Download processing
    void ProcessDownload(std::shared_ptr<DownloadItem> download);
    bool DownloadFile(std::shared_ptr<DownloadItem> download);
    bool VerifyFileIntegrity(std::shared_ptr<DownloadItem> download);
    bool ScanForMalware(std::shared_ptr<DownloadItem> download);
    void MoveTempToFinal(std::shared_ptr<DownloadItem> download);
    
    // Platform-specific implementations
#ifdef _WIN32
    bool DownloadFileWinHTTP(std::shared_ptr<DownloadItem> download);
    bool DownloadFileURLMon(std::shared_ptr<DownloadItem> download);
#else
    bool DownloadFileCURL(std::shared_ptr<DownloadItem> download);
#endif
    
    // Utility methods
    std::string GetContentDisposition(const std::string& url, const std::string& headers);
    std::string CalculateFileHash(const std::string& filename);
    bool CreateDirectory(const std::string& path);
    bool FileExists(const std::string& filename);
    size_t GetFileSize(const std::string& filename);
    
    // Queue management
    void EnqueueDownload(std::shared_ptr<DownloadItem> download);
    std::shared_ptr<DownloadItem> DequeueDownload();
    bool IsQueueEmpty();
};

// Download page class
class DownloadPage {
private:
    std::shared_ptr<DownloadManager> download_manager;
    std::string page_url;
    std::shared_ptr<DOMDocument> document;
    
public:
    DownloadPage(std::shared_ptr<DownloadManager> manager);
    ~DownloadPage();
    
    // Page management
    bool LoadPage(const std::string& url);
    void ShowDownloads();
    void AddDownloadFromURL(const std::string& url);
    void ShowDownloadProgress();
    void ShowDownloadHistory();
    
    // UI methods
    void RenderDownloadList();
    void RenderDownloadItem(const DownloadItem& download);
    void RenderDownloadStats();
    
private:
    // Page parsing
    void ParseDownloadLinks();
    std::vector<std::string> ExtractDownloadLinks();
    
    // Event handlers
    void OnDownloadProgress(const DownloadItem& download);
    void OnDownloadCompleted(const DownloadItem& download);
    void OnDownloadFailed(const DownloadItem& download, const std::string& error);
    
    // UI helpers
    void ClearScreen();
    void PrintHeader();
    void PrintFooter();
    std::string FormatDownloadStatus(DownloadStatus status);
    std::string GetProgressBar(double progress, int width = 30);
};

// Factory functions
namespace DownloadFactory {
    std::shared_ptr<DownloadManager> CreateDownloadManager();
    std::shared_ptr<DownloadPage> CreateDownloadPage(std::shared_ptr<DownloadManager> manager);
}

// Utility functions
namespace DownloadUtils {
    std::string GetFileSizeString(size_t size);
    std::string GetSpeedString(double bytes_per_sec);
    std::string GetTimeString(double seconds);
    bool IsValidURL(const std::string& url);
    std::string SanitizeFilename(const std::string& filename);
    std::string GetFileExtension(const std::string& url);
    std::string GetMimeType(const std::string& filename);
}

} // namespace ComperCore

#endif // COMPER_DOWNLOAD_H
