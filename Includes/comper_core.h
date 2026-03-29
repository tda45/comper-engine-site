#ifndef COMPER_CORE_H
#define COMPER_CORE_H

// Comper Engine - Tarayıcı Motoru İskeleti
// Low-level C++ Tarayıcı Motoru
// Developer: Comper Team
// License: MIT

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace Comper {

// Temel renk yapısı
struct Color {
    int r, g, b;
    Color(int red = 0, int green = 0, int blue = 0) : r(red), g(green), b(blue) {}
};

// Temel tema yapısı
struct Theme {
    Color background;     // #050505 - Gece kırmızısı arka plan
    Color accent;         // #E60000 - Vurgu rengi
    Color text;           // Metin rengi
    Color button_bg;      // Buton arka plan
    Color button_hover;   // Buton hover rengi
    
    Theme() : 
        background(5, 5, 5), 
        accent(230, 0, 0), 
        text(255, 255, 255),
        button_bg(40, 40, 40),
        button_hover(60, 60, 60) {}
};

// HTML element yapısı
struct HTMLElement {
    std::string tag;
    std::string content;
    std::map<std::string, std::string> attributes;
    std::vector<std::shared_ptr<HTMLElement>> children;
    
    HTMLElement(const std::string& t) : tag(t) {}
};

// Ağ isteği yapısı
struct NetworkRequest {
    std::string url;
    std::string method;
    std::map<std::string, std::string> headers;
    std::string body;
    
    NetworkRequest(const std::string& u) : url(u), method("GET") {}
};

// Ağ yanıtı yapısı
struct NetworkResponse {
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;
    
    NetworkResponse() : status_code(200) {}
};

// İleri bildirimler
class NetworkManager;
class HTMLParser;
class ComperRenderer;
class DevToolsInspector;

// Ana motor sınıfı
class ComperEngine {
private:
    std::unique_ptr<NetworkManager> network_manager;
    std::unique_ptr<HTMLParser> html_parser;
    std::unique_ptr<ComperRenderer> renderer;
    std::unique_ptr<DevToolsInspector> inspector;
    Theme current_theme;
    
public:
    ComperEngine();
    ~ComperEngine();
    
    // Motor başlatma ve kapatma
    bool Initialize();
    void Shutdown();
    
    // Tema yönetimi
    void SetTheme(const Theme& theme);
    const Theme& GetTheme() const;
    
    // Sayfa yükleme
    NetworkResponse LoadPage(const std::string& url);
    
    // HTML işleme
    std::shared_ptr<HTMLElement> ParseHTML(const std::string& html);
    
    // Çizim işlemleri
    void RenderPage(std::shared_ptr<HTMLElement> root);
    void ClearScreen();
    
    // DevTools
    void ShowInspector();
    void HideInspector();
    bool IsInspectorVisible() const;
    
    // Ağ yönetimi
    NetworkResponse MakeRequest(const NetworkRequest& request);
    
    // Olay döngüsü
    void RunEventLoop();
    void StopEventLoop();
};

} // namespace Comper

#endif // COMPER_CORE_H
