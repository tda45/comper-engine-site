# Themes Resources - Comper Engine SDK

**Night Red Ultra Tema Sistemi**

## 📁 Klasör Yapısı

```
Resources/Themes/
├── NightRed/                   # Night Red Ultra teması
│   ├── theme.json              # Tema konfigürasyonu
│   ├── colors.json             # Renk paleti
│   ├── styles.css              # CSS stilleri
│   ├── animations.json         # Animasyon ayarları
│   └── assets/                 # Tema varlıkları
│       ├── backgrounds/
│       ├── icons/
│       └── effects/
├── Dark/                       # Koyu tema
│   ├── theme.json
│   ├── colors.json
│   ├── styles.css
│   └── assets/
├── Light/                      # Açık tema
│   ├── theme.json
│   ├── colors.json
│   ├── styles.css
│   └── assets/
├── Blue/                       # Mavi tema
│   ├── theme.json
│   ├── colors.json
│   ├── styles.css
│   └── assets/
├── Green/                      # Yeşil tema
│   ├── theme.json
│   ├── colors.json
│   ├── styles.css
│   └── assets/
├── Custom/                     # Özel temalar
│   ├── corporate_theme/
│   ├── gaming_theme/
│   └── developer_theme/
└── Presets/                    # Hazır ayarlar
    ├── neon_glow.json
    ├── subtle.json
    ├── high_contrast.json
    └── accessibility.json
```

## 🎨 Night Red Ultra Teması

### Tema Yapılandırması
```json
{
  "name": "Night Red Ultra",
  "version": "1.0.0",
  "description": "Kırmızı neon parlamalı gece teması",
  "author": "Comper Team",
  "type": "dark",
  "primary_color": "#E60000",
  "background_color": "#050505",
  "text_color": "#FFFFFF",
  "accent_color": "#FF0033",
  "neon_glow": true,
  "border_effects": true,
  "animations": true
}
```

### Renk Paleti
```json
{
  "colors": {
    "primary": "#E60000",
    "secondary": "#CC0000",
    "accent": "#FF0033",
    "background": "#050505",
    "surface": "#1A1A1A",
    "text": "#FFFFFF",
    "text_secondary": "#CCCCCC",
    "text_disabled": "#666666",
    "border": "#333333",
    "shadow": "#000000",
    "highlight": "#FF0000",
    "success": "#00FF00",
    "warning": "#FFAA00",
    "error": "#FF0000",
    "info": "#0088FF",
    
    "neon_colors": {
      "red": "#E60000",
      "dark_red": "#CC0000",
      "bright_red": "#FF0033",
      "glow_red": "#FF0066",
      "pulse_red": "#FF3333"
    },
    
    "gradients": {
      "primary_gradient": "linear-gradient(135deg, #E60000, #990000)",
      "neon_gradient": "linear-gradient(45deg, #FF0033, #E60000, #CC0000)",
      "surface_gradient": "linear-gradient(180deg, #1A1A1A, #050505)",
      "text_gradient": "linear-gradient(90deg, #FFFFFF, #CCCCCC)"
    },
    
    "shadows": {
      "neon_glow": "0 0 20px rgba(230, 0, 0, 0.8)",
      "neon_pulse": "0 0 40px rgba(255, 0, 51, 1)",
      "soft_shadow": "0 2px 10px rgba(0, 0, 0, 0.5)",
      "hard_shadow": "0 4px 20px rgba(0, 0, 0, 0.8)"
    }
  }
}
```

### CSS Stilleri
```css
/* Night Red Ultra Ana Stilleri */
:root {
  --primary-red: #E60000;
  --dark-bg: #050505;
  --text-white: #FFFFFF;
  --neon-glow: #FF0033;
  --surface-dark: #1A1A1A;
}

/* Ana arka plan */
body {
  background: var(--dark-bg);
  color: var(--text-white);
  font-family: 'ComerSans', sans-serif;
}

/* Neon efektli butonlar */
.neon-button {
  background: linear-gradient(135deg, var(--primary-red), #990000);
  border: 2px solid var(--primary-red);
  color: var(--text-white);
  padding: 12px 24px;
  border-radius: 8px;
  font-weight: bold;
  box-shadow: 0 0 20px rgba(230, 0, 0, 0.8);
  transition: all 0.3s ease;
}

.neon-button:hover {
  background: linear-gradient(135deg, var(--neon-glow), var(--primary-red));
  box-shadow: 0 0 40px rgba(255, 0, 51, 1);
  transform: translateY(-2px);
}

.neon-button:active {
  background: linear-gradient(135deg, #CC0000, var(--primary-red));
  box-shadow: 0 0 10px rgba(204, 0, 0, 0.6);
  transform: translateY(0);
}

/* Neon parlama efektleri */
.neon-glow {
  animation: neonPulse 2s infinite alternate;
}

@keyframes neonPulse {
  from {
    text-shadow: 0 0 10px var(--primary-red), 0 0 20px var(--primary-red);
  }
  to {
    text-shadow: 0 0 20px var(--neon-glow), 0 0 30px var(--neon-glow);
  }
}

/* Kenar animasyonları */
.neon-border {
  border: 2px solid var(--primary-red);
  border-radius: 8px;
  position: relative;
  overflow: hidden;
}

.neon-border::before {
  content: '';
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  background: linear-gradient(45deg, var(--neon-glow), var(--primary-red), var(--neon-glow));
  border-radius: 8px;
  z-index: -1;
  animation: borderRotate 3s linear infinite;
}

@keyframes borderRotate {
  0% {
    transform: rotate(0deg);
  }
  100% {
    transform: rotate(360deg);
  }
}

/* Surface elementler */
.surface {
  background: var(--surface-dark);
  border: 1px solid #333333;
  border-radius: 8px;
  padding: 16px;
  box-shadow: 0 2px 10px rgba(0, 0, 0, 0.5);
}

/* Input alanları */
.neon-input {
  background: var(--surface-dark);
  border: 2px solid #333333;
  color: var(--text-white);
  padding: 12px 16px;
  border-radius: 8px;
  font-size: 16px;
  transition: all 0.3s ease;
}

.neon-input:focus {
  border-color: var(--primary-red);
  box-shadow: 0 0 20px rgba(230, 0, 0, 0.8);
  outline: none;
}

/* Sekmeler */
.neon-tab {
  background: var(--surface-dark);
  border: 1px solid #333333;
  border-bottom: none;
  color: var(--text-secondary);
  padding: 12px 24px;
  border-radius: 8px 8px 0 0;
  cursor: pointer;
  transition: all 0.3s ease;
}

.neon-tab.active {
  background: var(--primary-red);
  color: var(--text-white);
  border-color: var(--primary-red);
  box-shadow: 0 0 20px rgba(230, 0, 0, 0.8);
}

/* Progress bar */
.neon-progress {
  background: var(--surface-dark);
  border: 2px solid #333333;
  border-radius: 12px;
  height: 8px;
  overflow: hidden;
}

.neon-progress-bar {
  background: linear-gradient(90deg, var(--primary-red), var(--neon-glow));
  height: 100%;
  border-radius: 10px;
  animation: progressGlow 2s infinite alternate;
}

@keyframes progressGlow {
  from {
    box-shadow: 0 0 5px rgba(230, 0, 0, 0.8);
  }
  to {
    box-shadow: 0 0 15px rgba(255, 0, 51, 1);
  }
}
```

### Animasyon Ayarları
```json
{
  "animations": {
    "neon_glow": {
      "duration": "2s",
      "easing": "ease-in-out",
      "iteration_count": "infinite",
      "direction": "alternate",
      "keyframes": {
        "0%": {
          "opacity": 0.8,
          "shadow": "0 0 10px rgba(230, 0, 0, 0.8)"
        },
        "100%": {
          "opacity": 1,
          "shadow": "0 0 30px rgba(255, 0, 51, 1)"
        }
      }
    },
    "border_pulse": {
      "duration": "3s",
      "easing": "linear",
      "iteration_count": "infinite",
      "keyframes": {
        "0%": {
          "rotate": "0deg"
        },
        "100%": {
          "rotate": "360deg"
        }
      }
    },
    "button_hover": {
      "duration": "0.3s",
      "easing": "ease-out",
      "keyframes": {
        "0%": {
          "transform": "translateY(0)",
          "shadow": "0 0 20px rgba(230, 0, 0, 0.8)"
        },
        "100%": {
          "transform": "translateY(-2px)",
          "shadow": "0 0 40px rgba(255, 0, 51, 1)"
        }
      }
    }
  }
}
```

## 🔧 Tema Yönetimi

### Tema Yükleme
```cpp
#include "Includes/SDK/comper_sdk.h"

// Tema yükle
void LoadTheme(const std::string& theme_name) {
    std::string theme_path = "Resources/Themes/" + theme_name + "/theme.json";
    
    // Tema konfigürasyonunu yükle
    std::string theme_config = ReadFile(theme_path);
    ParseThemeConfig(theme_config);
    
    // Temayı uygula
    ApplyTheme(theme_name);
    
    std::cout << "[Theme] Tema yüklendi: " << theme_name << std::endl;
}

// Night Red Ultra temasını uygula
void ApplyNightRedTheme() {
    // Renkleri ayarla
    NightRedUltra::SetNeonIntensity(0.8f);
    NightRedUltra::SetGlowColor(230, 0, 0);
    NightRedUltra::EnableNeonGlow(true);
    NightRedUltra::EnableWindowEffects(true);
    NightRedUltra::SetBorderAnimation("pulse");
    
    // CSS stillerini yükle
    LoadCSS("Resources/Themes/NightRed/styles.css");
    
    // Animasyonları yükle
    LoadAnimations("Resources/Themes/NightRed/animations.json");
    
    std::cout << "[Theme] Night Red Ultra teması uygulandı" << std::endl;
}
```

### Dinamik Tema Değiştirme
```cpp
// Tema değiştir
void SwitchTheme(const std::string& new_theme) {
    // Mevcut temayı kaydet
    std::string current_theme = GetCurrentTheme();
    SaveThemeState(current_theme);
    
    // Yeni temayı yükle
    LoadTheme(new_theme);
    
    // UI'ı güncelle
    UpdateUIForTheme(new_theme);
    
    // Animasyonları güncelle
    UpdateAnimationsForTheme(new_theme);
    
    std::cout << "[Theme] Tema değiştirildi: " << current_theme << " -> " << new_theme << std::endl;
}
```

### Tema Özelleştirme
```cpp
// Temayı özelleştir
void CustomizeTheme(const std::string& theme_name, const ThemeSettings& settings) {
    // Tema konfigürasyonunu güncelle
    UpdateThemeConfig(theme_name, settings);
    
    // Renkleri güncelle
    if (settings.primary_color.has_value()) {
        SetPrimaryColor(settings.primary_color.value());
    }
    
    if (settings.neon_intensity.has_value()) {
        NightRedUltra::SetNeonIntensity(settings.neon_intensity.value());
    }
    
    if (settings.border_animation.has_value()) {
        NightRedUltra::SetBorderAnimation(settings.border_animation.value());
    }
    
    // Değişiklikleri uygula
    ApplyThemeChanges(theme_name);
    
    std::cout << "[Theme] Tema özelleştirildi: " << theme_name << std::endl;
}
```

## 🎯 Mevcut Temalar

### Night Red Ultra (Varsayılan)
- **Açıklama**: Kırmızı neon parlamalı gece teması
- **Renkler**: #E60000, #050505, #FFFFFF
- **Özellikler**: Neon glow, kenar animasyonları, 4K desteği

### Dark (Koyu)
- **Açıklama**: Minimalist koyu tema
- **Renkler**: #333333, #000000, #FFFFFF
- **Özellikler**: Yüksek kontrast, göz yorgunluğu azaltma

### Light (Açık)
- **Açıklama**: Parlak ve temiz tema
- **Renkler**: #FFFFFF, #F0F0F0, #000000
- **Özellikler**: Gündüz kullanımı, yüksek okunabilirlik

### Blue (Mavi)
- **Açıklama**: Mavi tonlu profesyonel tema
- **Renkler**: #0066CC, #003366, #FFFFFF
- **Özellikler**: Kurumsal görünüm, sakin renkler

### Green (Yeşil)
- **Açıklama**: Yeşil tonlu modern tema
- **Renkler**: #00CC66, #006633, #FFFFFF
- **Özellikler**: Doğal görünüm, göz dostu

## 📊 Tema İstatistikleri

| Tema | Dosya Sayısı | Toplam Boyut | Özellikler |
|------|--------------|--------------|------------|
| NightRed | 15 | 5MB | Neon glow, animasyonlar |
| Dark | 12 | 3MB | Yüksek kontrast |
| Light | 12 | 3MB | Gündüz modu |
| Blue | 12 | 3MB | Kurumsal |
| Green | 12 | 3MB | Doğal |
| Custom | 20 | 8MB | Özelleştirilebilir |
| **Toplam** | **83** | **25MB** | **Tüm özellikler** |

## 🔍 Tema Geliştirme

### Yeni Tema Oluşturma
1. **Klasör Oluştur**: `Resources/Themes/MyTheme/`
2. **Konfigürasyon**: `theme.json` dosyasını oluştur
3. **Renk Paleti**: `colors.json` dosyasını oluştur
4. **CSS Stilleri**: `styles.css` dosyasını oluştur
5. **Varlıklar**: `assets/` klasörünü oluştur
6. **Kaydet**: Temayı sistemeye kaydet

### Tema Yapısı
```
MyTheme/
├── theme.json              # Tema bilgileri
├── colors.json             # Renk paleti
├── styles.css              # CSS stilleri
├── animations.json         # Animasyonlar
└── assets/                 # Varlıklar
    ├── backgrounds/
    ├── icons/
    └── effects/
```

## 📝 Lisans ve Kullanım

**Lisans:** Comper Engine SDK Commercial License  
**Kullanım:** SDK ile birlikte dağıtılır  
**Değiştirme**: Temalar özelleştirilebilir  
**Dağıtım:** Özel temalar dağıtılabilir

---

**Themes Resources** - Night Red Ultra dahil tüm tema sistemleri, özelleştirilebilir ve genişletilebilir.
