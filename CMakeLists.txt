# Comper Engine - Tarayıcı Motoru İskeleti
# CMake yapılandırma dosyası
# Developer: Comper Team
# License: MIT

cmake_minimum_required(VERSION 3.16)

# Proje bilgileri
project(ComperEngine 
    VERSION 1.0.0 
    DESCRIPTION "Comper Engine - Açık Kaynak Tarayıcı Motoru"
    LANGUAGES CXX
)

# C++ standardı
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Derleme ayarları
if(MSVC)
    # Visual Studio için ayarlar
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # GCC/Clang için ayarlar
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Debug modu için ayarlar
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
    if(NOT MSVC)
        add_compile_options(-g -O0)
    endif()
endif()

# Release modu için ayarlar
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(NOT MSVC)
        add_compile_options(-O3)
    endif()
endif()

# Gerekli kütüphaneleri bul
find_package(CURL REQUIRED)

# Include dizinleri
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)
include_directories(${CURL_INCLUDE_DIRS})

# Core modülü için kaynak dosyalar
set(CORE_SOURCES
    Core/network_manager.cpp
    Core/html_parser.cpp
    Core/renderer.cpp
    Core/devtools_inspector.cpp
    Core/comper_engine.cpp
)

# Core statik kütüphanesi oluştur
add_library(ComperCore STATIC ${CORE_SOURCES})

# Core kütüphanesi için link ayarları
target_link_libraries(ComperCore ${CURL_LIBRARIES})

# Platform'a özel kütüphaneler
if(WIN32)
    # Windows için kütüphaneler
    target_link_libraries(ComperCore 
        ws2_32 
        wsock32 
        gdi32 
        user32 
        kernel32
    )
endif()

# Ana uygulama hedefi
add_executable(ComperBrowser Template/ui_elements.cpp)

# Ana uygulama için link ayarları
target_link_libraries(ComperBrowser ComperCore)

# Template kaynaklarını kopyala (isteğe bağlı)
# Bu, kullanıcıların kendi dosyalarını derlemesini sağlar
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Template/settings.json")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Template/settings.json"
        "${CMAKE_CURRENT_BINARY_DIR}/settings.json"
        COPYONLY
    )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Template/new_tab.html")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/Template/new_tab.html"
        "${CMAKE_CURRENT_BINARY_DIR}/new_tab.html"
        COPYONLY
    )
endif()

# Kurulum ayarları
install(TARGETS ComperBrowser
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY Includes/
    DESTINATION include/ComperEngine
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY Template/
    DESTINATION share/ComperEngine/Template
    FILES_MATCHING PATTERN "*.html" PATTERN "*.json" PATTERN "*.cpp"
)

# Test hedefi (isteğe bağlı)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Dokümantasyon (isteğe bağlı)
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()

# Paketleme ayarları
set(CPACK_PACKAGE_NAME "ComperEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Comper Team")
set(CPACK_PACKAGE_CONTACT "info@comper.engine")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)

# Bilgi mesajları
message(STATUS "Comper Engine ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "CURL found: ${CURL_FOUND}")
message(STATUS "CURL version: ${CURL_VERSION}")

# Kullanıcı için kolay derleme komutları
message(STATUS "")
message(STATUS "=== KOLAY DERLEME KOMUTLARI ===")
message(STATUS "")
message(STATUS "Windows için:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake .. -G \"Visual Studio 16 2019\"")
message(STATUS "  cmake --build . --config Release")
message(STATUS "")
message(STATUS "Linux/macOS için:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  make -j$(nproc)")
message(STATUS "")
message(STATUS "Veya ComperBuilder.bat kullanın!")
message(STATUS "")
