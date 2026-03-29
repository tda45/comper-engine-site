@echo off
REM ================================================================
REM Comper Engine SDK 1.0 - Tek Tık Tarayıcı Oluşturucu
REM ================================================================
REM 
REM Bu script, AppInfo.h dosyasındaki ayarlarınıza göre
REM profesyonel bir tarayıcı oluşturur.
REM 
REM Developer: tda45
REM SDK Version: 1.0.0
REM Build Size: ~1GB
REM ================================================================

title Comper Engine SDK - Professional Browser Builder
color 0C

echo.
echo ╔══════════════════════════════════════════════════════════════════╗
echo ║        Comper Engine SDK 1.0 - Professional Browser Builder      ║
echo ║                     ~1GB Development Kit                        ║
echo ╚══════════════════════════════════════════════════════════════════╝
echo.

REM SDK Version kontrolü
echo [SDK] Comper Engine SDK 1.0 Professional başlatılıyor...
echo [SDK] Build Date: %date% %time%
echo.

REM Environment kontrolü
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [HATA] CMake bulunamadı!
    echo Lütfen CMake'i https://cmake.org/ adresinden kurun.
    pause
    exit /b 1
)

REM Visual Studio kontrolü
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [BİLGİ] Visual Studio Developer Command Prompt aranıyor...
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
        echo [SDK] Visual Studio 2022 Community bulundu.
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
        echo [SDK] Visual Studio 2022 Professional bulundu.
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
        echo [SDK] Visual Studio 2022 Enterprise bulundu.
    ) else (
        echo [HATA] Visual Studio 2022 bulunamadı!
        echo Lütfen Visual Studio 2022'yi kurun.
        pause
        exit /b 1
    )
)

REM AppInfo.h kontrolü
if not exist "..\AppInfo.h" (
    echo [HATA] AppInfo.h bulunamadı!
    echo Lütfen Boilerplate klasöründe AppInfo.h dosyasının olduğundan emin olun.
    pause
    exit /b 1
)

echo [SDK] AppInfo.h dosyası okunuyor...
for /f "tokens=2 delims==" %%a in ('findstr /R "^#define BROWSER_NAME" "..\AppInfo.h"') do set BROWSER_NAME=%%a
for /f "tokens=2 delims==" %%a in ('findstr /R "^#define BROWSER_VERSION" "..\AppInfo.h"') do set BROWSER_VERSION=%%a
for /f "tokens=2 delims==" %%a in ('findstr /R "^#define THEME_PRIMARY_COLOR" "..\AppInfo.h"') do set THEME_COLOR=%%a

echo [SDK] Tarayıcı: %BROWSER_NAME% %BROWSER_VERSION%
echo [SDK] Tema: %THEME_COLOR%
echo.

REM Build dizini oluştur
if not exist "build" mkdir build
cd build

REM SDK Asset kütüphanesini kontrol et
echo [SDK] Asset kütüphanesi kontrol ediliyor...
if not exist "..\..\Assets\4K" (
    echo [UYARI] Asset kütüphanesi bulunamadı!
    echo [SDK] Varsayılan assetler oluşturuluyor...
    call "..\..\Scripts\CreateDefaultAssets.bat"
)

REM CMake konfigürasyonu
echo [SDK] CMake konfigürasyonu yapılıyor...
cmake ..\..\ -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCOMPER_SDK_BUILD=ON ^
    -DENABLE_ULTRA_PERFORMANCE=ON ^
    -DENABLE_4K_MODE=ON ^
    -DENABLE_NEON_GLOW=ON

if %ERRORLEVEL% NEQ 0 (
    echo [HATA] CMake konfigürasyonu başarısız!
    cd ..
    pause
    exit /b 1
)

REM Derleme işlemi
echo.
echo [SDK] Derleme başlatılıyor...
echo [SDK] Bu işlem 5-10 dakika sürebildir (1GB SDK derleniyor)...
echo.

cmake --build . --config Release --parallel 8

if %ERRORLEVEL% NEQ 0 (
    echo [HATA] Derleme başarısız!
    cd ..
    pause
    exit /b 1
)

REM Asset paketleme
echo.
echo [SDK] Asset paketleme başlatılıyor...
powershell -Command "Compress-Archive -Path '..\..\Assets\*' -DestinationPath 'Release\Assets.zip' -Force"

if %ERRORLEVEL% NEQ 0 (
    echo [UYARI] Asset paketleme başarısız, manuel olarak devam ediliyor.
)

REM Çalıştırılabilir dosyayı kopyala
echo [SDK] Çalıştırılabilir dosya hazırlanıyor...
copy "Release\ComperBrowser.exe" "%BROWSER_NAME% v%BROWSER_VERSION%.exe" >nul 2>nul

REM Template dosyalarını kopyala
if not exist "Release\Template" mkdir "Release\Template"
xcopy "..\..\Template\*" "Release\Template\" /E /Y >nul 2>nul

REM Lisans dosyasını ekle
copy "..\..\LICENSE" "Release\" >nul 2>nul

REM Installer oluştur (istenenirse)
echo.
set /p create_installer="Installer oluşturulsun mu? (E/H): "
if /i "%create_installer%"=="E" (
    echo [SDK] Installer oluşturuluyor...
    powershell -Command "& '..\..\Scripts\CreateInstaller.ps1' -Name '%BROWSER_NAME%' -Version '%BROWSER_VERSION%' -OutputPath 'Release\%BROWSER_NAME% v%BROWSER_VERSION% Installer.msi'"
)

REM Başarılı mesajı
echo.
echo ╔══════════════════════════════════════════════════════════════════╗
echo ║                     DERLEME BAŞARILI! 🎉                        ║
echo ╚══════════════════════════════════════════════════════════════════╝
echo.
echo [BAŞARILI] Tarayıcınız hazır: %BROWSER_NAME% v%BROWSER_VERSION%.exe
echo [BİLGİ] Boyut: ~1GB (SDK dahil)
echo [BİLGİ] Özellikler: Night Red Ultra, 4K, AdBlocker, DevTools, Media Player
echo.

REM Test etme seçeneği
set /p test_browser="Tarayıcıyı şimdi test etmek ister misiniz? (E/H): "
if /i "%test_browser%"=="E" (
    echo.
    echo [SDK] %BROWSER_NAME% başlatılıyor...
    start "" "%BROWSER_NAME% v%BROWSER_VERSION%.exe"
)

REM Dağıtım paketi oluştur
echo.
echo [SDK] Dağıtım paketi oluşturuluyor...
set "PACKAGE_NAME=%BROWSER_NAME%_v%BROWSER_VERSION%_SDK_Package"
mkdir "%PACKAGE_NAME%"
copy "%BROWSER_NAME% v%BROWSER_VERSION%.exe" "%PACKAGE_NAME%\" >nul 2>nul
xcopy "Release\*" "%PACKAGE_NAME%\" /E /Y >nul 2>nul
copy "..\..\Docs\*" "%PACKAGE_NAME%\Docs\" /E /Y >nul 2>nul

echo [BAŞARILI] Dağıtım paketi hazır: %PACKAGE_NAME%

cd ..

echo.
echo ╔══════════════════════════════════════════════════════════════════╗
echo ║                      İŞLEM TAMAMLANDI ✅                        ║
echo ╚══════════════════════════════════════════════════════════════════╝
echo.
echo Tarayıcınız hazır ve dağıtıma hazır!
echo.
echo 📁 Oluşturulan dosyalar:
echo    • %BROWSER_NAME% v%BROWSER_VERSION%.exe (~50MB)
echo    • %PACKAGE_NAME% (~1GB)
echo.
echo 🚀 Özellikler:
echo    • Night Red Ultra grafik motoru
echo    • 4K ve 120 FPS desteği
echo    • Dahili AdBlocker
echo    • Gelişmiş DevTools
echo    • PDF & Media Renderer
echo    • Comper Scripts desteği
echo    • 1GB Asset kütüphanesi
echo.
echo 📚 Dokümantasyon: Docs/Tutorials/
echo 🌐 Web sitesi: https://github.com/tda45/comper-engine
echo.
echo Teşekkürler! Comper Engine SDK 1.0 Professional kullandığınız için.
echo.
pause
