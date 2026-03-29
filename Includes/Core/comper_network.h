#ifndef COMPER_NETWORK_H
#define COMPER_NETWORK_H

/*
 * Comper Engine Core - Network Module
 * 
 * Ağ istekleri ve HTTP/HTTPS işlemleri için temel modül
 * High-performance, thread-safe, cache destekli
 * 
 * Features:
 * - HTTP/HTTPS desteği
 * - Connection pooling
 * - Request caching
 * - Cookie management
 * - Proxy support
 * - SSL/TLS security
 * - Async operations
 * - Rate limiting
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>

#ifdef _WIN32
#include <winhttp.h>
#include <wininet.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#include <pthread.h>
#endif

namespace ComperCore {

// Network request structure
struct NetworkRequest {
    std::string url;
    std::string method;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> cookies;
    int timeout = 30;
    bool follow_redirects = true;
    bool verify_ssl = true;
    std::string proxy;
    std::string user_agent;
    
    NetworkRequest(const std::string& request_url) 
        : url(request_url), method("GET") {}
};

// Network response structure
struct NetworkResponse {
    int status_code = 0;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> cookies;
    std::string error_message;
    double response_time = 0.0;
    size_t content_length = 0;
    std::string content_type;
    std::string charset;
    bool is_cached = false;
    std::chrono::time_point<std::chrono::high_resolution_clock> cache_time;
    
    bool IsSuccess() const {
        return status_code >= 200 && status_code < 300;
    }
    
    bool IsRedirect() const {
        return status_code >= 300 && status_code < 400;
    }
    
    bool IsClientError() const {
        return status_code >= 400 && status_code < 500;
    }
    
    bool IsServerError() const {
        return status_code >= 500 && status_code < 600;
    }
};

// Cookie structure
struct Cookie {
    std::string name;
    std::string value;
    std::string domain;
    std::string path;
    std::chrono::time_point<std::chrono::high_resolution_clock> expires;
    bool secure = false;
    bool http_only = false;
    bool session_only = false;
    
    bool IsExpired() const {
        if (session_only) return false;
        return std::chrono::high_resolution_clock::now() > expires;
    }
    
    std::string ToString() const {
        std::string result = name + "=" + value;
        if (!domain.empty()) result += "; Domain=" + domain;
        if (!path.empty()) result += "; Path=" + path;
        if (secure) result += "; Secure";
        if (http_only) result += "; HttpOnly";
        return result;
    }
};

// Connection pool entry
struct Connection {
    std::string host;
    int port;
    bool is_ssl;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
    int usage_count = 0;
    bool is_active = true;
    
#ifdef _WIN32
    HINTERNET handle = nullptr;
#else
    void* handle = nullptr;
#endif
};

// Cache entry
struct CacheEntry {
    std::string url;
    NetworkResponse response;
    std::chrono::time_point<std::chrono::high_resolution_clock> cached_at;
    std::chrono::seconds max_age{300}; // 5 minutes default
    
    bool IsExpired() const {
        auto now = std::chrono::high_resolution_clock::now();
        return (now - cached_at) > max_age;
    }
};

// Network manager class
class NetworkManager {
private:
    std::unique_ptr<std::thread> worker_thread;
    std::vector<std::thread> pool_threads;
    
    // Connection pooling
    std::map<std::string, std::vector<std::shared_ptr<Connection>>> connection_pool;
    std::mutex pool_mutex;
    size_t max_connections_per_host = 10;
    size_t max_total_connections = 100;
    
    // Cookie management
    std::map<std::string, std::vector<Cookie>> cookie_jar;
    std::mutex cookie_mutex;
    
    // Cache management
    std::map<std::string, CacheEntry> response_cache;
    std::mutex cache_mutex;
    size_t max_cache_size = 1000;
    std::chrono::seconds default_cache_ttl{300};
    
    // Rate limiting
    std::map<std::string, std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>> request_times;
    std::mutex rate_limit_mutex;
    size_t max_requests_per_second = 10;
    
    // Statistics
    struct Stats {
        size_t total_requests = 0;
        size_t successful_requests = 0;
        size_t failed_requests = 0;
        size_t cached_requests = 0;
        double total_response_time = 0.0;
        size_t bytes_transferred = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    } stats;
    
    bool is_running = false;
    std::mutex stats_mutex;
    
public:
    NetworkManager();
    ~NetworkManager();
    
    // Core functionality
    bool Initialize();
    void Shutdown();
    
    // Request methods
    NetworkResponse Get(const std::string& url);
    NetworkResponse Post(const std::string& url, const std::string& body);
    NetworkResponse Put(const std::string& url, const std::string& body);
    NetworkResponse Delete(const std::string& url);
    NetworkResponse Head(const std::string& url);
    NetworkResponse Request(const NetworkRequest& request);
    
    // Async operations
    void AsyncRequest(const NetworkRequest& request, 
                    std::function<void(const NetworkResponse&)> callback);
    void AsyncGet(const std::string& url, 
                  std::function<void(const NetworkResponse&)> callback);
    
    // Connection pooling
    std::shared_ptr<Connection> GetConnection(const std::string& host, int port, bool ssl);
    void ReturnConnection(std::shared_ptr<Connection> connection);
    void CleanupIdleConnections();
    
    // Cookie management
    void SetCookie(const Cookie& cookie);
    void SetCookie(const std::string& name, const std::string& value, 
                   const std::string& domain = "", const std::string& path = "");
    std::vector<Cookie> GetCookies(const std::string& url);
    void ClearCookies();
    void ClearExpiredCookies();
    
    // Cache management
    void CacheResponse(const std::string& url, const NetworkResponse& response);
    NetworkResponse GetCachedResponse(const std::string& url);
    void ClearCache();
    void SetCacheTTL(std::chrono::seconds ttl);
    void SetMaxCacheSize(size_t size);
    
    // Rate limiting
    bool IsRateLimited(const std::string& host);
    void SetRateLimit(size_t requests_per_second);
    
    // Configuration
    void SetUserAgent(const std::string& user_agent);
    void SetProxy(const std::string& proxy);
    void SetTimeout(int seconds);
    void SetMaxConnections(size_t max_connections);
    void SetSSLVerification(bool verify);
    
    // Statistics
    Stats GetStats() const;
    void ResetStats();
    
    // Utility methods
    std::string URLEncode(const std::string& str);
    std::string URLDecode(const std::string& str);
    std::string GetDomainFromURL(const std::string& url);
    std::string GetPathFromURL(const std::string& url);
    
private:
    // Internal request processing
    NetworkResponse ProcessRequest(const NetworkRequest& request);
    NetworkResponse ProcessRequestSync(const NetworkRequest& request);
    
    // Connection management
    std::shared_ptr<Connection> CreateConnection(const std::string& host, int port, bool ssl);
    void CloseConnection(std::shared_ptr<Connection> connection);
    
    // Cookie parsing
    std::vector<Cookie> ParseCookies(const std::string& cookie_header);
    std::string BuildCookieHeader(const std::vector<Cookie>& cookies);
    
    // Cache management
    void CleanupCache();
    std::string GenerateCacheKey(const NetworkRequest& request);
    
    // Rate limiting
    void RecordRequest(const std::string& host);
    
    // Platform-specific implementations
#ifdef _WIN32
    NetworkResponse ProcessWinHTTP(const NetworkRequest& request);
    NetworkResponse ProcessWinINet(const NetworkRequest& request);
#else
    NetworkResponse ProcessCURL(const NetworkRequest& request);
#endif
    
    // Worker thread
    void WorkerThread();
};

// Utility functions
namespace NetworkUtils {
    std::string GetMimeType(const std::string& file_extension);
    std::string GetFileExtension(const std::string& url);
    bool IsURL(const std::string& str);
    std::string ResolveRelativeURL(const std::string& base_url, const std::string& relative_url);
    std::map<std::string, std::string> ParseQueryString(const std::string& query);
    std::string BuildQueryString(const std::map<std::string, std::string>& params);
}

} // namespace ComperCore

#endif // COMPER_NETWORK_H
