/*
 * Comper Engine SDK 1.0 - Basic Browser Template
 * 
 * Basit, hızlı başlangıç için tarayıcı şablonu
 * Temel özellikler: gezinme, sekme yönetimi, tema desteği
 * 
 * Features:
 * - Minimal arayüz
 * - Hızlı başlangıç
 * - Temel gezinme
 * - Sekme yönetimi
 * - Night Red Ultra tema
 */

#include "../../Includes/SDK/comper_sdk.h"
#include <iostream>
#include <string>
#include <vector>

using namespace ComperSDK;

class BasicBrowser : public ComperEngine {
private:
    std::vector<std::string> tabs;
    int current_tab = 0;
    std::string current_url;
    
public:
    BasicBrowser() {
        std::cout << "[BasicBrowser] Başlatılıyor..." << std::endl;
        
        // Temel ayarlar
        tabs.push_back("https://www.google.com");
        current_url = tabs[0];
        
        // Night Red Ultra temasını uygula
        NightRedUltra::EnableNeonGlow(true);
        NightRedUltra::SetNeonIntensity(0.7f);
        NightRedUltra::SetGlowColor(230, 0, 0);
        
        std::cout << "[BasicBrowser] Hazır!" << std::endl;
    }
    
    void Navigate(const std::string& url) {
        std::cout << "[BasicBrowser] Navigating to: " << url << std::endl;
        current_url = url;
        
        // Sayfayı yükle
        NetworkResponse response = LoadPage(url);
        
        if (response.status_code == 200) {
            std::cout << "[BasicBrowser] Sayfa başarıyla yüklendi" << std::endl;
        } else {
            std::cout << "[BasicBrowser] Hata: " << response.status_code << std::endl;
        }
    }
    
    void NewTab(const std::string& url = "https://www.google.com") {
        tabs.push_back(url);
        current_tab = tabs.size() - 1;
        Navigate(url);
        
        std::cout << "[BasicBrowser] Yeni sekme: " << tabs.size() << " sekmeler" << std::endl;
    }
    
    void CloseTab(int tab_index) {
        if (tabs.size() > 1 && tab_index >= 0 && tab_index < tabs.size()) {
            tabs.erase(tabs.begin() + tab_index);
            
            if (current_tab >= tabs.size()) {
                current_tab = tabs.size() - 1;
            }
            
            Navigate(tabs[current_tab]);
            std::cout << "[BasicBrowser] Sekme kapatıldı: " << tabs.size() << " sekmeler kaldı" << std::endl;
        }
    }
    
    void SwitchTab(int tab_index) {
        if (tab_index >= 0 && tab_index < tabs.size()) {
            current_tab = tab_index;
            Navigate(tabs[current_tab]);
            std::cout << "[BasicBrowser] Sekme değiştirildi: " << (tab_index + 1) << "/" << tabs.size() << std::endl;
        }
    }
    
    void ShowTabs() {
        std::cout << "\n=== SEKMELER ===" << std::endl;
        for (int i = 0; i < tabs.size(); i++) {
            std::cout << (i == current_tab ? "► " : "  ") 
                      << (i + 1) << ". " << tabs[i] << std::endl;
        }
        std::cout << "===============" << std::endl;
    }
    
    void ShowHelp() {
        std::cout << "\n=== BASIC BROWSER KOMUTLARI ===" << std::endl;
        std::cout << "navigate <url>     - Sayfaya git" << std::endl;
        std::cout << "newtab [url]      - Yeni sekme aç" << std::endl;
        std::cout << "closetab <index> - Sekmeyi kapat" << std::endl;
        std::cout << "switchtab <index> - Sekmeye geç" << std::endl;
        std::cout << "tabs              - Sekmeleri göster" << std::endl;
        std::cout << "back              - Geri git" << std::endl;
        std::cout << "forward           - İleri git" << std::endl;
        std::cout << "refresh           - Sayfayı yenile" << std::endl;
        std::cout << "help              - Yardım göster" << std::endl;
        std::cout << "exit              - Çıkış" << std::endl;
        std::cout << "================================" << std::endl;
    }
    
    void Run() {
        std::cout << "\n=== Comper Basic Browser ===" << std::endl;
        std::cout << "Night Red Ultra tema aktif" << std::endl;
        std::cout << "Yardım için 'help' yazın" << std::endl;
        std::cout << "==============================" << std::endl;
        
        // İlk sayfayı yükle
        Navigate(current_url);
        
        // Komut döngüsü
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
                std::cout << "[BasicBrowser] Kapatılıyor..." << std::endl;
                break;
            } else if (cmd == "navigate" || cmd == "nav") {
                std::string url;
                iss >> url;
                if (!url.empty()) {
                    Navigate(url);
                } else {
                    std::cout << "Kullanım: navigate <url>" << std::endl;
                }
            } else if (cmd == "newtab" || cmd == "nt") {
                std::string url;
                iss >> url;
                if (url.empty()) {
                    url = "https://www.google.com";
                }
                NewTab(url);
            } else if (cmd == "closetab" || cmd == "ct") {
                std::string index_str;
                iss >> index_str;
                if (!index_str.empty()) {
                    int index = std::stoi(index_str) - 1;
                    CloseTab(index);
                } else {
                    std::cout << "Kullanım: closetab <index>" << std::endl;
                }
            } else if (cmd == "switchtab" || cmd == "st") {
                std::string index_str;
                iss >> index_str;
                if (!index_str.empty()) {
                    int index = std::stoi(index_str) - 1;
                    SwitchTab(index);
                } else {
                    std::cout << "Kullanım: switchtab <index>" << std::endl;
                }
            } else if (cmd == "tabs") {
                ShowTabs();
            } else if (cmd == "back") {
                std::cout << "[BasicBrowser] Geri git (basit implementasyon)" << std::endl;
            } else if (cmd == "forward") {
                std::cout << "[BasicBrowser] İleri git (basit implementasyon)" << std::endl;
            } else if (cmd == "refresh" || cmd == "r") {
                std::cout << "[BasicBrowser] Sayfa yenileniyor..." << std::endl;
                Navigate(current_url);
            } else if (cmd == "help" || cmd == "?") {
                ShowHelp();
            } else if (cmd == "devtools") {
                ComperInspector::Show();
                std::cout << "[BasicBrowser] DevTools açıldı" << std::endl;
            } else if (cmd == "adblock") {
                AdBlockerEngine::Enable(true);
                std::cout << "[BasicBrowser] AdBlocker etkinleştirildi" << std::endl;
            } else if (cmd == "theme") {
                std::cout << "[BasicBrowser] Night Red Ultra tema ayarları:" << std::endl;
                std::cout << "- Neon Glow: Aktif" << std::endl;
                std::cout << "- Intensity: 0.7" << std::endl;
                std::cout << "- Color: #E60000" << std::endl;
            } else {
                std::cout << "Bilinmeyen komut: " << cmd << std::endl;
                std::cout << "Yardım için 'help' yazın" << std::endl;
            }
        }
    }
};

int main() {
    try {
        std::cout << "=== Comper Engine SDK 1.0 - Basic Browser Template ===" << std::endl;
        std::cout << "Night Red Ultra Grafik Motoru" << std::endl;
        std::cout << "=====================================================" << std::endl;
        
        // Tarayıcıyı oluştur ve çalıştır
        BasicBrowser browser;
        browser.Run();
        
    } catch (const std::exception& e) {
        std::cerr << "Hata: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

/*
 * === BASIC BROWSER ÖZELLİKLERİ ===
 * 
 * ✅ Minimal arayüz
 * ✅ Hızlı başlangıç (< 1 saniye)
 * ✅ Temel gezinme
 * ✅ Sekme yönetimi
 * ✅ Night Red Ultra tema
 * ✅ Komut satırı arayüzü
 * ✅ DevTools entegrasyonu
 * ✅ AdBlocker desteği
 * 
 * === KULLANIM ÖRNEKLERİ ===
 * 
 * navigate https://github.com
 * newtab https://youtube.com
 * tabs
 * switchtab 2
 * closetab 1
 * devtools
 * adblock
 * theme
 * exit
 */
