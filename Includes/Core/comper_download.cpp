/*
 * Comper Engine Core - Download Module Implementation
 * 
 * İndirme yönetimi, dosya indirme, progress tracking, resume desteği
 * Multi-threaded indirme, pause/resume, güvenlik kontrolleri
 */

#include "comper_download.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <shlobj.h>
#include <winerror.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace ComperCore {

// DownloadManager Implementation
DownloadManager::DownloadManager() {
    // Get default download path
    default_download_path = GetDefaultDownloadPath();
    
    // Initialize statistics
    statistics.Reset();
}

DownloadManager::~DownloadManager() {
    Shutdown();
}

bool DownloadManager::Initialize(const std::string& download_path) {
    if (!download_path.empty()) {
        default_download_path = download_path;
    }
    
    // Create download directory if it doesn't exist
    if (!CreateDirectory(default_download_path)) {
        std::cerr << "[DownloadManager] Failed to create download directory: " 
                  << default_download_path << std::endl;
        return false;
    }
    
    // Start worker threads
    StartWorkerThreads();
    
    std::cout << "[DownloadManager] Initialized with download path: " 
              << default_download_path << std::endl;
    return true;
}

void DownloadManager::Shutdown() {
    is_running = false;
    queue_condition.notify_all();
    
    // Wait for worker threads to finish
    for (auto& thread : worker_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    worker_threads.clear();
    
    std::cout << "[DownloadManager] Shutdown completed" << std::endl;
}

std::string DownloadManager::AddDownload(const std::string& url, const std::string& filename, 
                                       DownloadPriority priority) {
    if (!IsValidURL(url)) {
        std::cerr << "[DownloadManager] Invalid URL: " << url << std::endl;
        return "";
    }
    
    std::string download_id = GenerateDownloadID();
    auto download = std::make_shared<DownloadItem>(download_id, url, filename);
    download->priority = priority;
    
    {
        std::lock_guard<std::mutex> lock(downloads_mutex);
        downloads[download_id] = download;
        statistics.total_downloads++;
    }
    
    EnqueueDownload(download);
    
    if (on_download_added) {
        on_download_added(*download);
    }
    
    std::cout << "[DownloadManager] Download added: " << download_id << " (" << url << ")" << std::endl;
    return download_id;
}

bool DownloadManager::RemoveDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    if (it != downloads.end()) {
        auto download = it->second;
        
        // Cancel download if it's active
        if (download->IsDownloading() || download->IsPaused()) {
            download->Cancel();
        }
        
        // Remove files
        if (FileExists(download->temp_filename)) {
            std::remove(download->temp_filename.c_str());
        }
        if (FileExists(download->filename)) {
            std::remove(download->filename.c_str());
        }
        
        downloads.erase(it);
        std::cout << "[DownloadManager] Download removed: " << download_id << std::endl;
        return true;
    }
    
    return false;
}

bool DownloadManager::PauseDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    if (it != downloads.end()) {
        it->second->Pause();
        return true;
    }
    
    return false;
}

bool DownloadManager::ResumeDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    if (it != downloads.end()) {
        it->second->Resume();
        return true;
    }
    
    return false;
}

bool DownloadManager::CancelDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    if (it != downloads.end()) {
        it->second->Cancel();
        return true;
    }
    
    return false;
}

bool DownloadManager::RetryDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    if (it != downloads.end()) {
        auto download = it->second;
        
        // Reset download state
        download->downloaded_size = 0;
        download->progress = 0.0;
        download->speed = 0.0;
        download->status = DownloadStatus::PENDING;
        download->should_cancel = false;
        download->should_pause = false;
        
        // Re-queue download
        EnqueueDownload(download);
        
        std::cout << "[DownloadManager] Download retry queued: " << download_id << std::endl;
        return true;
    }
    
    return false;
}

std::shared_ptr<DownloadItem> DownloadManager::GetDownload(const std::string& download_id) {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    auto it = downloads.find(download_id);
    return (it != downloads.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<DownloadItem>> DownloadManager::GetAllDownloads() {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    std::vector<std::shared_ptr<DownloadItem>> result;
    for (const auto& pair : downloads) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<std::shared_ptr<DownloadItem>> DownloadManager::GetActiveDownloads() {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    std::vector<std::shared_ptr<DownloadItem>> result;
    for (const auto& pair : downloads) {
        const auto& download = pair.second;
        if (download->IsDownloading() || download->IsPaused()) {
            result.push_back(download);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<DownloadItem>> DownloadManager::GetCompletedDownloads() {
    std::lock_guard<std::mutex> lock(downloads_mutex);
    
    std::vector<std::shared_ptr<DownloadItem>> result;
    for (const auto& pair : downloads) {
        const auto& download = pair.second;
        if (download->IsCompleted()) {
            result.push_back(download);
        }
    }
    
    return result;
}

void DownloadManager::SetMaxConcurrentDownloads(int max_downloads) {
    max_concurrent_downloads = std::max(1, max_downloads);
    std::cout << "[DownloadManager] Max concurrent downloads set to: " 
              << max_concurrent_downloads << std::endl;
}

void DownloadManager::SetMaxDownloadSpeed(int speed_bytes_per_sec) {
    max_download_speed = std::max(0, speed_bytes_per_sec);
    std::cout << "[DownloadManager] Max download speed set to: " 
              << FormatFileSize(max_download_speed) << "/s" << std::endl;
}

void DownloadManager::SetDefaultDownloadPath(const std::string& path) {
    default_download_path = path;
    CreateDirectory(path);
    std::cout << "[DownloadManager] Default download path set to: " << path << std::endl;
}

void DownloadManager::SetVerifyDownloads(bool verify) {
    verify_downloads = verify;
    std::cout << "[DownloadManager] Download verification " 
              << (verify ? "enabled" : "disabled") << std::endl;
}

void DownloadManager::SetScanDownloads(bool scan) {
    scan_downloads = scan;
    std::cout << "[DownloadManager] Malware scanning " 
              << (scan ? "enabled" : "disabled") << std::endl;
}

void DownloadManager::ResetStatistics() {
    statistics.Reset();
    std::cout << "[DownloadManager] Statistics reset" << std::endl;
}

std::string DownloadManager::GetDefaultDownloadPath() const {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path) == S_OK) {
        return std::string(path) + "\\Downloads";
    }
    return "./Downloads";
#else
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Downloads";
    }
    return "./Downloads";
#endif
}

bool DownloadManager::IsValidURL(const std::string& url) {
    // Basic URL validation
    if (url.empty()) return false;
    
    // Check for protocol
    if (url.substr(0, 7) != "http://" && url.substr(0, 8) != "https://") {
        return false;
    }
    
    // Check for valid characters
    for (char c : url) {
        if (!isalnum(c) && c != ':' && c != '/' && c != '.' && c != '-' && 
            c != '_' && c != '~' && c != '%' && c != '&' && c != '?' && 
            c != '=' && c != '#' && c != '+' && c != ' ') {
            return false;
        }
    }
    
    return true;
}

std::string DownloadManager::GenerateDownloadID() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    
    return "dl_" + std::to_string(dis(gen));
}

void DownloadManager::StartWorkerThreads() {
    for (int i = 0; i < max_concurrent_downloads; ++i) {
        worker_threads.emplace_back([this, i]() {
            WorkerThread(i);
        });
    }
    
    std::cout << "[DownloadManager] Started " << worker_threads.size() 
              << " worker threads" << std::endl;
}

void DownloadManager::StopWorkerThreads() {
    is_running = false;
    queue_condition.notify_all();
}

void DownloadManager::WorkerThread(int thread_id) {
    std::cout << "[DownloadManager] Worker thread " << thread_id << " started" << std::endl;
    
    while (is_running) {
        std::shared_ptr<DownloadItem> download = DequeueDownload();
        
        if (!download || !is_running) {
            break;
        }
        
        ProcessDownload(download);
    }
    
    std::cout << "[DownloadManager] Worker thread " << thread_id << " stopped" << std::endl;
}

void DownloadManager::ProcessDownload(std::shared_ptr<DownloadItem> download) {
    std::cout << "[DownloadManager] Processing download: " << download->id << std::endl;
    
    download->status = DownloadStatus::DOWNLOADING;
    download->start_time = std::chrono::high_resolution_clock::now();
    
    try {
        if (DownloadFile(download)) {
            // Verify file integrity if enabled
            if (verify_downloads && !VerifyFileIntegrity(download)) {
                download->status = DownloadStatus::FAILED;
                if (on_download_failed) {
                    on_download_failed(*download, "File integrity verification failed");
                }
                statistics.IncrementFailed();
                return;
            }
            
            // Scan for malware if enabled
            if (scan_downloads && !ScanForMalware(download)) {
                download->status = DownloadStatus::FAILED;
                if (on_download_failed) {
                    on_download_failed(*download, "Malware detected");
                }
                statistics.IncrementFailed();
                return;
            }
            
            // Move temp file to final location
            MoveTempToFinal(download);
            
            download->status = DownloadStatus::COMPLETED;
            if (on_download_completed) {
                on_download_completed(*download);
            }
            
            statistics.IncrementCompleted();
            statistics.UpdateDownloadStats(download->downloaded_size);
            
            std::cout << "[DownloadManager] Download completed: " << download->id << std::endl;
        } else {
            download->status = DownloadStatus::FAILED;
            if (on_download_failed) {
                on_download_failed(*download, "Download failed");
            }
            statistics.IncrementFailed();
        }
    } catch (const std::exception& e) {
        download->status = DownloadStatus::FAILED;
        if (on_download_failed) {
            on_download_failed(*download, e.what());
        }
        statistics.IncrementFailed();
        std::cerr << "[DownloadManager] Download error: " << e.what() << std::endl;
    }
}

bool DownloadManager::DownloadFile(std::shared_ptr<DownloadItem> download) {
#ifdef _WIN32
    return DownloadFileWinHTTP(download);
#else
    return DownloadFileCURL(download);
#endif
}

#ifdef _WIN32
bool DownloadManager::DownloadFileWinHTTP(std::shared_ptr<DownloadItem> download) {
    std::wstring wide_url(download->url.begin(), download->url.end());
    std::wstring wide_agent(download->user_agent.begin(), download->user_agent.end());
    
    HINTERNET hSession = WinHttpOpen(wide_agent.c_str(), 
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME, 
                                     WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cerr << "[DownloadManager] Failed to open WinHTTP session" << std::endl;
        return false;
    }
    
    HINTERNET hConnect = WinHttpConnect(hSession, 
                                      L"www.example.com", 
                                      INTERNET_DEFAULT_HTTPS_PORT,
                                      NULL, NULL, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        std::cerr << "[DownloadManager] Failed to connect" << std::endl;
        return false;
    }
    
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, 
                                           L"GET", 
                                           wide_url.c_str(),
                                           NULL, WINHTTP_NO_REFERER,
                                           WINHTTP_DEFAULT_ACCEPT_TYPES,
                                           0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[DownloadManager] Failed to open request" << std::endl;
        return false;
    }
    
    // Send request
    if (!WinHttpSendRequest(hRequest, 
                            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[DownloadManager] Failed to send request" << std::endl;
        return false;
    }
    
    // Get response status
    DWORD status_code = 0;
    DWORD status_size = sizeof(status_code);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                       WINHTTP_HEADER_NAME_BY_INDEX, NULL, &status_code, &status_size,
                       WINHTTP_NO_HEADER_INDEX);
    
    if (status_code != HTTP_STATUS_OK) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[DownloadManager] HTTP error: " << status_code << std::endl;
        return false;
    }
    
    // Get content length
    DWORD content_length = 0;
    DWORD size = sizeof(content_length);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                       WINHTTP_HEADER_NAME_BY_INDEX, NULL, &content_length, &size,
                       WINHTTP_NO_HEADER_INDEX);
    
    download->total_size = content_length;
    
    // Open file for writing
    std::ofstream file(download->temp_filename, std::ios::binary);
    if (!file.is_open()) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        std::cerr << "[DownloadManager] Failed to create file: " << download->temp_filename << std::endl;
        return false;
    }
    
    // Download data
    DWORD bytes_read = 0;
    DWORD total_bytes_read = 0;
    const DWORD buffer_size = 8192;
    std::vector<char> buffer(buffer_size);
    
    auto last_progress_update = std::chrono::high_resolution_clock::now();
    
    while (WinHttpReadData(hRequest, buffer.data(), buffer_size, &bytes_read, 
                           WINHTTP_NO_CALLBACK) && bytes_read > 0) {
        
        // Check for pause/cancel
        if (download->should_cancel) {
            file.close();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            std::cout << "[DownloadManager] Download cancelled: " << download->id << std::endl;
            return false;
        }
        
        while (download->should_pause) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (download->should_cancel) {
                file.close();
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return false;
            }
        }
        
        // Write to file
        file.write(buffer.data(), bytes_read);
        total_bytes_read += bytes_read;
        download->downloaded_size = total_bytes_read;
        download->UpdateProgress();
        
        // Update progress periodically
        auto now = std::chrono::high_resolution_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_progress_update);
        
        if (time_diff.count() > 500) { // Update every 500ms
            if (on_download_progress) {
                on_download_progress(*download);
            }
            last_progress_update = now;
        }
    }
    
    file.close();
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    
    return total_bytes_read > 0;
}
#else
bool DownloadManager::DownloadFileCURL(std::shared_ptr<DownloadItem> download) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[DownloadManager] Failed to initialize CURL" << std::endl;
        return false;
    }
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, download->url.c_str());
    
    // Set user agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, download->user_agent.c_str());
    
    // Set follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // Set write callback
    std::ofstream file(download->temp_filename, std::ios::binary);
    if (!file.is_open()) {
        curl_easy_cleanup(curl);
        std::cerr << "[DownloadManager] Failed to create file: " << download->temp_filename << std::endl;
        return false;
    }
    
    auto write_callback = [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        size_t total_size = size * nmemb;
        std::ofstream* file = static_cast<std::ofstream*>(userp);
        file->write(static_cast<char*>(contents), total_size);
        return total_size;
    };
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    
    // Set progress callback
    auto progress_callback = [](void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                                curl_off_t ultotal, curl_off_t ulnow) -> int {
        DownloadItem* download = static_cast<DownloadItem*>(clientp);
        
        if (dltotal > 0) {
            download->total_size = dltotal;
        }
        download->downloaded_size = dlnow;
        download->UpdateProgress();
        
        // Check for pause/cancel
        if (download->should_cancel) {
            return 1; // Abort download
        }
        
        while (download->should_pause) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (download->should_cancel) {
                return 1;
            }
        }
        
        return 0;
    };
    
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, download.get());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    
    // Perform download
    CURLcode res = curl_easy_perform(curl);
    
    file.close();
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "[DownloadManager] CURL error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
    
    return true;
}
#endif

bool DownloadManager::VerifyFileIntegrity(std::shared_ptr<DownloadItem> download) {
    if (download->expected_hash.empty()) {
        return true; // No hash to verify against
    }
    
    std::string actual_hash = CalculateFileHash(download->temp_filename);
    download->actual_hash = actual_hash;
    
    bool verified = (actual_hash == download->expected_hash);
    
    std::cout << "[DownloadManager] File integrity check: " 
              << (verified ? "PASSED" : "FAILED") << std::endl;
    std::cout << "  Expected: " << download->expected_hash << std::endl;
    std::cout << "  Actual: " << actual_hash << std::endl;
    
    return verified;
}

bool DownloadManager::ScanForMalware(std::shared_ptr<DownloadItem> download) {
    // In a real implementation, this would integrate with antivirus software
    // For now, we'll do a basic heuristic check
    
    std::cout << "[DownloadManager] Malware scanning: " << download->filename << std::endl;
    
    // Basic heuristic: check file extension
    std::string ext = GetFileExtension(download->filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::vector<std::string> suspicious_exts = {
        ".exe", ".scr", ".bat", ".cmd", ".com", ".pif", ".vbs", ".js", ".jar"
    };
    
    bool suspicious = std::find(suspicious_exts.begin(), suspicious_exts.end()) != suspicious_exts.end();
    
    if (suspicious) {
        std::cout << "[DownloadManager] Suspicious file extension detected: " << ext << std::endl;
        // In a real implementation, this would trigger actual malware scanning
    }
    
    return !suspicious; // Return true if safe
}

void DownloadManager::MoveTempToFinal(std::shared_ptr<DownloadItem> download) {
    std::string final_path = default_download_path + "/" + download->filename;
    
    if (std::rename(download->temp_filename.c_str(), final_path.c_str()) == 0) {
        std::cout << "[DownloadManager] File moved to: " << final_path << std::endl;
    } else {
        std::cerr << "[DownloadManager] Failed to move file to: " << final_path << std::endl;
    }
}

std::string DownloadManager::CalculateFileHash(const std::string& filename) {
    // Simple hash calculation (in real implementation, use SHA-256)
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::hash<std::string> hasher;
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    return std::to_string(hasher(content));
}

bool DownloadManager::CreateDirectory(const std::string& path) {
#ifdef _WIN32
    return CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    struct stat st = {0};
    if (stat(path.c_str(), &st) == -1) {
        return mkdir(path.c_str(), 0755) == 0;
    }
    return true;
#endif
}

bool DownloadManager::FileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

size_t DownloadManager::GetFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        return file.tellg();
    }
    return 0;
}

std::string DownloadManager::GetFileExtension(const std::string& filename) {
    size_t last_dot = filename.find_last_of('.');
    if (last_dot != std::string::npos) {
        return filename.substr(last_dot);
    }
    return "";
}

void DownloadManager::EnqueueDownload(std::shared_ptr<DownloadItem> download) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    download_queue.push(download);
    queue_condition.notify_one();
}

std::shared_ptr<DownloadItem> DownloadManager::DequeueDownload() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_condition.wait(lock, [this] { return !download_queue.empty() || !is_running; });
    
    if (!is_running) {
        return nullptr;
    }
    
    auto download = download_queue.front();
    download_queue.pop();
    return download;
}

bool DownloadManager::IsQueueEmpty() {
    std::lock_guard<std::mutex> lock(queue_mutex);
    return download_queue.empty();
}

// DownloadPage Implementation
DownloadPage::DownloadPage(std::shared_ptr<DownloadManager> manager) 
    : download_manager(manager) {
}

DownloadPage::~DownloadPage() {
}

bool DownloadPage::LoadPage(const std::string& url) {
    page_url = url;
    
    // Create a simple browser instance to load the page
    ComperEngine browser;
    NetworkResponse response = browser.LoadPage(url);
    
    if (response.IsSuccess()) {
        document = browser.ParseHTML(response.body);
        if (document) {
            ParseDownloadLinks();
            ShowDownloads();
            return true;
        }
    }
    
    std::cerr << "[DownloadPage] Failed to load page: " << url << std::endl;
    return false;
}

void DownloadPage::ShowDownloads() {
    ClearScreen();
    PrintHeader();
    
    auto downloads = download_manager->GetAllDownloads();
    
    if (downloads.empty()) {
        std::cout << "No downloads found." << std::endl;
    } else {
        RenderDownloadList();
    }
    
    RenderDownloadStats();
    PrintFooter();
}

void DownloadPage::AddDownloadFromURL(const std::string& url) {
    std::string download_id = download_manager->AddDownload(url);
    if (!download_id.empty()) {
        std::cout << "Download added: " << download_id << std::endl;
    } else {
        std::cout << "Failed to add download" << std::endl;
    }
}

void DownloadPage::ShowDownloadProgress() {
    auto active_downloads = download_manager->GetActiveDownloads();
    
    ClearScreen();
    PrintHeader();
    
    std::cout << "Active Downloads:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& download : active_downloads) {
        RenderDownloadItem(*download);
    }
    
    PrintFooter();
}

void DownloadPage::ShowDownloadHistory() {
    auto completed_downloads = download_manager->GetCompletedDownloads();
    
    ClearScreen();
    PrintHeader();
    
    std::cout << "Download History:" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& download : completed_downloads) {
        RenderDownloadItem(*download);
    }
    
    PrintFooter();
}

void DownloadPage::ParseDownloadLinks() {
    if (!document) {
        return;
    }
    
    // Find all download links
    auto links = document->QuerySelectorAll("a[href]");
    
    std::cout << "[DownloadPage] Found " << links.size() << " links" << std::endl;
    
    // Filter for download links (heuristic)
    for (const auto& link : links) {
        std::string href = link->GetAttribute("href");
        std::string text = link->GetTextContent();
        
        // Check if this looks like a download link
        if (IsDownloadLink(href, text)) {
            std::cout << "[DownloadPage] Found download link: " << href << std::endl;
            // Could add to download queue automatically
        }
    }
}

std::vector<std::string> DownloadPage::ExtractDownloadLinks() {
    std::vector<std::string> download_links;
    
    if (!document) {
        return download_links;
    }
    
    auto links = document->QuerySelectorAll("a[href]");
    for (const auto& link : links) {
        std::string href = link->GetAttribute("href");
        std::string text = link->GetTextContent();
        
        if (IsDownloadLink(href, text)) {
            download_links.push_back(href);
        }
    }
    
    return download_links;
}

bool DownloadPage::IsDownloadLink(const std::string& href, const std::string& text) {
    // Heuristic to identify download links
    std::string lower_href = href;
    std::string lower_text = text;
    std::transform(lower_href.begin(), lower_href.end(), lower_href.begin(), ::tolower);
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
    
    // Check for download indicators
    std::vector<std::string> download_indicators = {
        "download", "install", "setup", ".exe", ".zip", ".rar", ".tar", ".gz",
        "get", "fetch", "obtain", "acquire"
    };
    
    for (const auto& indicator : download_indicators) {
        if (lower_href.find(indicator) != std::string::npos || 
            lower_text.find(indicator) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

void DownloadPage::OnDownloadProgress(const DownloadItem& download) {
    // Update progress display if showing active downloads
    // This would be called asynchronously
}

void DownloadPage::OnDownloadCompleted(const DownloadItem& download) {
    std::cout << "[DownloadPage] Download completed: " << download.filename 
              << " (" << download.GetFormattedSize() << ")" << std::endl;
}

void DownloadPage::OnDownloadFailed(const DownloadItem& download, const std::string& error) {
    std::cout << "[DownloadPage] Download failed: " << download.filename 
              << " - " << error << std::endl;
}

void DownloadPage::RenderDownloadList() {
    auto downloads = download_manager->GetAllDownloads();
    
    for (const auto& download : downloads) {
        RenderDownloadItem(*download);
    }
}

void DownloadPage::RenderDownloadItem(const DownloadItem& download) {
    std::cout << "ID: " << download.id << std::endl;
    std::cout << "URL: " << download.url << std::endl;
    std::cout << "File: " << download.filename << std::endl;
    std::cout << "Status: " << FormatDownloadStatus(download.status) << std::endl;
    
    if (download.IsDownloading() || download.IsPaused()) {
        std::cout << "Progress: " << GetProgressBar(download.progress) << std::endl;
        std::cout << "Size: " << download.GetFormattedSize() << std::endl;
        std::cout << "Speed: " << download.GetFormattedSpeed() << std::endl;
        std::cout << "ETA: " << download.GetETA() << std::endl;
    }
    
    std::cout << std::string(60, '-') << std::endl;
}

void DownloadPage::RenderDownloadStats() {
    auto stats = download_manager->GetStatistics();
    
    std::cout << "\nDownload Statistics:" << std::endl;
    std::cout << "Total Downloads: " << stats.total_downloads.load() << std::endl;
    std::cout << "Completed: " << stats.completed_downloads.load() << std::endl;
    std::cout << "Failed: " << stats.failed_downloads.load() << std::endl;
    std::cout << "Total Size: " << DownloadUtils::GetFileSizeString(stats.total_bytes_downloaded.load()) << std::endl;
}

void DownloadPage::ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void DownloadPage::PrintHeader() {
    std::cout << "========================================" << std::endl;
    std::cout << "       COMPER DOWNLOAD MANAGER" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

void DownloadPage::PrintFooter() {
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  add <url>     - Add download" << std::endl;
    std::cout << "  pause <id>    - Pause download" << std::endl;
    std::cout << "  resume <id>   - Resume download" << std::endl;
    std::cout << "  cancel <id>   - Cancel download" << std::endl;
    std::cout << "  remove <id>   - Remove download" << std::endl;
    std::cout << "  progress       - Show active downloads" << std::endl;
    std::cout << "  history       - Show download history" << std::endl;
    std::cout << "  stats         - Show statistics" << std::endl;
    std::cout << "  exit          - Exit" << std::endl;
    std::cout << "========================================" << std::endl;
}

std::string DownloadPage::FormatDownloadStatus(DownloadStatus status) {
    switch (status) {
        case DownloadStatus::PENDING: return "Pending";
        case DownloadStatus::DOWNLOADING: return "Downloading";
        case DownloadStatus::PAUSED: return "Paused";
        case DownloadStatus::COMPLETED: return "Completed";
        case DownloadStatus::FAILED: return "Failed";
        case DownloadStatus::CANCELLED: return "Cancelled";
        case DownloadStatus::VERIFYING: return "Verifying";
        default: return "Unknown";
    }
}

std::string DownloadPage::GetProgressBar(double progress, int width) {
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

// Factory functions
namespace DownloadFactory {
    std::shared_ptr<DownloadManager> CreateDownloadManager() {
        return std::make_shared<DownloadManager>();
    }
    
    std::shared_ptr<DownloadPage> CreateDownloadPage(std::shared_ptr<DownloadManager> manager) {
        return std::make_shared<DownloadPage>(manager);
    }
}

// Utility functions
namespace DownloadUtils {
    std::string GetFileSizeString(size_t size) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unit = 0;
        double size_d = static_cast<double>(size);
        
        while (size_d >= 1024.0 && unit < 4) {
            size_d /= 1024.0;
            unit++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size_d << " " << units[unit];
        return oss.str();
    }
    
    std::string GetSpeedString(double bytes_per_sec) {
        return GetFileSizeString(static_cast<size_t>(bytes_per_sec)) + "/s";
    }
    
    std::string GetTimeString(double seconds) {
        if (seconds < 60) {
            return std::to_string(static_cast<int>(seconds)) + "s";
        } else if (seconds < 3600) {
            return std::to_string(static_cast<int>(seconds / 60)) + "m";
        } else {
            return std::to_string(static_cast<int>(seconds / 3600)) + "h";
        }
    }
    
    bool IsValidURL(const std::string& url) {
        // Basic URL validation
        return !url.empty() && 
               (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://");
    }
    
    std::string SanitizeFilename(const std::string& filename) {
        std::string sanitized = filename;
        
        // Remove invalid characters
        const std::string invalid_chars = "<>:\"/\\|?*";
        for (char c : invalid_chars) {
            sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), c), sanitized.end());
        }
        
        return sanitized;
    }
    
    std::string GetFileExtension(const std::string& filename) {
        size_t last_dot = filename.find_last_of('.');
        if (last_dot != std::string::npos) {
            return filename.substr(last_dot);
        }
        return "";
    }
    
    std::string GetMimeType(const std::string& filename) {
        std::string ext = GetFileExtension(filename);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        static std::map<std::string, std::string> mime_types = {
            {".html", "text/html"},
            {".htm", "text/html"},
            {".txt", "text/plain"},
            {".pdf", "application/pdf"},
            {".zip", "application/zip"},
            {".exe", "application/x-msdownload"},
            {".jpg", "image/jpeg"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".mp4", "video/mp4"},
            {".mp3", "audio/mpeg"}
        };
        
        auto it = mime_types.find(ext);
        return (it != mime_types.end()) ? it->second : "application/octet-stream";
    }
}

} // namespace ComperCore
