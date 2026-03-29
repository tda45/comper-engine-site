#include "../Includes/comper_core.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Comper {

// DevTools Inspector sınıfı implementasyonu
class DevToolsInspector {
private:
    bool is_visible;
    Theme* theme;
    std::shared_ptr<HTMLElement> inspected_element;
    int inspector_width;
    int inspector_height;
    int inspector_x;
    int inspector_y;
    
public:
    DevToolsInspector(Theme* current_theme) 
        : is_visible(false), 
          theme(current_theme),
          inspector_width(400),
          inspector_height(300),
          inspector_x(10),
          inspector_y(10) {
    }
    
    void Show() {
        is_visible = true;
        std::cout << "\n=== Comper DevTools Inspector Aktif ===" << std::endl;
        std::cout << "Sağ tık -> 'İncele' ile elementleri inceleyebilirsiniz" << std::endl;
    }
    
    void Hide() {
        is_visible = false;
        std::cout << "\n=== Comper DevTools Inspector Pasif ===" << std::endl;
    }
    
    bool IsVisible() const {
        return is_visible;
    }
    
    void InspectElement(std::shared_ptr<HTMLElement> element) {
        if (!is_visible) return;
        
        inspected_element = element;
        RenderInspector();
    }
    
    void SetPosition(int x, int y) {
        inspector_x = x;
        inspector_y = y;
    }
    
    void SetSize(int width, int height) {
        inspector_width = width;
        inspector_height = height;
    }
    
private:
    void RenderInspector() {
        if (!inspected_element) return;
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "COMPER DEVTOOLS - ELEMENT INSPECTOR" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        PrintElementInfo(inspected_element, 0);
        
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Komutlar: [b] geri, [q] çıkış, [s] styles" << std::endl;
    }
    
    void PrintElementInfo(std::shared_ptr<HTMLElement> element, int depth) {
        if (!element) return;
        
        // Girintileme
        std::string indent(depth * 2, ' ');
        
        // Tag bilgisi
        std::cout << indent << "<" << element->tag;
        
        // Attribute'lar
        for (const auto& attr : element->attributes) {
            std::cout << " " << attr.first << "=\"" << attr.second << "\"";
        }
        
        if (element->children.empty()) {
            std::cout << " />";
        } else {
            std::cout << ">";
        }
        
        // Metin içeriği
        if (!element->content.empty() && element->tag == "text") {
            std::cout << " \"" << element->content << "\"";
        }
        
        std::cout << std::endl;
        
        // Çocuk elementleri
        for (const auto& child : element->children) {
            PrintElementInfo(child, depth + 1);
        }
        
        // Kapanış tag'i
        if (!element->children.empty()) {
            std::cout << indent << "</" << element->tag << ">" << std::endl;
        }
    }
    
    void PrintElementStyles(std::shared_ptr<HTMLElement> element) {
        if (!element) return;
        
        std::cout << "\n--- STYLES ---" << std::endl;
        std::cout << "Tag: " << element->tag << std::endl;
        
        // Varsayılan stiller
        if (element->tag == "h1") {
            std::cout << "font-size: 24px" << std::endl;
            std::cout << "font-weight: bold" << std::endl;
            std::cout << "color: #" << std::hex << theme->accent.r << theme->accent.g << theme->accent.b << std::dec << std::endl;
        } else if (element->tag == "button") {
            std::cout << "background: #" << std::hex << theme->button_bg.r << theme->button_bg.g << theme->button_bg.b << std::dec << std::endl;
            std::cout << "width: 100px" << std::endl;
            std::cout << "height: 30px" << std::endl;
        }
        
        // Inline stiller (varsa)
        auto style_attr = element->attributes.find("style");
        if (style_attr != element->attributes.end()) {
            std::cout << "inline-style: " << style_attr->second << std::endl;
        }
        
        std::cout << "--------------" << std::endl;
    }
};

// ComperEngine'in DevTools kullanımı
void ComperEngine::ShowInspector() {
    if (inspector) {
        inspector->Show();
    }
}

void ComperEngine::HideInspector() {
    if (inspector) {
        inspector->Hide();
    }
}

bool ComperEngine::IsInspectorVisible() const {
    return inspector ? inspector->IsVisible() : false;
}

} // namespace Comper
