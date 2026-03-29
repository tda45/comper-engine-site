# Fonts Resources - Comper Engine SDK

**Comper Font Ailesi - Night Red Ultra Tipografisi**

## 📁 Klasör Yapısı

```
Resources/Fonts/
├── ComerSans/                   # Comer Sans font ailesi
│   ├── ComerSans-Regular.ttf    # Regular weight
│   ├── ComerSans-Bold.ttf      # Bold weight
│   ├── ComerSans-Light.ttf     # Light weight
│   ├── ComerSans-Italic.ttf    # Italic style
│   ├── ComerSans-BoldItalic.ttf # Bold Italic
│   ├── ComerSans-Black.ttf     # Black weight
│   ├── ComerSans-Thin.ttf      # Thin weight
│   └── ComerSans-Variable.ttf  # Variable font
├── ComerMono/                   # Comer Mono font ailesi
│   ├── ComerMono-Regular.ttf    # Regular monospace
│   ├── ComerMono-Bold.ttf      # Bold monospace
│   ├── ComerMono-Italic.ttf    # Italic monospace
│   └── ComerMono-Variable.ttf  # Variable monospace
├── Display/                     # Display fontları
│   ├── ComerDisplay-Black.ttf   # Display headlines
│   ├── ComerDisplay-Light.ttf   # Light display
│   ├── ComerDisplay-Neon.ttf   # Neon effect font
│   └── ComerDisplay-Glow.ttf   # Glow effect font
├── Icons/                        # İkon fontları
│   ├── ComerIcons-Regular.ttf    # Icon font
│   ├── ComerIcons-Solid.ttf     # Solid icons
│   └── ComerIcons-Brands.ttf    # Brand icons
├── System/                       # Sistem fontları
│   ├── SystemUI-Regular.ttf     # System UI
│   ├── SystemUI-Bold.ttf       # System UI Bold
│   └── SystemUI-Mono.ttf       # System UI Mono
└── Web/                          # Web fontları
    ├── ComerSans-Regular.woff2   # WOFF2 format
    ├── ComerSans-Bold.woff2     # WOFF2 bold
    ├── ComerMono-Regular.woff2   # WOFF2 mono
    └── ComerIcons-Regular.woff2  # WOFF2 icons
```

## 🔤 Comer Sans Font Ailesi

### Font Özellikleri
- **Tasarım**: Modern, teknolojik, okunabilir
- **Stil**: Sans-serif, geometric
- **Karakter Seti**: Latin, Turkish, Extended
- **Boyutlar**: 6 weights, italic, variable
- **Optimizasyon**: Screen ve print için optimize edilmiş

### Weight'ler
- **Thin (100)**: Ultra ince, başlıklar için
- **Light (300)**: İnce, metin için
- **Regular (400)**: Standart, genel kullanım
- **Bold (700)**: Kalın, vurgu için
- **Black (900)**: Ultra kalın, başlıklar için
- **Variable**: 100-900 arası ağırlık

### Karakter Özellikleri
- **Latin Extended**: Türkçe karakterler (ç, ğ, ı, ö, ş, ü)
- **Punctuation**: Zengin noktalama işaretleri
- **Symbols**: Teknolojik ve matematiksel semboller
- **Numbers**: Tabular numbers, lining figures

## 🔤 Comer Mono Font Ailesi

### Font Özellikleri
- **Tasarım**: Monospace, kod için optimize edilmiş
- **Stil**: Sans-serif monospace
- **Karakter Genişliği**: Tutarlı genişlik
- **Özellikler**: Programlama dostu, okunabilir

### Weight'ler
- **Regular (400)**: Standart kod fontu
- **Bold (700)**: Kalın vurgu
- **Italic**: İtalik kod
- **Variable**: 400-700 arası ağırlık

### Kod Özellikleri
- **Ligatures**: Programlama ligatürleri
- **Disambiguation**: Benzer karakterleri ayırma
- **Zero Slashed**: Sıfırı ayırt etme
- **Tabular**: Tabular numaralar

## 🎨 Display Fontları

### Comer Display Black
- **Kullanım**: Başlıklar, posterler
- **Özellikler**: Impact, bold, modern
- **Boyut**: 48pt ve üzeri için optimize

### Comer Display Light
- **Kullanım**: Alt başlıklar, modern metin
- **Özellikler**: Şık, ince, zarif
- **Boyut**: 24pt-48pt arası

### Comer Display Neon
- **Kullanım**: Neon efektli metinler
- **Özellikler**: Glow efektli, teknolojik
- **Boyut**: 36pt ve üzeri

### Comer Display Glow
- **Kullanım**: Parlayan metinler
- **Özellikler**: Soft glow, modern
- **Boyut**: 24pt ve üzeri

## 🔧 Font Yönetimi

### Font Yükleme
```cpp
#include "Includes/SDK/comper_sdk.h"

// Font yükle
void* font = AssetLibrary::GetFont("ComerSans-Regular.ttf");

// Fontu sisteme yükle
bool LoadFont(const std::string& font_path) {
    // Windows API ile font yükle
    HFONT hFont = AddFontResourceEx(font_path.c_str(), FR_PRIVATE, nullptr);
    
    if (hFont) {
        std::cout << "[Font] Font yüklendi: " << font_path << std::endl;
        return true;
    }
    
    return false;
}

// Tüm fontları yükle
void LoadAllFonts() {
    std::vector<std::string> fonts = AssetLibrary::GetAllFonts();
    
    for (const auto& font : fonts) {
        LoadFont("Resources/Fonts/" + font);
    }
    
    std::cout << "[Font] " << fonts.size() << " font yüklendi" << std::endl;
}
```

### Font Uygulama
```cpp
// Fontu UI'a uygula
void ApplyFont(const std::string& font_name, const std::string& element) {
    UIElement* ui_element = GetUIElement(element);
    
    if (font_name == "ComerSans-Regular") {
        ui_element->SetFont("ComerSans", 16, FontStyle::Regular);
    } else if (font_name == "ComerSans-Bold") {
        ui_element->SetFont("ComerSans", 16, FontStyle::Bold);
    } else if (font_name == "ComerMono-Regular") {
        ui_element->SetFont("ComerMono", 14, FontStyle::Regular);
    }
    
    std::cout << "[Font] Font uygulandı: " << font_name << " -> " << element << std::endl;
}

// Night Red Ultra font ayarları
void ApplyNightRedFonts() {
    // Ana metin için Comer Sans
    ApplyFont("ComerSans-Regular", "body");
    ApplyFont("ComerSans-Bold", "headings");
    ApplyFont("ComerSans-Italic", "emphasis");
    
    // Kod için Comer Mono
    ApplyFont("ComerMono-Regular", "code");
    ApplyFont("ComerMono-Bold", "code_bold");
    
    // Başlıklar için Display
    ApplyFont("ComerDisplay-Black", "main_title");
    ApplyFont("ComerDisplay-Light", "subtitle");
    
    std::cout << "[Font] Night Red Ultra fontları uygulandı" << std::endl;
}
```

### Web Fontları
```html
<!-- Web fontları için CSS -->
@font-face {
    font-family: 'ComerSans';
    src: url('Resources/Fonts/Web/ComerSans-Regular.woff2') format('woff2'),
         url('Resources/Fonts/ComerSans-Regular.ttf') format('truetype');
    font-weight: 400;
    font-style: normal;
    font-display: swap;
}

@font-face {
    font-family: 'ComerSans';
    src: url('Resources/Fonts/Web/ComerSans-Bold.woff2') format('woff2'),
         url('Resources/Fonts/ComerSans-Bold.ttf') format('truetype');
    font-weight: 700;
    font-style: normal;
    font-display: swap;
}

@font-face {
    font-family: 'ComerMono';
    src: url('Resources/Fonts/Web/ComerMono-Regular.woff2') format('woff2'),
         url('Resources/Fonts/ComerMono-Regular.ttf') format('truetype');
    font-weight: 400;
    font-style: normal;
    font-display: swap;
}
```

## 📊 Font İstatistikleri

| Font Ailesi | Weight'ler | Dosya Sayısı | Toplam Boyut |
|-------------|------------|--------------|--------------|
| ComerSans | 7 | 8 | 15MB |
| ComerMono | 4 | 5 | 8MB |
| Display | 4 | 4 | 10MB |
| Icons | 3 | 3 | 5MB |
| System | 3 | 3 | 6MB |
| Web | 4 | 4 | 4MB |
| **Toplam** | **25** | **27** | **48MB** |

## 🎯 Font Kullanım Rehberi

### Metin Türüne Göre Font Seçimi
```cpp
// Metin türüne göre font seçimi
enum class TextType {
    BODY_TEXT,        // Ana metin
    HEADING,          // Başlıklar
    CODE,             // Kod
    UI_ELEMENT,       // UI elementleri
    DISPLAY,          // Display metin
    EMPHASIS          // Vurgu
};

std::string GetFontForTextType(TextType type) {
    switch (type) {
        case TextType::BODY_TEXT:
            return "ComerSans-Regular";
        case TextType::HEADING:
            return "ComerSans-Bold";
        case TextType::CODE:
            return "ComerMono-Regular";
        case TextType::UI_ELEMENT:
            return "ComerSans-Regular";
        case TextType::DISPLAY:
            return "ComerDisplay-Black";
        case TextType::EMPHASIS:
            return "ComerSans-Italic";
        default:
            return "ComerSans-Regular";
    }
}
```

### Boyut Optimizasyonu
```cpp
// Boyuta göre font seçimi
std::string GetFontForSize(int size) {
    if (size >= 48) {
        return "ComerDisplay-Black";
    } else if (size >= 36) {
        return "ComerDisplay-Light";
    } else if (size >= 24) {
        return "ComerSans-Bold";
    } else if (size >= 16) {
        return "ComerSans-Regular";
    } else {
        return "ComerSans-Light";
    }
}
```

### Variable Font Kullanımı
```cpp
// Variable font kullanımı
void UseVariableFont(const std::string& text, float weight) {
    // Variable font ile ağırlık ayarı
    std::string css_weight = std::to_string(static_cast<int>(weight));
    
    // CSS olarak uygula
    std::string css_rule = "font-variation-settings: 'wght' " + css_weight + ";";
    ApplyCSSRule(css_rule);
    
    std::cout << "[Font] Variable font ağırlığı: " << weight << std::endl;
}
```

## 🔍 Font Kalitesi

### Teknik Özellikler
- **Format**: TrueType (.ttf), WOFF2 (.woff2)
- **Encoding**: Unicode, UTF-8
- **Hinting**: Screen optimize edilmiş
- **Kerning**: Otomatik kerning
- **Ligatures**: OpenType ligatürleri

### Optimizasyonlar
- **Subset**: Latin Extended subset
- **Compression**: WOFF2 sıkıştırma
- **Caching**: Font cache sistemi
- **Loading**: Lazy loading

### Test Prosedürü
1. Karakter görüntüleme testi
2. Boyut tutarlılığı testi
3. Performans testi
4. Cross-platform testi
5. Türkçe karakter testi

## 📝 Lisans ve Kullanım

**Lisans:** Comper Engine SDK Commercial License  
**Kullanım:** SDK ile birlikte dağıtılır  
**Değiştirme:** Fontlar değiştirilemez  
**Dağıtım:** Web fontları dağıtılabilir

---

**Fonts Resources** - Comper Font ailesi, Night Red Ultra teması için optimize edilmiş, profesyonel tipografi.
