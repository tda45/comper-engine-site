/*
 * Comper Engine SDK - Basic Browser Example
 * 
 * Bu örnek, Comper Engine SDK kullanarak basit bir tarayıcı oluşturmayı gösterir.
 * Temel özellikler: gezinme, HTML parse etme, rendering, Night Red Ultra teması.
 * 
 * Features:
 * - Web sayfası yükleme
 * - HTML parsing ve DOM manipulation
 * - Night Red Ultra tema
 * - Interactive command line interface
 * - Error handling
 * - Performance monitoring
 */

#include "../../../Includes/Core/comper_core.h"
#include "../../../Includes/SDK/comper_sdk.h"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace ComperCore;
using namespace ComperSDK;

class BasicBrowserExample {
private:
    std::unique_ptr<ComperEngine> engine;
    std::shared_ptr<DOMDocument> current_document;
    std::string current_url;
    std::vector<std::string> history;
    int history_index = -1;
    
    // Performance tracking
    struct PerformanceMetrics {
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        double page_load_time = 0.0;
        size_t total_bytes_downloaded = 0;
        int pages_loaded = 0;
        
        void Reset() {
            start_time = std::chrono::high_resolution_clock::now();
            page_load_time = 0.0;
            total_bytes_downloaded = 0;
            pages_loaded = 0;
        }
        
        void UpdatePageLoad(double load_time, size_t bytes) {
            page_load_time = load_time;
            total_bytes_downloaded += bytes;
            pages_loaded++;
        }
        
        void PrintStats() const {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
            
            std::cout << "\n📊 Performans İstatistikleri:" << std::endl;
            std::cout << "  Çalışma Süresi: " << duration.count() << " saniye" << std::endl;
            std::cout << "  Yüklenen Sayfalar: " << pages_loaded << std::endl;
            std::cout << "  Toplam İndirilen: " << (total_bytes_downloaded / 1024.0) << " KB" << std::endl;
            std::cout << "  Son Yükleme Süresi: " << std::fixed << std::setprecision(2) 
                      << page_load_time << " saniye" << std::endl;
        }
    } performance;
    
public:
    BasicBrowserExample() {
        std::cout << "🚀 Comper Engine Basic Browser başlatılıyor..." << std::endl;
        
        // Engine'i oluştur
        engine = std::make_unique<ComperEngine>();
        
        // Night Red Ultra temasını uygula
        ApplyNightRedTheme();
        
        // DevTools'u başlat
        InitializeDevTools();
        
        // Performans metriklerini sıfırla
        performance.Reset();
        
        std::cout << "✅ Browser başarıyla başlatıldı!" << std::endl;
    }
    
    ~BasicBrowserExample() {
        std::cout << "\n📊 Son Performans İstatistikleri:" << std::endl;
        performance.PrintStats();
        std::cout << "👋 Browser kapatılıyor..." << std::endl;
    }
    
private:
    void ApplyNightRedTheme() {
        std::cout << "🎨 Night Red Ultra teması uygulanıyor..." << std::endl;
        
        // Neon efektleri
        NightRedUltra::EnableNeonGlow(true);
        NightRedUltra::SetNeonIntensity(0.8f);
        NightRedUltra::SetGlowColor(230, 0, 0); // #E60000
        
        // Kenar animasyonları
        NightRedUltra::EnableWindowEffects(true);
        NightRedUltra::SetBorderAnimation("pulse");
        
        // 4K desteği
        NightRedUltra::Set4KMode(true);
        NightRedUltra::SetMaxFPS(120);
        
        std::cout << "✨ Night Red Ultra teması aktif!" << std::endl;
    }
    
    void InitializeDevTools() {
        std::cout << "🔧 DevTools başlatılıyor..." << std::endl;
        
        // Inspector'ı etkinleştir
        ComperInspector::Show();
        ComperInspector::EnableMemoryProfiler(true);
        ComperInspector::EnableNetworkAnalyzer(true);
        ComperInspector::EnablePerformanceProfiler(true);
        ComperInspector::StartRealTimeMonitoring();
        
        std::cout << "🔍 DevTools aktif!" << std::endl;
    }
    
public:
    bool NavigateToURL(const std::string& url) {
        std::cout << "\n🌐 Navigating to: " << url << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // URL'i normalize et
            std::string normalized_url = NormalizeURL(url);
            
            // Sayfayı yükle
            NetworkResponse response = engine->LoadPage(normalized_url);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            double load_time = duration.count() / 1000.0;
            
            if (response.IsSuccess()) {
                std::cout << "✅ Sayfa başarıyla yüklendi!" << std::endl;
                std::cout << "📄 Status: " << response.status_code << std::endl;
                std::cout << "📏 Content Length: " << response.content_length << " bytes" << std::endl;
                std::cout << "⏱️ Load Time: " << std::fixed << std::setprecision(2) 
                          << load_time << " saniye" << std::endl;
                std::cout << "🎯 Content Type: " << response.content_type << std::endl;
                
                // HTML'i parse et
                if (response.content_type.find("text/html") != std::string::npos) {
                    ParseHTMLContent(response.body);
                } else {
                    std::cout << "📄 Non-HTML content detected" << std::endl;
                }
                
                // History'e ekle
                AddToHistory(normalized_url);
                
                // Performansı güncelle
                performance.UpdatePageLoad(load_time, response.content_length);
                
                current_url = normalized_url;
                return true;
                
            } else {
                std::cout << "❌ Sayfa yüklenemedi!" << std::endl;
                std::cout << "🔧 Status Code: " << response.status_code << std::endl;
                std::cout << "📝 Error Message: " << response.error_message << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cout << "💥 Exception occurred: " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    std::string NormalizeURL(const std::string& url) {
        std::string normalized = url;
        
        // HTTP/HTTPS ekle
        if (normalized.find("://") == std::string::npos) {
            normalized = "https://" + normalized;
        }
        
        // Slash ekle
        if (normalized.back() != '/') {
            normalized += "/";
        }
        
        return normalized;
    }
    
    void ParseHTMLContent(const std::string& html) {
        std::cout << "📄 HTML content parsing..." << std::endl;
        
        try {
            // HTML'i parse et
            current_document = engine->ParseHTML(html);
            
            if (!current_document) {
                std::cout << "❌ HTML parsing failed!" << std::endl;
                return;
            }
            
            // Başlığı bul
            auto title_element = current_document->QuerySelector("title");
            if (title_element) {
                std::string title = title_element->GetTextContent();
                std::cout << "📝 Sayfa Başlığı: " << title << std::endl;
            }
            
            // Meta bilgilerini göster
            ShowMetaInfo();
            
            // Link'leri göster
            ShowLinks();
            
            // Form'ları göster
            ShowForms();
            
            std::cout << "✅ HTML parsing completed!" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "💥 HTML parsing error: " << e.what() << std::endl;
        }
    }
    
    void ShowMetaInfo() {
        std::cout << "\n📋 Meta Bilgileri:" << std::endl;
        
        // Description
        auto desc_element = current_document->QuerySelector("meta[name='description']");
        if (desc_element) {
            std::string description = desc_element->GetAttribute("content");
            std::cout << "  📝 Description: " << description << std::endl;
        }
        
        // Keywords
        auto keywords_element = current_document->QuerySelector("meta[name='keywords']");
        if (keywords_element) {
            std::string keywords = keywords_element->GetAttribute("content");
            std::cout << "  🏷️ Keywords: " << keywords << std::endl;
        }
        
        // Viewport
        auto viewport_element = current_document->QuerySelector("meta[name='viewport']");
        if (viewport_element) {
            std::string viewport = viewport_element->GetAttribute("content");
            std::cout << "  📱 Viewport: " << viewport << std::endl;
        }
    }
    
    void ShowLinks() {
        auto links = current_document->QuerySelectorAll("a[href]");
        std::cout << "\n🔗 Link'ler (" << links.size() << " adet):" << std::endl;
        
        for (size_t i = 0; i < std::min(links.size(), size_t(10)); ++i) {
            auto link = links[i];
            std::string href = link->GetAttribute("href");
            std::string text = link->GetTextContent();
            
            if (text.empty()) {
                text = href;
            }
            
            std::cout << "  " << (i + 1) << ". " << text << " -> " << href << std::endl;
        }
        
        if (links.size() > 10) {
            std::cout << "  ... ve " << (links.size() - 10) << " link daha" << std::endl;
        }
    }
    
    void ShowForms() {
        auto forms = current_document->QuerySelectorAll("form");
        std::cout << "\n📝 Form'lar (" << forms.size() << " adet):" << std::endl;
        
        for (size_t i = 0; i < forms.size(); ++i) {
            auto form = forms[i];
            std::string action = form->GetAttribute("action");
            std::string method = form->GetAttribute("method");
            
            std::cout << "  " << (i + 1) << ". Form: " << method << " -> " << action << std::endl;
            
            // Input'ları göster
            auto inputs = form->QuerySelectorAll("input");
            for (const auto& input : inputs) {
                std::string type = input->GetAttribute("type");
                std::string name = input->GetAttribute("name");
                std::cout << "    - Input: " << type << " (name: " << name << ")" << std::endl;
            }
        }
    }
    
    void AddToHistory(const std::string& url) {
        // Eğer mevcut URL'den sonraki bir yere gidiliyorsa, history'i kes
        if (history_index < static_cast<int>(history.size()) - 1) {
            history.resize(history_index + 1);
        }
        
        history.push_back(url);
        history_index = history.size() - 1;
    }
    
public:
    void GoBack() {
        if (history_index > 0) {
            history_index--;
            std::string url = history[history_index];
            std::cout << "⬅️ Geri gidiliyor: " << url << std::endl;
            NavigateToURL(url);
        } else {
            std::cout << "❌ Geri gidecek sayfa yok!" << std::endl;
        }
    }
    
    void GoForward() {
        if (history_index < static_cast<int>(history.size()) - 1) {
            history_index++;
            std::string url = history[history_index];
            std::cout << "➡️ İleri gidiliyor: " << url << std::endl;
            NavigateToURL(url);
        } else {
            std::cout << "❌ İleri gidecek sayfa yok!" << std::endl;
        }
    }
    
    void ShowHistory() {
        std::cout << "\n📚 Geçmiş (" << history.size() << " sayfa):" << std::endl;
        
        for (size_t i = 0; i < history.size(); ++i) {
            std::string marker = (i == history_index) ? "►" : " ";
            std::cout << marker << " " << (i + 1) << ". " << history[i] << std::endl;
        }
    }
    
    void ShowPageInfo() {
        if (!current_document) {
            std::cout << "❌ Hiçbir sayfa yüklenmemiş!" << std::endl;
            return;
        }
        
        std::cout << "\n📄 Sayfa Bilgileri:" << std::endl;
        std::cout << "🌐 URL: " << current_url << std::endl;
        
        // Başlık
        auto title_element = current_document->QuerySelector("title");
        if (title_element) {
            std::cout << "📝 Başlık: " << title_element->GetTextContent() << std::endl;
        }
        
        // Element sayısı
        auto all_elements = current_document->QuerySelectorAll("*");
        std::cout << "🏗️ Element Sayısı: " << all_elements.size() << std::endl;
        
        // Link sayısı
        auto links = current_document->QuerySelectorAll("a[href]");
        std::cout << "🔗 Link Sayısı: " << links.size() << std::endl;
        
        // Resim sayısı
        auto images = current_document->QuerySelectorAll("img[src]");
        std::cout << "🖼️ Resim Sayısı: " << images.size() << std::endl;
        
        // Form sayısı
        auto forms = current_document->QuerySelectorAll("form");
        std::cout << "📝 Form Sayısı: " << forms.size() << std::endl;
    }
    
    void ShowHelp() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "           🌟 COMPER BROWSER - YARDIM" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "📋 Navigasyon Komutları:" << std::endl;
        std::cout << "  nav <url>           - Sayfaya git" << std::endl;
        std::cout << "  google <query>       - Google'da ara" << std::endl;
        std::cout << "  back                 - Geri git" << std::endl;
        std::cout << "  forward              - İleri git" << std::endl;
        std::cout << "  history              - Geçmişi göster" << std::endl;
        std::cout << "\n📄 Sayfa Komutları:" << std::endl;
        std::cout << "  info                 - Sayfa bilgileri" << std::endl;
        std::cout << "  links                - Link'leri göster" << std::endl;
        std::cout << "  forms                - Form'ları göster" << std::endl;
        std::cout << "  title                - Sayfa başlığı" << std::endl;
        std::cout << "\n🎨 Tema Komutları:" << std::endl;
        std::cout << "  theme                - Tema bilgisi" << std::endl;
        std::cout << "  neon <intensity>     - Neon yoğunluğu" << std::endl;
        std::cout << "  color <r> <g> <b>   - Neon rengi" << std::endl;
        std::cout << "\n🔧 Araçlar:" << std::endl;
        std::cout << "  devtools             - DevTools'u aç" << std::endl;
        std::cout << "  stats                - Performans istatistikleri" << std::endl;
        std::cout << "  adblock <on/off>     - AdBlocker ayarla" << std::endl;
        std::cout << "\n💾 Diğer:" << std::endl;
        std::cout << "  clear                - Ekranı temizle" << std::endl;
        std::cout << "  help                 - Bu yardımı göster" << std::endl;
        std::cout << "  exit/quit            - Çıkış" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    void RunInteractive() {
        std::cout << "\n🎯 Comper Browser Interactive Mode" << std::endl;
        std::cout << "Night Red Ultra Teması Aktif ✨" << std::endl;
        std::cout << "Yardım için 'help' yazın" << std::endl;
        
        std::string command;
        while (true) {
            std::cout << "\nComper> ";
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            // Komutu parse et
            std::istringstream iss(command);
            std::string cmd;
            iss >> cmd;
            
            if (cmd == "exit" || cmd == "quit") {
                break;
            } else if (cmd == "nav") {
                std::string url;
                iss >> url;
                if (!url.empty()) {
                    NavigateToURL(url);
                } else {
                    std::cout << "Kullanım: nav <url>" << std::endl;
                }
            } else if (cmd == "google") {
                std::string query;
                std::getline(iss, query);
                if (!query.empty() && query[0] == ' ') {
                    query = query.substr(1);
                }
                if (!query.empty()) {
                    std::string search_url = "https://www.google.com/search?q=" + query;
                    NavigateToURL(search_url);
                } else {
                    std::cout << "Kullanım: google <query>" << std::endl;
                }
            } else if (cmd == "back") {
                GoBack();
            } else if (cmd == "forward") {
                GoForward();
            } else if (cmd == "history") {
                ShowHistory();
            } else if (cmd == "info") {
                ShowPageInfo();
            } else if (cmd == "links") {
                if (current_document) {
                    ShowLinks();
                } else {
                    std::cout << "❌ Hiçbir sayfa yüklenmemiş!" << std::endl;
                }
            } else if (cmd == "forms") {
                if (current_document) {
                    ShowForms();
                } else {
                    std::cout << "❌ Hiçbir sayfa yüklenmemiş!" << std::endl;
                }
            } else if (cmd == "title") {
                if (current_document) {
                    auto title_element = current_document->QuerySelector("title");
                    if (title_element) {
                        std::cout << "📝 Sayfa Başlığı: " << title_element->GetTextContent() << std::endl;
                    } else {
                        std::cout << "❌ Sayfa başlığı bulunamadı!" << std::endl;
                    }
                } else {
                    std::cout << "❌ Hiçbir sayfa yüklenmemiş!" << std::endl;
                }
            } else if (cmd == "theme") {
                std::cout << "🎨 Night Red Ultra Tema:" << std::endl;
                std::cout << "  Neon Glow: ✅ Aktif" << std::endl;
                std::cout << "  Border Effects: ✅ Aktif" << std::endl;
                std::cout << "  4K Mode: ✅ Aktif" << std::endl;
                std::cout << "  FPS Limit: 120" << std::endl;
            } else if (cmd == "neon") {
                float intensity;
                iss >> intensity;
                if (intensity >= 0.0f && intensity <= 1.0f) {
                    NightRedUltra::SetNeonIntensity(intensity);
                    std::cout << "✨ Neon yoğunluğu ayarlandı: " << intensity << std::endl;
                } else {
                    std::cout << "Kullanım: neon <0.0-1.0>" << std::endl;
                }
            } else if (cmd == "color") {
                float r, g, b;
                iss >> r >> g >> b;
                if (r >= 0.0f && r <= 255.0f && g >= 0.0f && g <= 255.0f && b >= 0.0f && b <= 255.0f) {
                    NightRedUltra::SetGlowColor(r, g, b);
                    std::cout << "🎨 Neon rengi ayarlandı: RGB(" << r << ", " << g << ", " << b << ")" << std::endl;
                } else {
                    std::cout << "Kullanım: color <0-255> <0-255> <0-255>" << std::endl;
                }
            } else if (cmd == "devtools") {
                ComperInspector::DisplayDashboard();
            } else if (cmd == "stats") {
                performance.PrintStats();
            } else if (cmd == "adblock") {
                std::string state;
                iss >> state;
                if (state == "on") {
                    AdBlockerEngine::Enable(true);
                    std::cout << "🛡️ AdBlocker etkinleştirildi" << std::endl;
                } else if (state == "off") {
                    AdBlockerEngine::Enable(false);
                    std::cout << "🛡️ AdBlocker devre dışı" << std::endl;
                } else {
                    std::cout << "Kullanım: adblock <on/off>" << std::endl;
                }
            } else if (cmd == "clear") {
                // Windows için ekran temizleme
                system("cls");
                std::cout << "🌟 Comper Browser - Ekran temizlendi" << std::endl;
            } else if (cmd == "help") {
                ShowHelp();
            } else {
                std::cout << "❌ Bilinmeyen komut: " << cmd << std::endl;
                std::cout << "Yardım için 'help' yazın" << std::endl;
            }
        }
    }
};

int main() {
    try {
        std::cout << "🎯 Comper Engine SDK - Basic Browser Example" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "Night Red Ultra Grafik Motoru" << std::endl;
        std::cout << "HTML5 Parsing & Rendering" << std::endl;
        std::cout << "Interactive Command Line Interface" << std::endl;
        std::cout << "=================================================" << std::endl;
        
        // Browser'ı oluştur ve çalıştır
        BasicBrowserExample browser;
        browser.RunInteractive();
        
        std::cout << "\n👋 Görüşürüz!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
 * 🎯 BU ÖRNEKDE NE ÖĞRENDİNİZ?
 * 
 * ✅ Comper Engine SDK kurulumu
 * ✅ Night Red Ultra teması uygulama
 * ✅ Web sayfası yükleme ve parsing
 * ✅ HTML DOM manipulation
 * ✅ Interactive command line interface
 * ✅ Error handling ve exception management
 * ✅ Performance monitoring
 * ✅ History management
 * ✅ DevTools entegrasyonu
 * ✅ AdBlocker entegrasyonu
 * 
 * 🚀 SONRAKİ ADIMLAR:
 * 
 * 1. GUI arayüzü ekleyin (Windows API)
 * 2. Sekme yönetimi implement edin
 * 3. Bookmark sistemi oluşturun
 * 4. Download manager ekleyin
 * 5. Settings page oluşturun
 * 
 * 📚 DAHA FAZLA BİLGİ İÇİN:
 * 
 * - [Core API Documentation](../../API/Core_API.md)
 * - [SDK API Documentation](../../API/SDK_API.md)
 * - [Advanced Browser Example](../AdvancedBrowser/)
 * - [Enterprise Browser Example](../EnterpriseBrowser/)
 */
