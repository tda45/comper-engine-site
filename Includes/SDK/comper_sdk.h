#ifndef COMPER_SDK_H
#define COMPER_SDK_H

/*
 * Comper Engine SDK 1.0 - Professional Browser Development Kit
 * Dünyanın en gelişmiş tarayıcı geliştirme kiti
 * 
 * Bu SDK, 1 GB'lık devasa bir paketle profesyonel tarayıcı geliştirme imkanı sunar.
 * Geliştiriciler sadece AppInfo.h dosyasını düzenleyerek kendi tarayıcılarını oluşturabilir.
 * 
 * Developer: tda45
 * License: Commercial SDK License
 * Version: 1.0.0
 * Size: ~1GB
 */

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

#ifdef _WIN32
    #ifdef COMPER_SDK_EXPORT
        #define COMPER_API __declspec(dllexport)
    #else
        #define COMPER_API __declspec(dllimport)
    #endif
#else
    #define COMPER_API __attribute__((visibility("default")))
#endif

namespace ComperSDK {

// SDK Version bilgisi
struct SDKVersion {
    int major = 1;
    int minor = 0;
    int patch = 0;
    const char* build = "Professional";
    const char* codename = "NightRed Ultra";
};

// Pre-built Core bileşenleri
class COMPER_API ComperCore {
public:
    // Motor başlatma
    static bool Initialize();
    static void Shutdown();
    
    // SDK Manager entegrasyonu
    static bool LoadSDK(const std::string& sdk_path);
    static void UnloadSDK();
    
    // Performans optimizasyonlu çekirdek
    static void SetPerformanceMode(bool ultra_performance);
    static void EnableMultithreading(bool enable);
    
private:
    static bool sdk_loaded;
    static bool performance_mode;
};

// Night Red Ultra Grafik Motoru
class COMPER_API NightRedUltra {
public:
    // Shader destekli neon parlamalar
    static void EnableNeonGlow(bool enable);
    static void SetNeonIntensity(float intensity);
    static void SetGlowColor(float r, float g, float b);
    
    // Pencere kenar efektleri
    static void EnableWindowEffects(bool enable);
    static void SetBorderAnimation(const std::string& animation_type);
    
    // 4K ve yüksek FPS desteği
    static void Set4KMode(bool enable);
    static void SetMaxFPS(int fps);
    
    // Özel Comper fontları
    static void LoadComperFonts();
    static void SetFontFamily(const std::string& font_name);
};

// Comper Inspector - Gelişmiş DevTools
class COMPER_API ComperInspector {
public:
    // Chrome'dan daha detaylı inspector
    static void Show();
    static void Hide();
    static bool IsVisible();
    
    // Gelişmiş debugging özellikleri
    static void EnableMemoryProfiler(bool enable);
    static void EnableNetworkAnalyzer(bool enable);
    static void EnablePerformanceProfiler(bool enable);
    
    // Real-time monitoring
    static void StartRealTimeMonitoring();
    static void StopRealTimeMonitoring();
    
    // Element inspection
    static void InspectElement(void* element_handle);
    static void HighlightElement(void* element_handle, const std::string& color);
};

// AdBlocker Engine
class COMPER_API AdBlockerEngine {
public:
    // Dahili reklam engelleme
    static void Enable(bool enable);
    static bool IsEnabled();
    
    // Filtre yönetimi
    static void LoadFilterList(const std::string& filter_url);
    static void AddCustomRule(const std::string& rule);
    static void RemoveRule(const std::string& rule_id);
    
    // İstatistikler
    static int GetBlockedAdsCount();
    static long long GetSavedBandwidth();
    
    // Whitelist/Blacklist
    static void AddToWhitelist(const std::string& domain);
    static void AddToBlacklist(const std::string& domain);
};

// PDF & Media Renderer
class COMPER_API MediaRenderer {
public:
    // PDF rendering
    static void EnablePDFRenderer(bool enable);
    static bool CanRenderPDF(const std::string& file_path);
    static void RenderPDF(const std::string& file_path, void* render_context);
    
    // Media oynatıcı
    static void EnableMediaPlayer(bool enable);
    static bool CanPlayMedia(const std::string& media_url);
    static void PlayMedia(const std::string& media_url, void* player_context);
    
    // Codec desteği
    static void Enable4KVideo(bool enable);
    static void EnableHDR(bool enable);
    static void SetHardwareAcceleration(bool enable);
    
    // Streaming
    static void EnableStreaming(bool enable);
    static void SetStreamingQuality(const std::string& quality);
};

// Comper Scripts - Özel Scripting Katmanı
class COMPER_API ComperScripts {
public:
    // Script motoru
    static void InitializeEngine();
    static void ShutdownEngine();
    
    // Script çalıştırma
    static bool ExecuteScript(const std::string& script_code);
    static bool ExecuteScriptFile(const std::string& file_path);
    
    // Eklenti sistemi
    static void LoadPlugin(const std::string& plugin_path);
    static void UnloadPlugin(const std::string& plugin_id);
    static std::vector<std::string> GetLoadedPlugins();
    
    // API exposure
    static void RegisterAPI(const std::string& api_name, void* api_function);
    static void* GetAPI(const std::string& api_name);
    
    // Sandbox
    static void EnableSandbox(bool enable);
    static void SetSandboxPermissions(const std::vector<std::string>& permissions);
};

// SDK Manager - Merkezi yönetim
class COMPER_API SDKManager {
public:
    // SDK yükleme ve yönetim
    static bool Initialize(const std::string& sdk_path);
    static void Shutdown();
    
    // Asset library yönetimi
    static void LoadAssetLibrary();
    static void* GetAsset(const std::string& asset_path);
    static std::vector<std::string> GetAvailableAssets();
    
    // Theme yönetimi
    static void LoadTheme(const std::string& theme_name);
    static void ApplyTheme();
    static std::vector<std::string> GetAvailableThemes();
    
    // Update sistemi
    static bool CheckForUpdates();
    static bool InstallUpdate(const std::string& update_url);
    
    // Lisans yönetimi
    static bool ValidateLicense(const std::string& license_key);
    static std::string GetLicenseInfo();
};

// Bootloader - Hızlı başlatma
class COMPER_API Bootloader {
public:
    // Ultra hızlı başlatma
    static void QuickStart();
    static void ColdStart();
    static void WarmStart();
    
    // Pre-loading
    static void PreloadCoreComponents();
    static void PreloadAssets();
    static void PreloadThemes();
    
    // Başlatma optimizasyonu
    static void OptimizeStartup();
    static float GetStartupTime();
    static void SetStartupProfile(const std::string& profile_name);
};

// Asset Library - Devasa kaynak kütüphanesi
class COMPER_API AssetLibrary {
public:
    // 4K grafikler
    static void* Get4KBackground(const std::string& background_name);
    static void* Get4KIcon(const std::string& icon_name);
    
    // SVG ikonlar
    static std::string GetSVGIcon(const std::string& icon_name);
    static std::vector<std::string> GetAllSVGIcons();
    
    // Özel Comper fontları
    static void* GetFont(const std::string& font_name);
    static std::vector<std::string> GetAllFonts();
    
    // Animasyonlar
    static void* GetAnimation(const std::string& animation_name);
    static void PlayAnimation(const std::string& animation_name, void* target);
    
    // Asset yönetimi
    static void LoadAssetPack(const std::string& pack_path);
    static void UnloadAssetPack(const std::string& pack_name);
    static long long GetAssetSize(const std::string& asset_path);
};

// Kolaylaştırılmış API - Tek satırda tarayıcı
namespace EasyAPI {

    // Tek fonksiyonla tarayıcı oluştur
    COMPER_API bool CreateBrowser(const std::string& app_name, 
                                 const std::string& version, 
                                 const std::string& logo_path);
    
    // Hızlı konfigürasyon
    COMPER_API void SetTheme(const std::string& theme_name);
    COMPER_API void SetHomepage(const std::string& url);
    COMPER_API void SetSearchEngine(const std::string& engine_url);
    
    // Özellik aç/kapat
    COMPER_API void EnableAdBlocker(bool enable);
    COMPER_API void EnableDevTools(bool enable);
    COMPER_API void EnableMediaPlayer(bool enable);
    
    // Derleme ve dağıtım
    COMPER_API bool BuildBrowser();
    COMPER_API bool PackageBrowser(const std::string& output_path);
    COMPER_API bool CreateInstaller(const std::string& installer_path);
}

} // namespace ComperSDK

// Legacy compatibility - Eski API ile uyum
namespace Comper = ComperSDK;

#endif // COMPER_SDK_H
