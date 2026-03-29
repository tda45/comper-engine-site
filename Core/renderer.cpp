#include "../Includes/comper_core.h"
#include <iostream>
#include <algorithm>

// Basit bir çizim kütüphanesi için Windows API
#ifdef _WIN32
#include <windows.h>
#include <wingdi.h>
#endif

namespace Comper {

// ComperRenderer sınıfı implementasyonu
class ComperRenderer {
private:
    Theme* theme;
    int window_width;
    int window_height;
    int cursor_x;
    int cursor_y;
    int line_height;
    int margin;
    
#ifdef _WIN32
    HWND window_handle;
    HDC device_context;
    HFONT default_font;
    HBRUSH background_brush;
    HPEN text_pen;
#endif
    
public:
    ComperRenderer(Theme* current_theme) 
        : theme(current_theme), 
          window_width(800), 
          window_height(600),
          cursor_x(20),
          cursor_y(20),
          line_height(20),
          margin(20) {
        
        InitializeGraphics();
    }
    
    ~ComperRenderer() {
        CleanupGraphics();
    }
    
    void SetWindowSize(int width, int height) {
        window_width = width;
        window_height = height;
    }
    
    void ClearScreen() {
#ifdef _WIN32
        if (device_context) {
            RECT rect = {0, 0, window_width, window_height};
            HBRUSH brush = CreateSolidBrush(RGB(theme->background.r, theme->background.g, theme->background.b));
            FillRect(device_context, &rect, brush);
            DeleteObject(brush);
        }
#else
        // Konsol modu için basit temizlik
        std::cout << "\033[2J\033[H"; // ANSI escape codes
#endif
        
        cursor_x = margin;
        cursor_y = margin;
    }
    
    void RenderElement(std::shared_ptr<HTMLElement> element) {
        if (!element) return;
        
        if (element->tag == "text") {
            RenderText(element->content);
        } else if (element->tag == "h1") {
            RenderHeading(element->content, 24);
        } else if (element->tag == "h2") {
            RenderHeading(element->content, 20);
        } else if (element->tag == "h3") {
            RenderHeading(element->content, 18);
        } else if (element->tag == "p") {
            RenderParagraph(element);
        } else if (element->tag == "div") {
            RenderDiv(element);
        } else if (element->tag == "button") {
            RenderButton(element);
        } else if (element->tag == "input") {
            RenderInput(element);
        } else if (element->tag == "a") {
            RenderLink(element);
        } else {
            // Bilinmeyen tag - çocukları render et
            RenderChildren(element);
        }
    }
    
    void RenderPage(std::shared_ptr<HTMLElement> root) {
        ClearScreen();
        
        if (root) {
            RenderElement(root);
        }
        
        // DevTools inspector'u çiz (görünürse)
        UpdateDisplay();
    }
    
private:
    void InitializeGraphics() {
#ifdef _WIN32
        // Windows için basit grafik başlatma
        window_handle = GetConsoleWindow();
        if (window_handle) {
            device_context = GetDC(window_handle);
            
            // Font oluştur
            default_font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
            
            if (default_font && device_context) {
                SelectObject(device_context, default_font);
            }
        }
#endif
    }
    
    void CleanupGraphics() {
#ifdef _WIN32
        if (default_font) {
            DeleteObject(default_font);
        }
        if (device_context && window_handle) {
            ReleaseDC(window_handle, device_context);
        }
#endif
    }
    
    void UpdateDisplay() {
        // Ekran güncellemesi (platform'a özel)
#ifdef _WIN32
        if (window_handle) {
            InvalidateRect(window_handle, nullptr, TRUE);
            UpdateWindow(window_handle);
        }
#endif
    }
    
    void RenderText(const std::string& text) {
        if (text.empty()) return;
        
#ifdef _WIN32
        if (device_context) {
            SetTextColor(device_context, RGB(theme->text.r, theme->text.g, theme->text.b));
            SetBkMode(device_context, TRANSPARENT);
            
            // UTF-8'den wide string'e çevir
            int wide_len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
            if (wide_len > 0) {
                std::wstring wide_text(wide_len, 0);
                MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wide_text[0], wide_len);
                
                TextOut(device_context, cursor_x, cursor_y, wide_text.c_str(), wide_len - 1);
            }
            
            // Metin genişliğini hesapla
            SIZE text_size;
            GetTextExtentPoint32(device_context, wide_text.c_str(), wide_len - 1, &text_size);
            cursor_x += text_size.cx;
        }
#else
        // Konsol çıktısı
        std::cout << "\033[38;2;" << theme->text.r << ";" << theme->text.g << ";" << theme->text.b << "m";
        std::cout << text;
        std::cout << "\033[0m";
#endif
    }
    
    void RenderHeading(const std::string& text, int font_size) {
        // Başlıkları kalın ve büyük olarak render et
        NewLine();
        NewLine();
        
#ifdef _WIN32
        if (device_context) {
            HFONT heading_font = CreateFont(font_size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                          DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
            
            if (heading_font) {
                HFONT old_font = (HFONT)SelectObject(device_context, heading_font);
                
                SetTextColor(device_context, RGB(theme->accent.r, theme->accent.g, theme->accent.b));
                
                int wide_len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
                if (wide_len > 0) {
                    std::wstring wide_text(wide_len, 0);
                    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wide_text[0], wide_len);
                    
                    TextOut(device_context, cursor_x, cursor_y, wide_text.c_str(), wide_len - 1);
                    
                    SIZE text_size;
                    GetTextExtentPoint32(device_context, wide_text.c_str(), wide_len - 1, &text_size);
                    cursor_x += text_size.cx;
                }
                
                SelectObject(device_context, old_font);
                DeleteObject(heading_font);
            }
        }
#else
        std::cout << "\n\n\033[1;38;2;" << theme->accent.r << ";" << theme->accent.g << ";" << theme->accent.b << "m";
        std::cout << text;
        std::cout << "\033[0m";
#endif
        
        NewLine();
        line_height = font_size + 10;
    }
    
    void RenderParagraph(std::shared_ptr<HTMLElement> element) {
        NewLine();
        RenderChildren(element);
        NewLine();
        line_height = 20;
    }
    
    void RenderDiv(std::shared_ptr<HTMLElement> element) {
        // DIV'i kutu olarak çiz
        int box_width = window_width - 2 * margin;
        int box_height = EstimateContentHeight(element);
        
        DrawBox(cursor_x - 5, cursor_y - 5, box_width, box_height + 10, theme->button_bg);
        
        RenderChildren(element);
        NewLine();
    }
    
    void RenderButton(std::shared_ptr<HTMLElement> element) {
        std::string button_text = element->content;
        if (button_text.empty()) {
            button_text = element->attributes["value"];
        }
        
        int button_width = 100;
        int button_height = 30;
        
        // Buton arka planını çiz
        DrawBox(cursor_x, cursor_y, button_width, button_height, theme->button_bg);
        
        // Buton metnini ortala
        int text_x = cursor_x + (button_width - GetTextWidth(button_text)) / 2;
        int text_y = cursor_y + (button_height - 16) / 2;
        
        int old_x = cursor_x;
        int old_y = cursor_y;
        cursor_x = text_x;
        cursor_y = text_y;
        
        RenderText(button_text);
        
        cursor_x = old_x + button_width + 10;
        cursor_y = old_y;
    }
    
    void RenderInput(std::shared_ptr<HTMLElement> element) {
        std::string input_type = element->attributes["type"];
        if (input_type.empty()) input_type = "text";
        
        int input_width = 200;
        int input_height = 25;
        
        // Input kutusunu çiz
        DrawBox(cursor_x, cursor_y, input_width, input_height, Color(255, 255, 255));
        
        // Placeholder veya değeri göster
        std::string value = element->attributes["value"];
        if (value.empty()) {
            value = element->attributes["placeholder"];
        }
        
        if (!value.empty()) {
            int old_x = cursor_x;
            int old_y = cursor_y;
            cursor_x += 5;
            cursor_y += 3;
            
            Color old_text = theme->text;
            theme->text = Color(128, 128, 128); // Gri için placeholder
            RenderText(value);
            theme->text = old_text;
            
            cursor_x = old_x;
            cursor_y = old_y;
        }
        
        cursor_x += input_width + 10;
    }
    
    void RenderLink(std::shared_ptr<HTMLElement> element) {
        std::string link_text = element->content;
        std::string href = element->attributes["href"];
        
        // Link rengini ayarla
        Color old_text = theme->text;
        theme->text = theme->accent;
        
        RenderText(link_text);
        
        // Alt çizgi çiz
        DrawUnderline(cursor_x - GetTextWidth(link_text), cursor_y + 16, GetTextWidth(link_text));
        
        theme->text = old_text;
    }
    
    void RenderChildren(std::shared_ptr<HTMLElement> element) {
        for (const auto& child : element->children) {
            RenderElement(child);
        }
    }
    
    void DrawBox(int x, int y, int width, int height, const Color& color) {
#ifdef _WIN32
        if (device_context) {
            HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(color.r, color.g, color.b));
            
            SelectObject(device_context, brush);
            SelectObject(device_context, pen);
            
            Rectangle(device_context, x, y, x + width, y + height);
            
            DeleteObject(brush);
            DeleteObject(pen);
        }
#else
        std::cout << "\033[48;2;" << color.r << ";" << color.g << ";" << color.b << "m";
        for (int i = 0; i < height; i++) {
            std::cout << "\n" << std::string(width, ' ');
        }
        std::cout << "\033[0m";
#endif
    }
    
    void DrawUnderline(int x, int y, int width) {
#ifdef _WIN32
        if (device_context) {
            HPEN pen = CreatePen(PS_SOLID, 1, RGB(theme->accent.r, theme->accent.g, theme->accent.b));
            SelectObject(device_context, pen);
            
            MoveToEx(device_context, x, y, nullptr);
            LineTo(device_context, x + width, y);
            
            DeleteObject(pen);
        }
#else
        std::cout << "\033[4m"; // ANSI underline
#endif
    }
    
    int GetTextWidth(const std::string& text) {
#ifdef _WIN32
        if (device_context) {
            int wide_len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
            if (wide_len > 0) {
                std::wstring wide_text(wide_len, 0);
                MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wide_text[0], wide_len);
                
                SIZE text_size;
                GetTextExtentPoint32(device_context, wide_text.c_str(), wide_len - 1, &text_size);
                return text_size.cx;
            }
        }
#endif
        return text.length() * 8; // Tahmini genişlik
    }
    
    int EstimateContentHeight(std::shared_ptr<HTMLElement> element) {
        // Basit yükseklik tahmini
        int height = 0;
        for (const auto& child : element->children) {
            if (child->tag == "text") {
                height += line_height;
            } else {
                height += 30; // Element için varsayılan yükseklik
            }
        }
        return std::max(height, 30);
    }
    
    void NewLine() {
        cursor_x = margin;
        cursor_y += line_height;
        
        // Sayfa sonuna geldiysek kaydır
        if (cursor_y > window_height - margin) {
            cursor_y = margin;
        }
    }
};

// ComperEngine'in Renderer kullanımı
void ComperEngine::RenderPage(std::shared_ptr<HTMLElement> root) {
    if (!renderer) {
        std::cerr << "Renderer başlatılmadı" << std::endl;
        return;
    }
    
    renderer->RenderPage(root);
}

void ComperEngine::ClearScreen() {
    if (renderer) {
        renderer->ClearScreen();
    }
}

} // namespace Comper
