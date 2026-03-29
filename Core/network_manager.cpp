#include "../Includes/comper_core.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>

namespace Comper {

// CURL callback fonksiyonu
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append((char*)contents, total_size);
    return total_size;
}

// NetworkManager sınıfı implementasyonu
class NetworkManager {
private:
    CURL* curl_handle;
    
public:
    NetworkManager() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_handle = curl_easy_init();
        
        if (!curl_handle) {
            std::cerr << "CURL başlatılamadı!" << std::endl;
        }
    }
    
    ~NetworkManager() {
        if (curl_handle) {
            curl_easy_cleanup(curl_handle);
        }
        curl_global_cleanup();
    }
    
    NetworkResponse MakeRequest(const NetworkRequest& request) {
        NetworkResponse response;
        
        if (!curl_handle) {
            response.status_code = 500;
            response.body = "CURL başlatılamadı";
            return response;
        }
        
        // URL ayarla
        curl_easy_setopt(curl_handle, CURLOPT_URL, request.url.c_str());
        
        // POST verisi varsa ayarla
        if (!request.body.empty() && request.method == "POST") {
            curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request.body.c_str());
        }
        
        // Header'ları ayarla
        struct curl_slist* headers = nullptr;
        for (const auto& header : request.headers) {
            std::string header_line = header.first + ": " + header.second;
            headers = curl_slist_append(headers, header_line.c_str());
        }
        
        if (headers) {
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
        }
        
        // Response'u yakala
        std::string response_body;
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response_body);
        
        // İsteği yap
        CURLcode res = curl_easy_perform(curl_handle);
        
        if (res == CURLE_OK) {
            // Status code'u al
            long status_code;
            curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &status_code);
            response.status_code = static_cast<int>(status_code);
            response.body = response_body;
            
            // Response header'larını al (basit implementasyon)
            char* content_type;
            if (curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &content_type) == CURLE_OK && content_type) {
                response.headers["Content-Type"] = content_type;
            }
        } else {
            response.status_code = 500;
            response.body = "Ağ hatası: " + std::string(curl_easy_strerror(res));
        }
        
        // Header listesini temizle
        if (headers) {
            curl_slist_free_all(headers);
        }
        
        return response;
    }
    
    bool IsInitialized() const {
        return curl_handle != nullptr;
    }
};

// ComperEngine'in NetworkManager kullanımı
NetworkResponse ComperEngine::MakeRequest(const NetworkRequest& request) {
    if (!network_manager) {
        NetworkResponse error_response;
        error_response.status_code = 500;
        error_response.body = "NetworkManager başlatılmadı";
        return error_response;
    }
    
    return network_manager->MakeRequest(request);
}

} // namespace Comper
