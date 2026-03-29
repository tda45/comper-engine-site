@echo off
REM Comper Engine - Tek Tık Derleme Aracı
REM Bu dosya, Comper Engine tabanlı tarayıcınızı kolayca derlemenizi sağlar
REM Developer: Comper Team
REM License: MIT

echo.
echo ========================================
echo    Comper Engine - Derleme Aracı
echo ========================================
echo.

REM Renkleri ayarla
color 0C

REM Başlık
title Comper Builder - Tarayıcı Derleme

echo [BİLGİ] Comper Engine tarayıcınız derleniyor...
echo.

REM Kontrol et: CMake kurulu mu?
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [HATA] CMake bulunamadı!
    echo Lütfen CMake'i https://cmake.org/ adresinden indirip kurun.
    echo.
    pause
    exit /b 1
)

REM Kontrol et: Visual Studio kurulu mu?
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [BİLGİ] Visual Studio Developer Command Prompt aranıyor...
    
    REM Visual Studio 2022
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
        echo [BİLGİ] Visual Studio 2022 Community bulundu.
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
        echo [BİLGİ] Visual Studio 2022 Professional bulundu.
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
        echo [BİLGİ] Visual Studio 2022 Enterprise bulundu.
    ) else (
        echo [HATA] Visual Studio bulunamadı!
        echo Lütfen Visual Studio 2022'yi https://visualstudio.microsoft.com/ adresinden indirin.
        echo.
        pause
        exit /b 1
    )
)

REM Build dizini oluştur
if not exist "build" (
    echo [BİLGİ] Build dizini oluşturuluyor...
    mkdir build
)

cd build

REM CMake yapılandırması
echo [BİLGİ] CMake yapılandırması yapılıyor...
cmake .. -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo [HATA] CMake yapılandırması başarısız!
    cd ..
    pause
    exit /b 1
)

REM Derleme işlemi
echo.
echo [BİLGİ] Derleme başlatılıyor...
echo Bu işlem birkaç dakika sürebilir...
echo.

cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo [HATA] Derleme başarısız!
    cd ..
    pause
    exit /b 1
)

REM Derleme başarılı
echo.
echo ========================================
echo           DERLEME BAŞARILI!
echo ========================================
echo.

REM Çalıştırılabilir dosyanın konumunu kontrol et
if exist "Release\ComperBrowser.exe" (
    echo [BİLGİ] Tarayıcınız hazır: build\Release\ComperBrowser.exe
    echo.
    
    REM Template dosyalarını kopyala
    echo [BİLGİ] Template dosyaları kopyalanıyor...
    if not exist "Release\Template" mkdir "Release\Template"
    xcopy "..\Template\*" "Release\Template\" /E /Y >nul 2>nul
    
    REM Çalıştırılabilir dosyayı ana dizine kopyala
    copy "Release\ComperBrowser.exe" "..\" >nul 2>nul
    copy "Release\Template\*" "..\Template\" /E /Y >nul 2>nul
    
    echo [BİLGİ] Tüm dosyalar hazır!
    echo.
    echo [BAŞARILI] Tarayıcınız çalışmaya hazır: ComperBrowser.exe
    echo.
    
    REM Hemen çalıştırmak ister misiniz?
    set /p choice="Tarayıcıyı şimdi çalıştırmak ister misiniz? (E/H): "
    if /i "%choice%"=="E" (
        echo.
        echo [BİLGİ] Tarayıcı başlatılıyor...
        start "" "..\ComperBrowser.exe"
    )
    
) else (
    echo [HATA] Çalıştırılabilir dosya bulunamadı!
    echo Derleme tamamlanmış olmalı ancak dosya oluşturulamamış.
)

cd ..

echo.
echo ========================================
echo           İŞLEM TAMAMLANDI
echo ========================================
echo.
echo Tarayıcınızı özelleştirmek için:
echo - Template\settings.json dosyasını düzenleyin
echo - Template\ui_elements.cpp dosyasını özelleştirin
echo - Kendi logo ve ikonlarınızı Template klasörüne ekleyin
echo.
echo Yeniden derlemek için bu dosyayı tekrar çalıştırın.
echo.
pause
