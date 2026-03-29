/*
 * Comper Engine SDK - AppInfo.h
 * 
 * BURASI TEK DÜZENLEMENİZ GEREKEN DOSYA!
 * 
 * Tüm tarayıcı özelliklerinizi buradan ayarlayın.
 * Geri kalan her şeyi Comper Engine SDK halleder.
 * 
 * Değiştirdikten sonra Boilerplate/QuickStart/Build.bat çalıştırın.
 */

#ifndef APP_INFO_H
#define APP_INFO_H

#include <string>

// ========== TARAYICI BİLGİLERİ ==========
// Burayı kendi tarayıcınıza göre düzenleyin
#define BROWSER_NAME "Benim Tarayıcım"
#define BROWSER_VERSION "1.0.0"
#define BROWSER_DESCRIPTION "Comper Engine SDK tabanlı profesyonel tarayıcım"
#define BROWSER_COMPANY "Şirket Adınız"
#define BROWSER_COPYRIGHT "© 2026 Şirket Adınız"

// ========== LOGO VE İKONLAR ==========
// Logo dosyanızın yolu (Assets/4K klasöründe olmalı)
#define BROWSER_LOGO "Assets/4K/logo.png"
#define BROWSER_ICON "Assets/Icons/app_icon.ico"

// ========== TEMA AYARLARI ==========
// Night Red Ultra tema ayarları
#define THEME_PRIMARY_COLOR "#E60000"     // Kırmızı neon
#define THEME_BACKGROUND_COLOR "#050505"  // Gece siyahı
#define THEME_NEON_INTENSITY 0.8f         // Neon parlaklığı
#define THEME_BORDER_ANIMATION "pulse"     // Kenar animasyonu

// ========== PERFORMANS AYARLARI ==========
#define ENABLE_ULTRA_PERFORMANCE true      // Ultra performans modu
#define ENABLE_MULTITHREADING true         // Çoklu thread
#define MAX_FPS 120                        // Maksimum FPS
#define ENABLE_4K_MODE true                // 4K desteği

// ========== ÖZELLİKLER ==========
#define ENABLE_ADBLOCKER true              // Reklam engelleme
#define ENABLE_DEVTOOLS true               // Geliştirici araçları
#define ENABLE_MEDIA_PLAYER true           // Medya oynatıcı
#define ENABLE_PDF_RENDERER true          // PDF görüntüleyici
#define ENABLE_SCRIPTING true              // Comper Scripts

// ========== GÜVENLİK AYARLARI ==========
#define ENABLE_HTTPS_ONLY false            // Sadece HTTPS
#define ENABLE_SANDBOX true               // Script sandbox
#define ENABLE_HARDWARE_ACCELERATION true  // Donanım ivmesi

// ========== AĞ AYARLARI ==========
#define HOMEPAGE_URL "https://www.google.com"
#define SEARCH_ENGINE_URL "https://www.google.com/search?q=%s"
#define NEW_TAB_URL "file:///Templates/Basic/new_tab.html"

// ========== LİSANS VE DAĞITIM ==========
#define LICENSE_TYPE "COMMERCIAL"          // COMMERCIAL, TRIAL, EDUCATIONAL
#define LICENSE_KEY ""                    // Lisans anahtarınızı buraya yazın
#define AUTO_UPDATE_ENABLED true          // Otomatik güncelleme

// ========== GELİŞTİRİCİ AYARLARI ==========
#define DEBUG_MODE false                  // Debug modu
#define VERBOSE_LOGGING false             // Detaylı loglama
#define ENABLE_PROFILER false             // Performans profiler

// ========== PAKETLEME AYARLARI ==========
#define CREATE_INSTALLER true             // Installer oluştur
#define CREATE_PORTABLE_VERSION true      // Portable versiyon oluştur
#define COMPRESS_ASSETS true              // Assetleri sıkıştır

// ========== PLATFORM AYARLARI ==========
#define TARGET_WINDOWS true               // Windows hedefi
#define TARGET_MAC false                  // macOS hedefi
#define TARGET_LINUX false                // Linux hedefi

// ========== CUSTOMIZATIONS ==========
// Buraya özel ayarlarınızı ekleyebilirsiniz
namespace CustomSettings {
    // Özel ayarlarınız
    const std::string custom_font = "Assets/Fonts/CustomFont.ttf";
    const std::string startup_animation = "Assets/Animations/startup.json";
    const std::string custom_splash = "Assets/4K/splash.png";
    
    // Özel menü öğeleri
    const bool enable_custom_menu = true;
    const std::string custom_menu_config = "Templates/Advanced/custom_menu.json";
    
    // Özel eklentiler
    const bool auto_load_plugins = true;
    const std::string plugins_directory = "Plugins/";
}

// ========== OTOMATİK AYARLAR (DOKUNMAYIN) ==========
// Bu ayarlar SDK tarafından otomatik yönetilir

#ifdef _WIN32
    #define PLATFORM_EXTENSION ".exe"
    #define INSTALLER_EXTENSION ".msi"
#else
    #define PLATFORM_EXTENSION ""
    #define INSTALLER_EXTENSION ""
#endif

// SDK versiyon bilgisi
#define SDK_VERSION_MAJOR 1
#define SDK_VERSION_MINOR 0
#define SDK_VERSION_PATCH 0

// Build bilgileri
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define COMPILER_VERSION "MSVC 2022"

// Asset yolları
#define ASSETS_ROOT "Assets/"
#define THEMES_ROOT "Resources/Themes/"
#define ICONS_ROOT "Resources/Icons/"
#define FONTS_ROOT "Resources/Fonts/"

// ThirdParty kütüphane yolları
#define SKIA_ROOT "ThirdParty/Skia/"
#define V8_ROOT "ThirdParty/v8/"
#define OPENSSL_ROOT "ThirdParty/OpenSSL/"
#define FFMPEG_ROOT "ThirdParty/FFmpeg/"

#endif // APP_INFO_H

/*
 * ========== KULLANIM KILAVUZU ==========
 * 
 * 1. Yukarıdaki ayarları düzenleyin
 * 2. Logo ve ikonlarınızı Assets klasörüne koyun
 * 3. Boilerplate/QuickStart/Build.bat çalıştırın
 * 4. Tarayıcınız hazır! 🎉
 * 
 * Not: Değişikliklerin görmek için projeyi yeniden derlemeniz gerekir.
 * 
 * ========== DESTEK ==========
 * 
 * Dokümantasyon: Docs/Tutorials/
 * API Referansı: Docs/API/
 * Örnekler: Docs/Examples/
 * 
 * Comper Engine SDK 1.0 - Professional Browser Development Kit
 * https://github.com/tda45/comper-engine
 */
