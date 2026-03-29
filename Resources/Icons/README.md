# Icons Resources - Comper Engine SDK

**Night Red Ultra İkon Kütüphanesi**

## 📁 Klasör Yapısı

```
Resources/Icons/
├── Navigation/                  # Navigasyon ikonları
│   ├── Arrows/                 # Ok ikonları
│   │   ├── arrow_left.svg
│   │   ├── arrow_right.svg
│   │   ├── arrow_up.svg
│   │   ├── arrow_down.svg
│   │   └── arrow_refresh.svg
│   ├── Controls/               # Kontrol ikonları
│   │   ├── play.svg
│   │   ├── pause.svg
│   │   ├── stop.svg
│   │   └── record.svg
│   └── Actions/                # Aksiyon ikonları
│       ├── download.svg
│       ├── upload.svg
│       ├── settings.svg
│       └── close.svg
├── Browser/                    # Tarayıcı ikonları
│   ├── Tabs/                   # Sekme ikonları
│   │   ├── tab_new.svg
│   │   ├── tab_close.svg
│   │   └── tab_duplicate.svg
│   ├── Navigation/             # Navigasyon ikonları
│   │   ├── home.svg
│   │   ├── back.svg
│   │   ├── forward.svg
│   │   └── refresh.svg
│   └── Features/               # Özellik ikonları
│       ├── bookmarks.svg
│       ├── history.svg
│       ├── downloads.svg
│       └── extensions.svg
├── Social/                     # Sosyal medya ikonları
│   ├── Platforms/              # Platform ikonları
│   │   ├── youtube.svg
│   │   ├── twitter.svg
│   │   ├── facebook.svg
│   │   ├── instagram.svg
│   │   └── linkedin.svg
│   ├── Communication/          # İletişim ikonları
│   │   ├── email.svg
│   │   ├── chat.svg
│   │   ├── phone.svg
│   │   └── video.svg
│   └── File Types/             # Dosya tipi ikonları
│       ├── pdf.svg
│       ├── image.svg
│       ├── video.svg
│       └── audio.svg
├── System/                     # Sistem ikonları
│   ├── Status/                 # Durum ikonları
│   │   ├── online.svg
│   │   ├── offline.svg
│   │   ├── busy.svg
│   │   └── away.svg
│   ├── Security/               # Güvenlik ikonları
│   │   ├── lock.svg
│   │   ├── shield.svg
│   │   ├── warning.svg
│   │   └── error.svg
│   └── Device/                 # Cihaz ikonları
│       ├── desktop.svg
│       ├── mobile.svg
│       ├── tablet.svg
│       └── server.svg
├── Development/                # Geliştirme ikonları
│   ├── Code/                   # Kod ikonları
│   │   ├── html.svg
│   │   ├── css.svg
│   │   ├── javascript.svg
│   │   └── json.svg
│   ├── Tools/                  # Araç ikonları
│   │   ├── terminal.svg
│   │   ├── debugger.svg
│   │   ├── git.svg
│   │   └── api.svg
│   └── Actions/                # Geliştirme aksiyonları
│       ├── run.svg
│       ├── debug.svg
│       ├── build.svg
│       └── deploy.svg
├── Branding/                   # Marka ikonları
│   ├── Comper/                 # Comper markası
│   │   ├── comper_logo.svg
│   │   ├── comper_icon.svg
│   │   ├── comper_symbol.svg
│   │   └── comper_text.svg
│   ├── NightRed/               # Night Red teması
│   │   ├── night_red_logo.svg
│   │   ├── night_red_icon.svg
│   │   └── night_red_symbol.svg
│   └── Custom/                 # Özel markalar
│       ├── custom_logo.svg
│       └── custom_icon.svg
└── Formats/                    # Format ikonları
    ├── Vectors/                # Vektör formatlar
    │   ├── svg/
    │   ├── ai/
    │   ├── eps/
    │   └── pdf/
    ├── Images/                 # Resim formatları
    │   ├── png/
    │   ├── jpg/
    │   ├── gif/
    │   └── webp/
    └── Icons/                  # İkon formatları
        ├── ico/
        ├── icns/
        └── png/
```

## 🎨 Night Red Ultra İkon Stili

### Tasarım İlkeleri
- **Renk Paleti**: #E60000 (kırmızı), #050505 (siyah), #FFFFFF (beyaz)
- **Stil**: Minimalist, modern, teknolojik
- **Boyutlar**: 16px, 24px, 32px, 48px, 64px, 128px, 256px
- **Format**: SVG (vektör), PNG (raster)
- **Tema**: Night Red Ultra

### İkon Özellikleri
- **Vektör Tabanlı**: SVG formatında, ölçeklenebilir
- **High DPI**: 4K ve retina ekranlar için optimize
- **Tutarlı Stil**: Tüm ikonlarda aynı tasarım dili
- **Neon Efekt**: Kırmızı neon parlaması
- **Optimize Edilmiş**: Küçük dosya boyutu

## 🔧 Kullanım Rehberi

### İkon Yükleme
```cpp
#include "Includes/SDK/comper_sdk.h"

// SVG ikon yükle
std::string icon_svg = AssetLibrary::GetSVGIcon("home.svg");

// İkon listesi al
std::vector<std::string> all_icons = AssetLibrary::GetAllSVGIcons();

// Belirli bir ikonu bul
std::string navigation_icon = AssetLibrary::GetSVGIcon("arrow_left.svg");
```

### İkon Kullanımı
```cpp
// UI'a ikon ekle
void AddIconToUI(const std::string& icon_name, const std::string& position) {
    std::string icon_svg = AssetLibrary::GetSVGIcon(icon_name);
    
    // İkonu UI element'ine ekle
    UIElement* element = GetUIElement(position);
    element->SetIcon(icon_svg);
    
    // Night Red temasını uygula
    element->ApplyTheme("night_red");
}
```

### Dinamik İkon Değiştirme
```cpp
// Temaya göre ikon değiştir
void UpdateIconsForTheme(const std::string& theme) {
    if (theme == "night_red") {
        // Night Red ikonlarını uygula
        UpdateIcon("home", "home_night_red.svg");
        UpdateIcon("back", "back_night_red.svg");
        UpdateIcon("forward", "forward_night_red.svg");
    }
}
```

## 📊 İkon İstatistikleri

| Kategori | İkon Sayısı | Format | Boyut |
|----------|-------------|--------|-------|
| Navigation | 25 | SVG | 2MB |
| Browser | 30 | SVG | 3MB |
| Social | 20 | SVG | 1.5MB |
| System | 35 | SVG | 4MB |
| Development | 25 | SVG | 2.5MB |
| Branding | 15 | SVG | 1MB |
| Formats | 40 | SVG/PNG | 6MB |
| **Toplam** | **190** | **Mixed** | **20MB** |

## 🎯 İkon Kategorileri

### Navigation İkonları
**Özellikler:**
- Oklar (sol, sağ, yukarı, aşağı)
- Yenileme
- Kontroller (oynat, durdur, kaydet)
- Aksiyonlar (indir, yükle, ayarlar)

**Dosyalar:**
- `arrow_left.svg` - Sol ok
- `arrow_right.svg` - Sağ ok
- `arrow_up.svg` - Yukarı ok
- `arrow_down.svg` - Aşağı ok
- `arrow_refresh.svg` - Yenileme oku

### Browser İkonları
**Sekmeler:**
- Yeni sekme
- Sekmeyi kapat
- Sekmeyi çoğalt

**Navigasyon:**
- Ana sayfa
- Geri
- İleri
- Yenile

**Özellikler:**
- Yer imleri
- Geçmiş
- İndirmeler
- Eklentiler

### Social İkonları
**Platformlar:**
- YouTube
- Twitter/X
- Facebook
- Instagram
- LinkedIn

**İletişim:**
- E-posta
- Sohbet
- Telefon
- Video

### System İkonları
**Durum:**
- Çevrimiçi
- Çevrimdışı
- Meşgul
- Uzakta

**Güvenlik:**
- Kilit
- Kalkan
- Uyarı
- Hata

## 🔍 İkon Kalitesi

### Teknik Özellikler
- **Vektör**: SVG formatı, sonsuz ölçeklenebilir
- **Renk**: RGB, 16.7 milyon renk
- **Katmanlar**: Organize edilmiş katman yapısı
- **Optimizasyon**: Optimize edilmiş path'ler

### Boyut Standartları
- **16px**: Küçük UI elementleri
- **24px**: Standart butonlar
- **32px**: Büyük butonlar
- **48px**: Tool bar ikonları
- **64px**: Dashboard ikonları
- **128px**: Uygulama ikonları
- **256px**: Yüksek çözünürlüklü ikonlar

## 🎨 Tema Entegrasyonu

### Night Red Teması
```css
/* Night Red ikon stilleri */
.icon-night-red {
    fill: #E60000;
    filter: drop-shadow(0 0 5px rgba(230, 0, 0, 0.8));
}

.icon-night-red:hover {
    fill: #FF0033;
    filter: drop-shadow(0 0 10px rgba(255, 0, 51, 1));
}

.icon-night-red:active {
    fill: #CC0000;
    filter: drop-shadow(0 0 3px rgba(204, 0, 0, 0.6));
}
```

### Dinamik Tema Değiştirme
```cpp
// İkon rengini temaya göre ayarla
void UpdateIconColors(const std::string& theme) {
    if (theme == "night_red") {
        SetIconColor("#E60000");
        SetIconGlow(true);
        SetIconShadow("0 0 5px rgba(230, 0, 0, 0.8)");
    }
}
```

## 📝 Lisans ve Kullanım

**Lisans:** Comper Engine SDK Commercial License  
**Kullanım:** SDK ile birlikte dağıtılır  
**Değiştirme:** Temel ikonlar değiştirilemez  
**Özelleştirme:** Marka ikonları özelleştirilebilir

---

**Icons Resources** - Night Red Ultra temasının tüm ikon kütüphanesi, vektör tabanlı ve optimize edilmiş.
