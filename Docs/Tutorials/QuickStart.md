# Comper Engine SDK - Quick Start Tutorial

**Hızlı Başlangıç Kılavuzu - 5 Dakikada İlk Tarayıcınız**

## 🎯 Hedef

Bu tutorial'da 5 dakika içinde Comper Engine SDK kullanarak basit bir tarayıcı oluşturacağız:

1. **Kurulum** - SDK'yı hazırlama
2. **Proje Oluşturma** - CMake projesi
3. **Temel Tarayıcı** - Gezinme ve rendering
4. **Night Red Ultra** - Tema uygulama
5. **Derleme ve Çalıştırma** - Build ve test

## 📋 Gereksinimler

- **Windows 10/11** (64-bit)
- **Visual Studio 2022** (Community veya üstü)
- **CMake 3.16+**
- **Git** (SDK klonlamak için)

## 🚀 Adım 1: SDK Kurulumu

### SDK'yı Klonlayın
```bash
git clone https://github.com/tda45/comper-engine.git
cd comper-engine
```

### Build Script'ini Çalıştırın
```bash
# Tek tık derleme
ComperBuilder.bat
```

Bu script otomatik olarak:
- ✅ Gerekli araçları kontrol eder
- ✅ CMake yapılandırmasını yapar
- ✅ Proje derler
- ✅ Bağımlılıkları yükler
- ✅ Test çalıştırır

**Sonuç:** SDK hazır ve test edilmiş durumda!

## 🏗️ Adım 2: Proje Oluşturma

### CMakeLists.txt Oluşturun
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyComperBrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Comper Engine SDK'yı ekle
set(COMPER_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
include_directories(${COMPER_SDK_ROOT}/Includes)
include_directories(${COMPER_SDK_ROOT}/Includes/Core)
include_directories(${COMPER_SDK_ROOT}/Includes/SDK)

# Link kütüphaneleri
find_library(COMPER_CORE_LIB NAMES ComperCore PATHS ${COMPER_SDK_ROOT}/Binaries)
find_library(COMPER_SDK_LIB NAMES ComperSDK PATHS ${COMPER_SDK_ROOT}/Binaries)

# Executable oluştur
add_executable(MyComperBrowser main.cpp)

# Link kütüphaneler
target_link_libraries(MyComperBrowser 
    ${COMPER_CORE_LIB}
    ${COMPER_SDK_LIB}
)

# Windows için
if(WIN32)
    target_link_libraries(MyComperBrowser 
        user32
        gdi32
        shell32
        ole32
    )
endif()
```

### main.cpp Oluşturun
```cpp
#include "Includes/Core/comper_core.h"
#include "Includes/SDK/comper_sdk.h"

using namespace ComperCore;
using namespace ComperSDK;

class MyBrowser : public ComperEngine {
public:
    MyBrowser() {
        std::cout << "🚀 Comper Browser başlatılıyor..." << std::endl;
        
        // Night Red Ultra temasını uygula
        NightRedUltra::EnableNeonGlow(true);
        NightRedUltra::SetNeonIntensity(0.8f);
        NightRedUltra::SetGlowColor(230, 0, 0); // #E60000
        
        std::cout << "✨ Night Red Ultra teması aktif!" << std::endl;
    }
    
    void NavigateToPage(const std::string& url) {
        std::cout << "🌐 Navigating to: " << url << std::endl;
        
        // Sayfayı yükle
        NetworkResponse response = LoadPage(url);
        
        if (response.IsSuccess()) {
            std::cout << "✅ Sayfa başarıyla yüklendi!" << std::endl;
            std::cout << "📄 Content length: " << response.content_length << " bytes" << std::endl;
            
            // HTML'i parse et
            std::shared_ptr<DOMDocument> document = ParseHTML(response.body);
            
            // Başlığı göster
            auto title_element = document->QuerySelector("title");
            if (title_element) {
                std::cout << "📝 Sayfa başlığı: " << title_element->GetTextContent() << std::endl;
            }
            
            // Sayfayı render et
            RenderPage(document);
            
        } else {
            std::cout << "❌ Sayfa yüklenemedi: " << response.status_code << std::endl;
            std::cout << "🔧 Hata: " << response.error_message << std::endl;
        }
    }
    
    void RunInteractive() {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "     🌟 Comper Browser - Interactive Mode" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Komutlar:" << std::endl;
        std::cout << "  nav <url>     - Sayfaya git" << std::endl;
        std::cout << "  google <query> - Google'da ara" << std::endl;
        std::cout << "  theme         - Tema bilgisi" << std::endl;
        std::cout << "  devtools      - DevTools'u aç" << std::endl;
        std::cout << "  exit          - Çıkış" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::string command;
        while (true) {
            std::cout << "\nComper> ";
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            if (command == "exit" || command == "quit") {
                break;
            } else if (command.substr(0, 3) == "nav") {
                std::string url = command.substr(4);
                if (!url.empty()) {
                    NavigateToPage(url);
                } else {
                    std::cout << "Kullanım: nav <url>" << std::endl;
                }
            } else if (command.substr(0, 6) == "google") {
                std::string query = command.substr(7);
                if (!query.empty()) {
                    std::string search_url = "https://www.google.com/search?q=" + query;
                    NavigateToPage(search_url);
                } else {
                    std::cout << "Kullanım: google <query>" << std::endl;
                }
            } else if (command == "theme") {
                std::cout << "🎨 Night Red Ultra Tema:" << std::endl;
                std::cout << "  Neon Glow: ✅ Aktif" << std::endl;
                std::cout << "  Intensity: 0.8" << std::endl;
                std::cout << "  Color: #E60000" << std::endl;
                std::cout << "  Animation: Pulse" << std::endl;
            } else if (command == "devtools") {
                ComperInspector::Show();
                std::cout << "🔧 DevTools açıldı" << std::endl;
            } else {
                std::cout << "Bilinmeyen komut: " << command << std::endl;
                std::cout << "Yardım için komutları kontrol edin" << std::endl;
            }
        }
    }
};

int main() {
    try {
        std::cout << "🎯 Comper Engine SDK - Quick Start" << std::endl;
        std::cout << "======================================" << std::endl;
        
        // Tarayıcıyı oluştur
        MyBrowser browser;
        
        // İnteraktif modu başlat
        browser.RunInteractive();
        
        std::cout << "\n👋 Görüşürüz!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Hata: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## 🔨 Adım 3: Derleme

### Build Klasörü Oluşturun
```bash
mkdir build
cd build
```

### CMake'i Çalıştırın
```bash
cmake .. -G "Visual Studio 17 2022" -A x64
```

### Proje Derleyin
```bash
cmake --build . --config Release
```

### Veya Tek Komutla
```bash
# Debug build
cmake .. -G "Visual Studio 17 2022" -A x64 && cmake --build . --config Debug

# Release build
cmake .. -G "Visual Studio 17 2022" -A x64 && cmake --build . --config Release
```

## 🎮 Adım 4: Çalıştırma

### Tarayıcıyı Çalıştırın
```bash
# Windows Command Prompt
cd build/Release
MyComperBrowser.exe

# Veya PowerShell
.\build\Release\MyComperBrowser.exe
```

### İlk Test
```
🚀 Comper Browser başlatılıyor...
✨ Night Red Ultra teması aktif!

==================================================
     🌟 Comper Browser - Interactive Mode
==================================================
Komutlar:
  nav <url>     - Sayfaya git
  google <query> - Google'da ara
  theme         - Tema bilgisi
  devtools      - DevTools'u aç
  exit          - Çıkış
==================================================

Comper> nav https://example.com
🌐 Navigating to: https://example.com
✅ Sayfa başarıyla yüklendi!
📄 Content length: 1256 bytes
📝 Sayfa başlığı: Example Domain

Comper> google Comper Engine
🌐 Navigating to: https://www.google.com/search?q=Comper Engine
✅ Sayfa başarıyla yüklendi!
📄 Content length: 45230 bytes
📝 Sayfa başlığı: Comper Engine - Google Search

Comper> theme
🎨 Night Red Ultra Tema:
  Neon Glow: ✅ Aktif
  Intensity: 0.8
  Color: #E60000
  Animation: Pulse

Comper> devtools
🔧 DevTools açıldı

Comper> exit
👋 Görüşürüz!
```

## 🎨 Adım 5: Night Red Ultra Tema Özelleştirme

### Tema Ayarları
```cpp
// Neon efektini özelleştir
NightRedUltra::SetNeonIntensity(0.9f);  // Daha yoğun
NightRedUltra::SetGlowColor(255, 0, 0); // Parlak kırmızı

// Kenar animasyonları
NightRedUltra::EnableWindowEffects(true);
NightRedUltra::SetBorderAnimation("pulse");

// 4K modu
NightRedUltra::Set4KMode(true);
NightRedUltra::SetMaxFPS(120);
```

### Özel Renkler
```cpp
// Kendi renklerinizi tanımlayın
Color custom_red(0.95f, 0.1f, 0.1f, 1.0f);  // #F21919
Color dark_bg(0.02f, 0.02f, 0.02f, 1.0f);    // #050505
Color text_white(1.0f, 1.0f, 1.0f, 1.0f);      // #FFFFFF
```

## 🚀 Gelişmiş Özellikler

### AdBlocker Ekleme
```cpp
// AdBlocker'ı etkinleştir
AdBlockerEngine::Enable(true);
AdBlockerEngine::LoadFilterList("https://easylist.to/easylist/easylist.txt");

// Özel kural ekle
AdBlockerEngine::AddCustomRule("||example.com/ads/*");
```

### DevTools Entegrasyonu
```cpp
// Gelişmiş inspector
ComperInspector::Show();
ComperInspector::EnableMemoryProfiler(true);
ComperInspector::EnableNetworkAnalyzer(true);
ComperInspector::StartRealTimeMonitoring();
```

### Media Renderer
```cpp
// PDF ve media desteği
MediaRenderer::EnablePDFRenderer(true);
MediaRenderer::EnableMediaPlayer(true);
MediaRenderer::SetHardwareAcceleration(true);
```

## 🔧 Sık Karşılaşılan Sorunlar

### Build Hataları
```bash
# CMake bulunamadı
# Çözüm: CMake'i PATH'e ekleyin
set PATH=%PATH%;C:\Program Files\CMake\bin

# Visual Studio bulunamadı
# Çözüm: Developer Command Prompt kullanın
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

### Link Hataları
```cmake
# Kütüphane bulunamadı
# Çözüm: Doğru yolları kontrol edin
find_library(COMPER_CORE_LIB NAMES ComperCore PATHS ${COMPER_SDK_ROOT}/Binaries/x64)

# Windows kütüphaneleri eksik
# Çözüm: Windows kütüphanelerini ekleyin
target_link_libraries(MyComperBrowser user32 gdi32 shell32)
```

### Runtime Hataları
```cpp
// SDK başlatılamadı
// Çözüm: SDK Manager'ı kontrol edin
SDKManager::Initialize();
if (!SDKManager::IsSDKValid()) {
    std::cerr << "SDK geçerli değil!" << std::endl;
    return 1;
}
```

## 📊 Başarım Kontrolü

### Memory Kullanımı
```cpp
// Memory profiler
ComperInspector::EnableMemoryProfiler(true);

// Memory kullanımını göster
auto stats = ComperInspector::GetMemoryStats();
std::cout << "Memory: " << (stats.used_memory / 1024 / 1024) << " MB" << std::endl;
```

### Network Performansı
```cpp
// Network analyzer
ComperInspector::EnableNetworkAnalyzer(true);

// Network istatistikleri
auto network_stats = ComperInspector::GetNetworkStats();
std::cout << "Requests: " << network_stats.total_requests << std::endl;
std::cout << "Avg time: " << network_stats.average_response_time << "ms" << std::endl;
```

## 🎯 Sonraki Adımlar

### 1. GUI Tarayıcı
- Windows API ile grafiksel arayüz
- Mouse ve keyboard events
- Window management

### 2. Sekme Yönetimi
- Multi-tab support
- Tab switching
- Tab history

### 3. Bookmark Sistemi
- Bookmark ekleme/silme
- Bookmark organizasyonu
- Import/export

### 4. History Manager
- Geçmiş kaydı
- Arama ve filtreleme
- Privacy settings

## 📚 Ek Kaynaklar

- [Core API Documentation](../API/Core_API.md)
- [SDK API Documentation](../API/SDK_API.md)
- [Examples](../Examples/)
- [GitHub Repository](https://github.com/tda45/comper-engine)

## 🤝 Destek

Sorunlarınız için:
- **GitHub Issues**: [comper-engine/issues](https://github.com/tda45/comper-engine/issues)
- **Documentation**: [docs.comper.engine](https://docs.comper.engine)
- **Community**: [discord.gg/comper](https://discord.gg/comper)

---

**🎉 Tebrikler!** İlk Comper Engine tarayıcınızı başarıyla oluşturdunuz!

**Şimdi yapabilecekleriniz:**
- ✅ Web sayfalarını gezinme
- ✅ HTML parse etme
- ✅ Night Red Ultra teması
- ✅ DevTools kullanma
- ✅ AdBlocker entegrasyonu

**Bir sonraki adım:** [Advanced Browser Tutorial](Advanced_Browser.md)
