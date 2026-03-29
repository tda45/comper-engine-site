#include "../Includes/comper_core.h"
#include <regex>
#include <iostream>
#include <algorithm>

namespace Comper {

// HTMLParser sınıfı implementasyonu
class HTMLParser {
private:
    std::string html_content;
    size_t current_pos;
    
    // Yardımcı fonksiyonlar
    std::string GetNextToken();
    void SkipWhitespace();
    bool Match(const std::string& expected);
    char Peek();
    char Advance();
    
public:
    HTMLParser() : current_pos(0) {}
    
    std::shared_ptr<HTMLElement> Parse(const std::string& html) {
        html_content = html;
        current_pos = 0;
        
        if (html_content.empty()) {
            return nullptr;
        }
        
        return ParseElement();
    }
    
private:
    std::shared_ptr<HTMLElement> ParseElement() {
        SkipWhitespace();
        
        // Açılış tag'ini bul
        if (Match("<")) {
            return ParseTag();
        }
        
        // Metin içeriği
        return ParseText();
    }
    
    std::shared_ptr<HTMLElement> ParseTag() {
        // Tag adını al
        std::string tag_name = GetTagName();
        
        auto element = std::make_shared<HTMLElement>(tag_name);
        
        // Attribute'ları parse et
        ParseAttributes(element);
        
        // Kapanış tag'ini kontrol et
        SkipWhitespace();
        
        if (Match("/>")) {
            // Self-closing tag
            return element;
        }
        
        if (!Match(">")) {
            std::cerr << "Hata: Beklenmedik tag yapısı" << std::endl;
            return element;
        }
        
        // İçerik ve çocuk elementleri parse et
        ParseContent(element);
        
        // Kapanış tag'ini bekle
        ParseClosingTag(tag_name);
        
        return element;
    }
    
    std::string GetTagName() {
        std::string tag_name;
        SkipWhitespace();
        
        while (current_pos < html_content.length() && 
               !std::isspace(html_content[current_pos]) && 
               html_content[current_pos] != '>' && 
               html_content[current_pos] != '/') {
            tag_name += html_content[current_pos];
            current_pos++;
        }
        
        std::transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);
        return tag_name;
    }
    
    void ParseAttributes(std::shared_ptr<HTMLElement> element) {
        SkipWhitespace();
        
        while (current_pos < html_content.length() && 
               html_content[current_pos] != '>' && 
               html_content[current_pos] != '/' && 
               html_content[current_pos] != '=') {
            
            std::string attr_name = GetAttributeName();
            SkipWhitespace();
            
            if (Match("=")) {
                std::string attr_value = GetAttributeValue();
                element->attributes[attr_name] = attr_value;
            } else {
                // Değersiz attribute (örn: "disabled")
                element->attributes[attr_name] = "";
            }
            
            SkipWhitespace();
        }
    }
    
    std::string GetAttributeName() {
        std::string name;
        while (current_pos < html_content.length() && 
               !std::isspace(html_content[current_pos]) && 
               html_content[current_pos] != '>' && 
               html_content[current_pos] != '=' && 
               html_content[current_pos] != '/') {
            name += html_content[current_pos];
            current_pos++;
        }
        return name;
    }
    
    std::string GetAttributeValue() {
        SkipWhitespace();
        
        char quote_char = '"';
        if (Match("'")) {
            quote_char = '\'';
        } else if (Match("\"")) {
            quote_char = '"';
        } else {
            // Tırnak işareti olmadan
            std::string value;
            while (current_pos < html_content.length() && 
                   !std::isspace(html_content[current_pos]) && 
                   html_content[current_pos] != '>' && 
                   html_content[current_pos] != '/') {
                value += html_content[current_pos];
                current_pos++;
            }
            return value;
        }
        
        std::string value;
        while (current_pos < html_content.length() && html_content[current_pos] != quote_char) {
            if (html_content[current_pos] == '\\') {
                current_pos++; // Escape karakteri atla
                if (current_pos < html_content.length()) {
                    value += html_content[current_pos];
                    current_pos++;
                }
            } else {
                value += html_content[current_pos];
                current_pos++;
            }
        }
        
        Match(std::string(1, quote_char)); // Kapanış tırnağını tüket
        return value;
    }
    
    void ParseContent(std::shared_ptr<HTMLElement> element) {
        while (current_pos < html_content.length()) {
            SkipWhitespace();
            
            if (current_pos >= html_content.length()) break;
            
            // Kapanış tag'ine geldik mi?
            if (html_content[current_pos] == '<') {
                size_t next_pos = current_pos + 1;
                if (next_pos < html_content.length() && html_content[next_pos] == '/') {
                    break; // Kapanış tag'i başlıyor
                }
                
                // Yeni element
                auto child = ParseElement();
                if (child) {
                    element->children.push_back(child);
                }
            } else {
                // Metin içeriği
                auto text_element = ParseText();
                if (text_element) {
                    element->children.push_back(text_element);
                }
            }
        }
    }
    
    void ParseClosingTag(const std::string& expected_tag) {
        SkipWhitespace();
        
        if (!Match("<")) {
            return;
        }
        
        if (!Match("/")) {
            return;
        }
        
        std::string closing_tag = GetTagName();
        SkipWhitespace();
        Match(">");
        
        // Tag adlarını kontrol et (opsiyonel)
        if (closing_tag != expected_tag) {
            std::cerr << "Uyarı: Açılış ve kapanış tag'leri eşleşmiyor: " 
                      << expected_tag << " vs " << closing_tag << std::endl;
        }
    }
    
    std::shared_ptr<HTMLElement> ParseText() {
        std::string text_content;
        
        while (current_pos < html_content.length() && html_content[current_pos] != '<') {
            if (html_content[current_pos] == '\\') {
                current_pos++;
                if (current_pos < html_content.length()) {
                    text_content += html_content[current_pos];
                    current_pos++;
                }
            } else {
                text_content += html_content[current_pos];
                current_pos++;
            }
        }
        
        // Boş metin elementleri oluşturma
        if (text_content.empty()) {
            return nullptr;
        }
        
        // Başta ve sonda boşlukları temizle ama iç boşlukları koru
        text_content = std::regex_replace(text_content, std::regex("^\\s+|\\s+$"), "");
        
        if (text_content.empty()) {
            return nullptr;
        }
        
        auto text_element = std::make_shared<HTMLElement>("text");
        text_element->content = text_content;
        return text_element;
    }
    
    void SkipWhitespace() {
        while (current_pos < html_content.length() && 
               std::isspace(html_content[current_pos])) {
            current_pos++;
        }
    }
    
    bool Match(const std::string& expected) {
        if (current_pos + expected.length() > html_content.length()) {
            return false;
        }
        
        if (html_content.substr(current_pos, expected.length()) == expected) {
            current_pos += expected.length();
            return true;
        }
        
        return false;
    }
    
    char Peek() {
        if (current_pos >= html_content.length()) {
            return '\0';
        }
        return html_content[current_pos];
    }
    
    char Advance() {
        if (current_pos >= html_content.length()) {
            return '\0';
        }
        return html_content[current_pos++];
    }
};

// ComperEngine'in HTMLParser kullanımı
std::shared_ptr<HTMLElement> ComperEngine::ParseHTML(const std::string& html) {
    if (!html_parser) {
        std::cerr << "HTMLParser başlatılmadı" << std::endl;
        return nullptr;
    }
    
    return html_parser->Parse(html);
}

} // namespace Comper
