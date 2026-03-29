#ifndef COMPER_RENDER_H
#define COMPER_RENDER_H

/*
 * Comper Engine Core - Render Module
 * 
 * 2D/3D rendering, UI çizimi, Night Red Ultra grafik motoru
 * Hardware acceleration, shader destekli, yüksek performans
 * 
 * Features:
 * - 2D/3D rendering
 * - Hardware acceleration
 * - Shader support
 * - UI element rendering
 * - Text rendering
 * - Image processing
 * - Animation support
 * - 4K/8K support
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <chrono>

#ifdef _WIN32
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dwrite.h>
#include <wincodec.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#else
#include <vulkan/vulkan.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

namespace ComperCore {

// Color structure
struct Color {
    float r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(1) {}
    Color(float red, float green, float blue, float alpha = 1.0f) 
        : r(red), g(green), b(blue), a(alpha) {}
    
    Color(uint32_t hex) {
        r = ((hex >> 24) & 0xFF) / 255.0f;
        g = ((hex >> 16) & 0xFF) / 255.0f;
        b = ((hex >> 8) & 0xFF) / 255.0f;
        a = (hex & 0xFF) / 255.0f;
    }
    
    uint32_t ToHex() const {
        return ((uint32_t)(r * 255) << 24) |
               ((uint32_t)(g * 255) << 16) |
               ((uint32_t)(b * 255) << 8) |
               ((uint32_t)(a * 255));
    }
    
    static Color FromHex(const std::string& hex);
    static Color FromHSV(float h, float s, float v);
};

// Rectangle structure
struct Rectangle {
    float x, y, width, height;
    
    Rectangle() : x(0), y(0), width(0), height(0) {}
    Rectangle(float x_pos, float y_pos, float w, float h) 
        : x(x_pos), y(y_pos), width(w), height(h) {}
    
    bool Contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
    
    bool Intersects(const Rectangle& other) const {
        return x < other.x + other.width && x + width > other.x &&
               y < other.y + other.height && y + height > other.y;
    }
};

// Point structure
struct Point {
    float x, y;
    
    Point() : x(0), y(0) {}
    Point(float x_pos, float y_pos) : x(x_pos), y(y_pos) {}
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    float Distance(const Point& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
};

// Vertex structure
struct Vertex {
    Point position;
    Color color;
    Point tex_coord;
    Point normal;
    
    Vertex() {}
    Vertex(const Point& pos, const Color& col, const Point& tc = Point(), const Point& n = Point())
        : position(pos), color(col), tex_coord(tc), normal(n) {}
};

// Render target
class RenderTarget {
public:
    virtual ~RenderTarget() = default;
    virtual void Resize(int width, int height) = 0;
    virtual void Clear(const Color& color) = 0;
    virtual void Present() = 0;
    virtual void* GetHandle() = 0;
};

// Texture
class Texture {
public:
    virtual ~Texture() = default;
    virtual bool LoadFromFile(const std::string& filename) = 0;
    virtual bool LoadFromMemory(const void* data, size_t size) = 0;
    virtual void* GetHandle() = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual void SetFilterMode(int mode) = 0;
};

// Font
class Font {
public:
    virtual ~Font() = default;
    virtual bool LoadFromFile(const std::string& filename) = 0;
    virtual void SetSize(float size) = 0;
    virtual float MeasureText(const std::string& text) = 0;
    virtual void* GetHandle() = 0;
};

// Shader
class Shader {
public:
    virtual ~Shader() = default;
    virtual bool LoadFromFile(const std::string& vertex_file, const std::string& fragment_file) = 0;
    virtual bool LoadFromSource(const std::string& vertex_source, const std::string& fragment_source) = 0;
    virtual void* GetHandle() = 0;
    virtual void SetUniform(const std::string& name, float value) = 0;
    virtual void SetUniform(const std::string& name, const Color& value) = 0;
    virtual void SetUniform(const std::string& name, const Point& value) = 0;
};

// Render command
struct RenderCommand {
    enum Type {
        CLEAR,
        DRAW_RECTANGLE,
        DRAW_TEXTURE,
        DRAW_TEXT,
        DRAW_LINE,
        DRAW_TRIANGLE,
        SET_SHADER,
        SET_TEXTURE,
        SET_TARGET
    };
    
    Type type;
    std::vector<float> parameters;
    std::string string_param;
    void* handle_param;
    
    RenderCommand(Type cmd_type) : type(cmd_type), handle_param(nullptr) {}
};

// Renderer interface
class Renderer {
protected:
    std::vector<RenderCommand> command_queue;
    std::unique_ptr<RenderTarget> default_target;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    std::map<std::string, std::shared_ptr<Font>> fonts;
    std::map<std::string, std::shared_ptr<Shader>> shaders;
    std::shared_ptr<Shader> current_shader;
    std::shared_ptr<Texture> current_texture;
    std::shared_ptr<RenderTarget> current_target;
    
    int window_width = 800;
    int window_height = 600;
    bool vsync_enabled = true;
    bool wireframe_mode = false;
    Color clear_color = Color(0.05f, 0.05f, 0.05f, 1.0f); // Night Red background
    
public:
    virtual ~Renderer() = default;
    
    // Initialization
    virtual bool Initialize(void* window_handle) = 0;
    virtual void Shutdown() = 0;
    
    // Frame management
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;
    
    // Render targets
    virtual std::shared_ptr<RenderTarget> CreateRenderTarget(int width, int height) = 0;
    virtual void SetRenderTarget(std::shared_ptr<RenderTarget> target) = 0;
    virtual void SetDefaultRenderTarget() = 0;
    
    // Drawing commands
    virtual void Clear(const Color& color = Color()) = 0;
    virtual void DrawRectangle(const Rectangle& rect, const Color& color) = 0;
    virtual void DrawFilledRectangle(const Rectangle& rect, const Color& color) = 0;
    virtual void DrawTexture(std::shared_ptr<Texture> texture, const Rectangle& dest_rect, 
                           const Rectangle& src_rect = Rectangle()) = 0;
    virtual void DrawText(const std::string& text, const Point& position, 
                        const Color& color, std::shared_ptr<Font> font = nullptr) = 0;
    virtual void DrawLine(const Point& start, const Point& end, const Color& color, float thickness = 1.0f) = 0;
    virtual void DrawTriangle(const Point& p1, const Point& p2, const Point& p3, const Color& color) = 0;
    
    // Resource management
    virtual std::shared_ptr<Texture> LoadTexture(const std::string& filename) = 0;
    virtual std::shared_ptr<Font> LoadFont(const std::string& filename) = 0;
    virtual std::shared_ptr<Shader> LoadShader(const std::string& vertex_file, const std::string& fragment_file) = 0;
    
    // Shader management
    virtual void SetShader(std::shared_ptr<Shader> shader) = 0;
    virtual void SetTexture(std::shared_ptr<Texture> texture) = 0;
    
    // Configuration
    virtual void SetVSync(bool enabled) = 0;
    virtual void SetWireframe(bool enabled) = 0;
    virtual void SetClearColor(const Color& color) = 0;
    virtual void SetWindowSize(int width, int height) = 0;
    
    // Night Red Ultra specific
    virtual void EnableNeonGlow(bool enabled) = 0;
    virtual void SetNeonIntensity(float intensity) = 0;
    virtual void SetNeonColor(const Color& color) = 0;
    virtual void EnableBorderEffects(bool enabled) = 0;
    virtual void SetBorderAnimation(const std::string& animation) = 0;
    
    // Utility
    virtual void* GetHandle() = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual float GetFPS() const = 0;
    virtual void GetStats(int& draw_calls, int& triangles_drawn) const = 0;
};

// Night Red Ultra renderer
class NightRedUltraRenderer : public Renderer {
private:
    struct NeonEffect {
        bool enabled = true;
        float intensity = 0.8f;
        Color glow_color = Color(230, 0, 0, 255); // #E60000
        float pulse_speed = 2.0f;
        float pulse_phase = 0.0f;
    } neon_effect;
    
    struct BorderEffect {
        bool enabled = true;
        std::string animation_type = "pulse";
        float animation_speed = 1.0f;
        float animation_phase = 0.0f;
        Color border_color = Color(230, 0, 0, 255);
        float border_width = 2.0f;
    } border_effect;
    
    int draw_calls = 0;
    int triangles_drawn = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time;
    float current_fps = 0.0f;
    
public:
    NightRedUltraRenderer();
    ~NightRedUltraRenderer() override;
    
    // Override base methods
    bool Initialize(void* window_handle) override;
    void Shutdown() override;
    void BeginFrame() override;
    void EndFrame() override;
    void Present() override;
    
    // Drawing with Night Red effects
    void DrawNeonRectangle(const Rectangle& rect, const Color& color);
    void DrawNeonText(const std::string& text, const Point& position, 
                      const Color& color, std::shared_ptr<Font> font = nullptr);
    void DrawBorderRectangle(const Rectangle& rect, const Color& color);
    
    // Night Red Ultra specific
    void EnableNeonGlow(bool enabled) override;
    void SetNeonIntensity(float intensity) override;
    void SetNeonColor(const Color& color) override;
    void EnableBorderEffects(bool enabled) override;
    void SetBorderAnimation(const std::string& animation) override;
    
    // Animation
    void UpdateAnimations(float delta_time);
    void SetAnimationSpeed(float speed);
    
private:
    void UpdateNeonEffect(float delta_time);
    void UpdateBorderEffect(float delta_time);
    void ApplyNeonShader();
    void ApplyBorderShader();
    
    // Platform-specific implementations
#ifdef _WIN32
    bool InitializeD3D12(void* window_handle);
    void RenderD3D12();
#else
    bool InitializeVulkan(void* window_handle);
    void RenderVulkan();
#endif
};

// UI Renderer
class UIRenderer {
private:
    std::shared_ptr<Renderer> renderer;
    std::map<std::string, std::shared_ptr<Texture>> ui_textures;
    std::map<std::string, std::shared_ptr<Font>> ui_fonts;
    
public:
    UIRenderer(std::shared_ptr<Renderer> render);
    ~UIRenderer();
    
    // UI elements
    void DrawButton(const std::string& id, const Rectangle& rect, const std::string& text);
    void DrawTextBox(const std::string& id, const Rectangle& rect, const std::string& text);
    void DrawProgressBar(const std::string& id, const Rectangle& rect, float progress);
    void DrawSlider(const std::string& id, const Rectangle& rect, float value);
    void DrawCheckBox(const std::string& id, const Rectangle& rect, bool checked);
    void DrawRadioButton(const std::string& id, const Rectangle& rect, bool selected);
    
    // Layout management
    void UpdateLayout();
    void SetTheme(const std::string& theme_name);
    
    // Event handling
    bool HandleClick(const Point& click_pos);
    bool HandleKeyPress(int key_code);
    
private:
    struct UIElement {
        std::string id;
        std::string type;
        Rectangle bounds;
        std::string text;
        bool is_visible = true;
        bool is_enabled = true;
        std::function<void()> on_click;
    };
    
    std::map<std::string, UIElement> ui_elements;
};

// Factory functions
namespace RenderFactory {
    std::shared_ptr<Renderer> CreateRenderer();
    std::shared_ptr<NightRedUltraRenderer> CreateNightRedUltraRenderer();
    std::shared_ptr<UIRenderer> CreateUIRenderer(std::shared_ptr<Renderer> renderer);
}

// Utility functions
namespace RenderUtils {
    Color LerpColor(const Color& a, const Color& b, float t);
    Point LerpPoint(const Point& a, const Point& b, float t);
    Rectangle LerpRect(const Rectangle& a, const Rectangle& b, float t);
    Color ApplyNeonGlow(const Color& base_color, const Color& glow_color, float intensity);
    float CalculateFPS();
    std::string GetRendererInfo();
}

} // namespace ComperCore

#endif // COMPER_RENDER_H
