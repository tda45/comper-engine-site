#include "../Includes/comper_core.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace Comper {

// ComperEngine ana implementasyonu
ComperEngine::ComperEngine() 
    : network_manager(nullptr),
      html_parser(nullptr),
      renderer(nullptr),
      inspector(nullptr) {
}

ComperEngine::~ComperEngine() {
    Shutdown();
}

bool ComperEngine::Initialize() {
    std::cout << "Comper Engine başlatılıyor..." << std::endl;
    
    try {
        // NetworkManager'ı başlat
        network_manager = std::make_unique<NetworkManager>();
        if (!network_manager->IsInitialized()) {
            std::cerr << "NetworkManager başlatılamadı!" << std::endl;
            return false;
        }
        
        // HTMLParser'ı başlat
        html_parser = std::make_unique<HTMLParser>();
        
        // Renderer'ı başlat
        renderer = std::make_unique<ComperRenderer>(&current_theme);
        renderer->SetWindowSize(800, 600);
        
        // DevTools Inspector'ı başlat
        inspector = std::make_unique<DevToolsInspector>(&current_theme);
        
        std::cout << "Comper Engine başarıyla başlatıldı!" << std::endl;
        std::cout << "Tema: Gece Kırmızısı (#050505, #E60000)" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ComperEngine başlatma hatası: " << e.what() << std::endl;
        return false;
    }
}

void ComperEngine::Shutdown() {
    std::cout << "Comper Engine kapatılıyor..." << std::endl;
    
    // Tüm bileşenleri sırayla temizle
    inspector.reset();
    renderer.reset();
    html_parser.reset();
    network_manager.reset();
    
    std::cout << "Comper Engine kapatıldı." << std::endl;
}

void ComperEngine::SetTheme(const Theme& theme) {
    current_theme = theme;
    
    // Renderer'a temayı uygula
    if (renderer) {
        // Renderer zaten theme pointer'ını kullanıyor
        std::cout << "Tema güncellendi" << std::endl;
    }
}

const Theme& ComperEngine::GetTheme() const {
    return current_theme;
}

NetworkResponse ComperEngine::LoadPage(const std::string& url) {
    std::cout << "Sayfa yükleniyor: " << url << std::endl;
    
    NetworkRequest request(url);
    NetworkResponse response = MakeRequest(request);
    
    if (response.status_code == 200) {
        std::cout << "Sayfa başarıyla yüklendi (" << response.body.length() << " bytes)" << std::endl;
        
        // HTML'i parse et
        auto parsed_html = ParseHTML(response.body);
        if (parsed_html) {
            std::cout << "HTML başarıyla parse edildi" << std::endl;
            
            // Sayfayı render et
            RenderPage(parsed_html);
        } else {
            std::cerr << "HTML parse edilemedi" << std::endl;
        }
    } else {
        std::cerr << "Sayfa yüklenemedi. Status: " << response.status_code << std::endl;
        std::cerr << "Hata: " << response.body << std::endl;
    }
    
    return response;
}

void ComperEngine::RunEventLoop() {
    std::cout << "Comper Engine olay döngüsü başlatıldı" << std::endl;
    std::cout << "Çıkmak için Ctrl+C basın" << std::endl;
    
    bool running = true;
    
    while (running) {
        // Basit olay döngüsü
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Klavye girdilerini kontrol et (basit implementasyon)
        // Gerçek implementasyonda platform'a özel olay sistemi gerekir
        
        // DevTools inspector kontrolü
        if (inspector && inspector->IsVisible()) {
            // Inspector aktifse özel komutları işle
        }
    }
}

void ComperEngine::StopEventLoop() {
    std::cout << "Olay döngüsü durduruluyor..." << std::endl;
    // Olay döngüsünü durdurmak için flag ayarla
}

} // namespace Comper

// Ana fonksiyon - test için
int main() {
    try {
        Comper::ComperEngine engine;
        
        // Motoru başlat
        if (!engine.Initialize()) {
            std::cerr << "Motor başlatılamadı!" << std::endl;
            return 1;
        }
        
        // Test sayfası yükle
        std::string test_html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Comper Engine Test</title>
</head>
<body>
    <h1>Comper Engine'e Hoş Geldiniz!</h1>
    <p>Bu, Comper Browser Engine'in test sayfasıdır.</p>
    <div>
        <button>Tıkla Beni!</button>
        <input type="text" placeholder="Ara..." />
    </div>
    <p>
        <a href="https://github.com/comper">Comper GitHub</a>
    </p>
</body>
</html>
        )";
        
        // HTML'i parse et ve render et
        auto parsed = engine.ParseHTML(test_html);
        if (parsed) {
            engine.RenderPage(parsed);
        }
        
        // DevTools'u göster
        engine.ShowInspector();
        
        // Olay döngüsünü çalıştır (basit test için 5 saniye)
        std::cout << "\nTest çalışıyor... 5 saniye sonra kapanacak." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Temizle
        engine.Shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Ana program hatası: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
