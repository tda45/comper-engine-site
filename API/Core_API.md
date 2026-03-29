# Comper Engine Core API Documentation

**Core Module API Reference - Version 1.0**

## 📋 Overview

Comper Engine Core API, tarayıcı motorunun temel işlevselliğini sağlayan C++ API'sidir. Ağ istekleri, HTML işleme, rendering ve temel tarayıcı fonksiyonlarını içerir.

## 🔧 Installation

```cpp
#include "Includes/Core/comper_core.h"
#include "Includes/Core/comper_network.h"
#include "Includes/Core/comper_render.h"
#include "Includes/Core/comper_html.h"

// Link with ComperCore library
#pragma comment(lib, "ComperCore.lib")
```

## 🌐 Network API

### NetworkManager Class

#### Constructor
```cpp
NetworkManager::NetworkManager();
```

#### Methods

##### Initialize
```cpp
bool NetworkManager::Initialize();
```
**Description:** Network manager'ı başlatır.
**Returns:** Başarı durumunu döndürür (true/false).

##### Get Request
```cpp
NetworkResponse NetworkManager::Get(const std::string& url);
```
**Parameters:**
- `url` - İstek yapılacak URL
**Returns:** NetworkResponse nesnesi

**Example:**
```cpp
NetworkManager network;
network.Initialize();

NetworkResponse response = network.Get("https://example.com");
if (response.IsSuccess()) {
    std::cout << "Content: " << response.body << std::endl;
}
```

##### Post Request
```cpp
NetworkResponse NetworkManager::Post(const std::string& url, const std::string& body);
```
**Parameters:**
- `url` - İstek yapılacak URL
- `body` - POST verisi
**Returns:** NetworkResponse nesnesi

##### Async Request
```cpp
void NetworkManager::AsyncRequest(const NetworkRequest& request, 
                               std::function<void(const NetworkResponse&)> callback);
```
**Parameters:**
- `request` - NetworkRequest nesnesi
- `callback` - Callback fonksiyonu

**Example:**
```cpp
NetworkRequest request("https://api.example.com/data");
request.method = "POST";
request.body = "{\"key\":\"value\"}";

network.AsyncRequest(request, [](const NetworkResponse& response) {
    if (response.IsSuccess()) {
        std::cout << "Async request completed!" << std::endl;
    }
});
```

#### Configuration Methods

##### Set User Agent
```cpp
void NetworkManager::SetUserAgent(const std::string& user_agent);
```

##### Set Timeout
```cpp
void NetworkManager::SetTimeout(int seconds);
```

##### Set Proxy
```cpp
void NetworkManager::SetProxy(const std::string& proxy);
```

##### Enable SSL Verification
```cpp
void NetworkManager::SetSSLVerification(bool verify);
```

### NetworkRequest Structure

```cpp
struct NetworkRequest {
    std::string url;                           // Request URL
    std::string method;                         // HTTP method (GET, POST, etc.)
    std::map<std::string, std::string> headers; // HTTP headers
    std::string body;                           // Request body
    std::map<std::string, std::string> cookies; // Cookies
    int timeout;                                // Timeout in seconds
    bool follow_redirects;                      // Follow redirects
    bool verify_ssl;                            // SSL verification
    std::string proxy;                          // Proxy server
    std::string user_agent;                     // User agent string
};
```

### NetworkResponse Structure

```cpp
struct NetworkResponse {
    int status_code;                            // HTTP status code
    std::map<std::string, std::string> headers; // Response headers
    std::string body;                           // Response body
    std::map<std::string, std::string> cookies; // Response cookies
    std::string error_message;                   // Error message
    double response_time;                        // Response time in seconds
    size_t content_length;                       // Content length
    std::string content_type;                    // Content type
    std::string charset;                         // Character set
    bool is_cached;                             // Is from cache
    
    bool IsSuccess() const;                     // Check if successful
    bool IsRedirect() const;                    // Check if redirect
    bool IsClientError() const;                 // Check if client error
    bool IsServerError() const;                 // Check if server error
};
```

## 🎨 Render API

### Renderer Class

#### Constructor
```cpp
Renderer::Renderer();
```

#### Initialization

##### Initialize
```cpp
bool Renderer::Initialize(void* window_handle);
```
**Parameters:**
- `window_handle` - Pencere handle'ı (HWND)
**Returns:** Başarı durumu

**Example:**
```cpp
Renderer renderer;
if (renderer.Initialize(GetActiveWindow())) {
    std::cout << "Renderer initialized successfully!" << std::endl;
}
```

##### Shutdown
```cpp
void Renderer::Shutdown();
```

#### Frame Management

##### Begin Frame
```cpp
void Renderer::BeginFrame();
```

##### End Frame
```cpp
void Renderer::EndFrame();
```

##### Present
```cpp
void Renderer::Present();
```

#### Drawing Methods

##### Clear Screen
```cpp
void Renderer::Clear(const Color& color = Color());
```
**Parameters:**
- `color` - Temizleme rengi

##### Draw Rectangle
```cpp
void Renderer::DrawRectangle(const Rectangle& rect, const Color& color);
void Renderer::DrawFilledRectangle(const Rectangle& rect, const Color& color);
```
**Parameters:**
- `rect` - Dikdörtgen koordinatları
- `color` - Çizim rengi

**Example:**
```cpp
// Night Red Ultra arka plan
Color night_red(0.02f, 0.02f, 0.02f, 1.0f); // #050505
Color neon_red(0.9f, 0.0f, 0.0f, 1.0f);      // #E60000

renderer.Clear(night_red);
renderer.DrawFilledRectangle(Rectangle(100, 100, 200, 50), neon_red);
```

##### Draw Texture
```cpp
void Renderer::DrawTexture(std::shared_ptr<Texture> texture, 
                         const Rectangle& dest_rect, 
                         const Rectangle& src_rect = Rectangle());
```

##### Draw Text
```cpp
void Renderer::DrawText(const std::string& text, 
                      const Point& position, 
                      const Color& color, 
                      std::shared_ptr<Font> font = nullptr);
```

##### Draw Line
```cpp
void Renderer::DrawLine(const Point& start, const Point& end, 
                      const Color& color, float thickness = 1.0f);
```

#### Resource Management

##### Load Texture
```cpp
std::shared_ptr<Texture> Renderer::LoadTexture(const std::string& filename);
```

##### Load Font
```cpp
std::shared_ptr<Font> Renderer::LoadFont(const std::string& filename);
```

##### Load Shader
```cpp
std::shared_ptr<Shader> Renderer::LoadShader(const std::string& vertex_file, 
                                           const std::string& fragment_file);
```

### Color Structure

```cpp
struct Color {
    float r, g, b, a;                           // RGBA values (0.0-1.0)
    
    Color();                                      // Default: black
    Color(float red, float green, float blue, float alpha = 1.0f);
    Color(uint32_t hex);                          // From hex color
    
    uint32_t ToHex() const;                      // To hex color
    static Color FromHex(const std::string& hex);  // From hex string
    static Color FromHSV(float h, float s, float v); // From HSV
};
```

### Rectangle Structure

```cpp
struct Rectangle {
    float x, y, width, height;
    
    Rectangle();
    Rectangle(float x_pos, float y_pos, float w, float h);
    
    bool Contains(float px, float py) const;      // Point in rectangle
    bool Intersects(const Rectangle& other) const; // Rectangle intersection
};
```

### Point Structure

```cpp
struct Point {
    float x, y;
    
    Point();
    Point(float x_pos, float y_pos);
    
    Point operator+(const Point& other) const;
    Point operator-(const Point& other) const;
    float Distance(const Point& other) const;      // Distance between points
};
```

## 📄 HTML API

### HTMLParser Class

#### Constructor
```cpp
HTMLParser::HTMLParser();
```

#### Parsing Methods

##### Parse HTML String
```cpp
std::shared_ptr<DOMDocument> HTMLParser::Parse(const std::string& html);
```
**Parameters:**
- `html` - HTML string'i
**Returns:** DOMDocument pointer'ı

**Example:**
```cpp
HTMLParser parser;
std::string html = R"(
    <html>
        <head><title>Comper Engine</title></head>
        <body>
            <div id="main" class="container">
                <h1>Hello World</h1>
                <p>Welcome to Comper Engine!</p>
            </div>
        </body>
    </html>
)";

std::shared_ptr<DOMDocument> document = parser.Parse(html);
```

##### Parse HTML File
```cpp
std::shared_ptr<DOMDocument> HTMLParser::ParseFile(const std::string& filename);
```

#### Error Handling

##### Has Errors
```cpp
bool HTMLParser::HasErrors() const;
```

##### Get Errors
```cpp
std::vector<std::string> HTMLParser::GetErrors() const;
```

### DOMDocument Class

#### Element Selection

##### Get Element by ID
```cpp
std::shared_ptr<DOMElement> DOMDocument::GetElementById(const std::string& id);
```

##### Get Elements by Tag Name
```cpp
std::vector<std::shared_ptr<DOMElement>> DOMDocument::GetElementsByTagName(const std::string& tag_name);
```

##### Get Elements by Class Name
```cpp
std::vector<std::shared_ptr<DOMElement>> DOMDocument::GetElementsByClassName(const std::string& class_name);
```

##### Query Selector
```cpp
std::shared_ptr<DOMNode> DOMDocument::QuerySelector(const std::string& selector);
std::vector<std::shared_ptr<DOMNode>> DOMDocument::QuerySelectorAll(const std::string& selector);
```

**Example:**
```cpp
// CSS selector kullanımı
auto element = document->QuerySelector("#main.container h1");
auto elements = document->QuerySelectorAll("p.highlight");
```

#### Element Creation

##### Create Element
```cpp
std::shared_ptr<DOMElement> DOMDocument::CreateElement(const std::string& tag_name);
```

##### Create Text Node
```cpp
std::shared_ptr<DOMText> DOMDocument::CreateTextNode(const std::string& text);
```

**Example:**
```cpp
auto div = document->CreateElement("div");
div->SetAttribute("class", "new-element");
div->SetAttribute("id", "dynamic-div");

auto text = document->CreateTextNode("Dynamic content");
div->AppendChild(text);

document->GetDocumentElement()->AppendChild(div);
```

### DOMElement Class

#### Attribute Methods

##### Get Attribute
```cpp
std::string DOMElement::GetAttribute(const std::string& name) const;
```

##### Set Attribute
```cpp
void DOMElement::SetAttribute(const std::string& name, const std::string& value);
```

##### Remove Attribute
```cpp
void DOMElement::RemoveAttribute(const std::string& name);
```

#### Class Methods

##### Has Class
```cpp
bool DOMElement::HasClass(const std::string& class_name) const;
```

##### Add Class
```cpp
void DOMElement::AddClass(const std::string& class_name);
```

##### Remove Class
```cpp
void DOMElement::RemoveClass(const std::string& class_name);
```

##### Toggle Class
```cpp
void DOMElement::ToggleClass(const std::string& class_name);
```

#### Style Methods

##### Get Style
```cpp
std::string DOMElement::GetStyle(const std::string& property_name);
```

##### Set Style
```cpp
void DOMElement::SetStyle(const std::string& property_name, const std::string& value);
```

#### Content Methods

##### Get Inner HTML
```cpp
std::string DOMElement::GetInnerHTML() const;
```

##### Set Inner HTML
```cpp
void DOMElement::SetInnerHTML(const std::string& html);
```

##### Get Text Content
```cpp
std::string DOMElement::GetTextContent() const;
```

##### Set Text Content
```cpp
void DOMElement::SetTextContent(const std::string& text);
```

#### Event Handling

##### Click Event
```cpp
element->on_click = [](const std::string& event_data) {
    std::cout << "Element clicked!" << std::endl;
};
```

##### Change Event
```cpp
element->on_change = [](const std::string& event_data) {
    std::cout << "Element changed!" << std::endl;
};
```

## 🚀 Complete Example

```cpp
#include "Includes/Core/comper_core.h"

int main() {
    try {
        // Network manager'ı başlat
        NetworkManager network;
        network.Initialize();
        network.SetUserAgent("ComperEngine/1.0");
        
        // Web sayfasını yükle
        NetworkResponse response = network.Get("https://example.com");
        
        if (!response.IsSuccess()) {
            std::cerr << "Failed to load page: " << response.error_message << std::endl;
            return 1;
        }
        
        // HTML'i parse et
        HTMLParser parser;
        std::shared_ptr<DOMDocument> document = parser.Parse(response.body);
        
        if (parser.HasErrors()) {
            std::cerr << "HTML parsing errors:" << std::endl;
            for (const auto& error : parser.GetErrors()) {
                std::cerr << "  " << error << std::endl;
            }
        }
        
        // Renderer'ı başlat
        Renderer renderer;
        if (!renderer.Initialize(GetActiveWindow())) {
            std::cerr << "Failed to initialize renderer!" << std::endl;
            return 1;
        }
        
        // Ana render döngüsü
        while (true) {
            renderer.BeginFrame();
            
            // Night Red Ultra arka plan
            renderer.Clear(Color(0.02f, 0.02f, 0.02f, 1.0f)); // #050505
            
            // Başlığı çiz
            auto title_element = document->QuerySelector("h1");
            if (title_element) {
                renderer.DrawText(title_element->GetTextContent(), 
                               Point(50, 50), 
                               Color(0.9f, 0.0f, 0.0f, 1.0f)); // #E60000
            }
            
            // Paragrafları çiz
            auto paragraphs = document->GetElementsByTagName("p");
            for (size_t i = 0; i < paragraphs.size(); i++) {
                auto paragraph = paragraphs[i];
                renderer.DrawText(paragraph->GetTextContent(), 
                               Point(50, 100 + i * 30), 
                               Color(1.0f, 1.0f, 1.0f, 1.0f)); // #FFFFFF
            }
            
            renderer.EndFrame();
            renderer.Present();
            
            // Çıkış kontrolü
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                break;
            }
        }
        
        renderer.Shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## 🔧 Configuration

### Compiler Settings

```cmake
# CMakeLists.txt
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Includes/Core)

# Link libraries
target_link_libraries(YourApp 
    ComperCore
    ${CMAKE_SOURCE_DIR}/ThirdParty/Skia/lib/skia.lib
    ${CMAKE_SOURCE_DIR}/ThirdParty/v8/lib/v8.lib
)
```

### Preprocessor Definitions

```cpp
#define COMPER_CORE_EXPORT
#define COMPER_NETWORK_EXPORT
#define COMPER_RENDER_EXPORT
#define COMPER_HTML_EXPORT
```

## 📊 Performance Considerations

### Network Optimization
- Connection pooling kullanın
- Request caching'i etkinleştirin
- Async operations kullanın
- Rate limiting uygulayın

### Render Optimization
- Batch drawing operations kullanın
- Texture atlases kullanın
- GPU acceleration'i etkinleştirin
- VSync'i ayarlayın

### HTML Parsing Optimization
- Streaming parser kullanın
- Memory pooling uygulayın
- Lazy loading kullanın

## 🐛 Error Handling

### Network Errors
```cpp
if (!response.IsSuccess()) {
    if (response.IsClientError()) {
        std::cerr << "Client error: " << response.status_code << std::endl;
    } else if (response.IsServerError()) {
        std::cerr << "Server error: " << response.status_code << std::endl;
    }
    
    if (!response.error_message.empty()) {
        std::cerr << "Error: " << response.error_message << std::endl;
    }
}
```

### Render Errors
```cpp
if (!renderer.Initialize(window_handle)) {
    std::cerr << "Renderer initialization failed!" << std::endl;
    // Fallback veya çıkış
}
```

### HTML Parsing Errors
```cpp
if (parser.HasErrors()) {
    for (const auto& error : parser.GetErrors()) {
        std::cerr << "Parse error: " << error << std::endl;
    }
    // Hatalara rağmen devam et veya çık
}
```

## 📚 Additional Resources

- [SDK API Documentation](SDK_API.md)
- [Tutorials](../Tutorials/)
- [Examples](../Examples/)
- [Night Red Ultra Theme](../Resources/Themes/)

---

**Comper Engine Core API** - High-performance, modern C++ tarayıcı motoru API'si.
