#ifndef COMPER_HTML_H
#define COMPER_HTML_H

/*
 * Comper Engine Core - HTML Module
 * 
 * HTML parsing, DOM manipulation, CSS processing
 * High-performance, memory efficient, standards compliant
 * 
 * Features:
 * - HTML5 parsing
 * - DOM tree manipulation
 * - CSS3 support
 * - JavaScript integration
 * - Event handling
 * - Layout engine
 * - Rendering pipeline
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <regex>

namespace ComperCore {

// CSS property types
enum class CSSPropertyType {
    COLOR,
    BACKGROUND_COLOR,
    FONT_SIZE,
    FONT_FAMILY,
    FONT_WEIGHT,
    TEXT_ALIGN,
    MARGIN,
    PADDING,
    WIDTH,
    HEIGHT,
    BORDER,
    DISPLAY,
    POSITION,
    TOP,
    LEFT,
    RIGHT,
    BOTTOM,
    OPACITY,
    TRANSFORM,
    ANIMATION,
    TRANSITION,
    BOX_SHADOW,
    TEXT_SHADOW,
    BORDER_RADIUS
};

// CSS value types
enum class CSSValueType {
    PIXEL,
    PERCENT,
    EM,
    REM,
    VW,
    VH,
    COLOR,
    STRING,
    NUMBER,
    AUTO,
    INHERIT
};

// CSS value structure
struct CSSValue {
    CSSValueType type;
    float number_value;
    std::string string_value;
    bool is_important = false;
    
    CSSValue() : type(CSSValueType::NUMBER), number_value(0) {}
    CSSValue(float value, CSSValueType val_type) 
        : type(val_type), number_value(value) {}
    CSSValue(const std::string& value, CSSValueType val_type = CSSValueType::STRING) 
        : type(val_type), string_value(value), number_value(0) {}
    
    float ToPixels(float parent_size = 0, float font_size = 16) const {
        switch (type) {
            case CSSValueType::PIXEL:
                return number_value;
            case CSSValueType::PERCENT:
                return parent_size * number_value / 100.0f;
            case CSSValueType::EM:
                return font_size * number_value;
            case CSSValueType::REM:
                return 16.0f * number_value; // Base font size
            case CSSValueType::VW:
                return 1280.0f * number_value / 100.0f; // Default viewport width
            case CSSValueType::VH:
                return 720.0f * number_value / 100.0f; // Default viewport height
            default:
                return 0;
        }
    }
    
    std::string ToString() const {
        if (type == CSSValueType::STRING) {
            return string_value;
        } else {
            return std::to_string(number_value);
        }
    }
};

// CSS property structure
struct CSSProperty {
    CSSPropertyType type;
    CSSValue value;
    std::string property_name;
    
    CSSProperty(CSSPropertyType prop_type, const CSSValue& prop_value, const std::string& name = "")
        : type(prop_type), value(prop_value), property_name(name) {}
};

// CSS rule structure
struct CSSRule {
    std::vector<std::string> selectors;
    std::vector<CSSProperty> properties;
    std::string media_query;
    int specificity = 0;
    
    bool Matches(const std::string& tag_name, const std::vector<std::string>& classes, 
                 const std::string& id) const;
    int CalculateSpecificity() const;
};

// Computed style
struct ComputedStyle {
    std::map<CSSPropertyType, CSSValue> properties;
    
    CSSValue GetProperty(CSSPropertyType type) const {
        auto it = properties.find(type);
        return (it != properties.end()) ? it->second : CSSValue();
    }
    
    void SetProperty(CSSPropertyType type, const CSSValue& value) {
        properties[type] = value;
    }
    
    Color GetColor(CSSPropertyType type) const {
        auto value = GetProperty(type);
        return Color::FromHex(value.string_value);
    }
    
    float GetLength(CSSPropertyType type, float parent_size = 0, float font_size = 16) const {
        return GetProperty(type).ToPixels(parent_size, font_size);
    }
};

// DOM node types
enum class DOMNodeType {
    ELEMENT,
    TEXT,
    COMMENT,
    DOCUMENT,
    DOCUMENT_TYPE,
    PROCESSING_INSTRUCTION,
    CDATA_SECTION,
    ENTITY_REFERENCE,
    ENTITY,
    NOTATION
};

// DOM node
class DOMNode {
protected:
    DOMNodeType node_type;
    std::string node_name;
    std::string node_value;
    std::shared_ptr<DOMNode> parent;
    std::vector<std::shared_ptr<DOMNode>> children;
    std::map<std::string, std::string> attributes;
    std::vector<std::string> classes;
    std::string id;
    ComputedStyle computed_style;
    
public:
    DOMNode(DOMNodeType type, const std::string& name = "", const std::string& value = "")
        : node_type(type), node_name(name), node_value(value) {}
    
    virtual ~DOMNode() = default;
    
    // Node properties
    DOMNodeType GetNodeType() const { return node_type; }
    const std::string& GetNodeName() const { return node_name; }
    const std::string& GetNodeValue() const { return node_value; }
    void SetNodeValue(const std::string& value) { node_value = value; }
    
    // Tree navigation
    std::shared_ptr<DOMNode> GetParent() const { return parent; }
    const std::vector<std::shared_ptr<DOMNode>>& GetChildren() const { return children; }
    std::shared_ptr<DOMNode> GetFirstChild() const { 
        return children.empty() ? nullptr : children[0]; 
    }
    std::shared_ptr<DOMNode> GetLastChild() const { 
        return children.empty() ? nullptr : children.back(); 
    }
    std::shared_ptr<DOMNode> GetNextSibling() const;
    std::shared_ptr<DOMNode> GetPreviousSibling() const;
    
    // Tree manipulation
    void AppendChild(std::shared_ptr<DOMNode> child);
    void RemoveChild(std::shared_ptr<DOMNode> child);
    void InsertBefore(std::shared_ptr<DOMNode> new_child, std::shared_ptr<DOMNode> reference_child);
    void ReplaceChild(std::shared_ptr<DOMNode> new_child, std::shared_ptr<DOMNode> old_child);
    
    // Attributes
    const std::map<std::string, std::string>& GetAttributes() const { return attributes; }
    std::string GetAttribute(const std::string& name) const;
    void SetAttribute(const std::string& name, const std::string& value);
    void RemoveAttribute(const std::string& name);
    bool HasAttribute(const std::string& name) const;
    
    // Classes and ID
    const std::vector<std::string>& GetClasses() const { return classes; }
    bool HasClass(const std::string& class_name) const;
    void AddClass(const std::string& class_name);
    void RemoveClass(const std::string& class_name);
    const std::string& GetID() const { return id; }
    void SetID(const std::string& new_id) { id = new_id; }
    
    // Style
    const ComputedStyle& GetComputedStyle() const { return computed_style; }
    void SetComputedStyle(const ComputedStyle& style) { computed_style = style; }
    
    // Query selectors
    std::vector<std::shared_ptr<DOMNode>> QuerySelector(const std::string& selector);
    std::shared_ptr<DOMNode> QuerySelectorOne(const std::string& selector);
    
    // Utility
    std::string GetInnerHTML() const;
    std::string GetOuterHTML() const;
    std::string GetTextContent() const;
    void SetTextContent(const std::string& text);
    
    // Event handling
    std::function<void(const std::string&)> on_click;
    std::function<void(const std::string&)> on_change;
    std::function<void(const std::string&)> on_input;
    std::function<void(const std::string&)> on_load;
    std::function<void(const std::string&)> on_error;
    
    // Layout
    struct LayoutBox {
        float x, y, width, height;
        float margin_top, margin_right, margin_bottom, margin_left;
        float padding_top, padding_right, padding_bottom, padding_left;
        float border_top, border_right, border_bottom, border_left;
        bool is_visible = true;
        bool is_positioned = false;
    } layout_box;
    
    const LayoutBox& GetLayoutBox() const { return layout_box; }
    void SetLayoutBox(const LayoutBox& box) { layout_box = box; }
};

// Element node
class DOMElement : public DOMNode {
public:
    DOMElement(const std::string& tag_name) 
        : DOMNode(DOMNodeType::ELEMENT, tag_name) {}
    
    // Element specific methods
    std::string GetTagName() const { return node_name; }
    std::string GetInnerHTML() const override;
    std::string GetOuterHTML() const override;
    
    // Style manipulation
    std::string GetStyle(const std::string& property_name);
    void SetStyle(const std::string& property_name, const std::string& value);
    std::string GetAttribute(const std::string& name) const override;
    void SetAttribute(const std::string& name, const std::string& value) override;
    
    // Class manipulation
    bool HasClass(const std::string& class_name) const override;
    void AddClass(const std::string& class_name) override;
    void RemoveClass(const std::string& class_name) override;
    void ToggleClass(const std::string& class_name);
};

// Text node
class DOMText : public DOMNode {
public:
    DOMText(const std::string& text) 
        : DOMNode(DOMNodeType::TEXT, "#text", text) {}
    
    std::string GetText() const { return node_value; }
    void SetText(const std::string& text) { node_value = text; }
    std::string GetInnerText() const override { return node_value; }
};

// Document node
class DOMDocument : public DOMNode {
private:
    std::shared_ptr<DOMNode> document_element;
    std::vector<CSSRule> css_rules;
    std::map<std::string, std::string> css_variables;
    
public:
    DOMDocument() : DOMNode(DOMNodeType::DOCUMENT, "#document") {}
    
    // Document methods
    std::shared_ptr<DOMElement> CreateElement(const std::string& tag_name);
    std::shared_ptr<DOMText> CreateTextNode(const std::string& text);
    std::shared_ptr<DOMElement> GetElementById(const std::string& id);
    std::vector<std::shared_ptr<DOMElement>> GetElementsByTagName(const std::string& tag_name);
    std::vector<std::shared_ptr<DOMElement>> GetElementsByClassName(const std::string& class_name);
    std::shared_ptr<DOMNode> QuerySelector(const std::string& selector);
    std::vector<std::shared_ptr<DOMNode>> QuerySelectorAll(const std::string& selector);
    
    // Document properties
    std::shared_ptr<DOMNode> GetDocumentElement() const { return document_element; }
    void SetDocumentElement(std::shared_ptr<DOMNode> element) { document_element = element; }
    
    // CSS management
    void AddCSSRule(const CSSRule& rule);
    void RemoveCSSRule(const CSSRule& rule);
    const std::vector<CSSRule>& GetCSSRules() const { return css_rules; }
    void SetCSSVariable(const std::string& name, const std::string& value);
    std::string GetCSSVariable(const std::string& name) const;
    
    // Style computation
    ComputedStyle ComputeStyle(std::shared_ptr<DOMNode> node);
    void UpdateAllStyles();
    
    // Layout
    void ComputeLayout();
    void InvalidateLayout();
};

// HTML Parser
class HTMLParser {
private:
    std::string html_source;
    size_t current_position;
    std::shared_ptr<DOMDocument> document;
    std::vector<std::shared_ptr<DOMNode>> node_stack;
    
    // Token types
    enum class TokenType {
        START_TAG,
        END_TAG,
        SELF_CLOSING_TAG,
        TEXT,
        COMMENT,
        DOCTYPE,
        EOF_TOKEN
    };
    
    struct Token {
        TokenType type;
        std::string tag_name;
        std::map<std::string, std::string> attributes;
        std::string text;
        bool is_self_closing = false;
        
        Token(TokenType token_type) : type(token_type) {}
    };
    
public:
    HTMLParser();
    ~HTMLParser();
    
    // Parsing
    std::shared_ptr<DOMDocument> Parse(const std::string& html);
    std::shared_ptr<DOMDocument> ParseFile(const std::string& filename);
    
    // Error handling
    bool HasErrors() const;
    std::vector<std::string> GetErrors() const;
    
private:
    // Tokenization
    Token NextToken();
    Token ParseTag();
    Token ParseText();
    Token ParseComment();
    Token ParseDoctype();
    
    // Attribute parsing
    std::map<std::string, std::string> ParseAttributes(const std::string& tag_content);
    std::string ParseAttributeValue(const std::string& attribute_string);
    
    // Tree construction
    void ProcessToken(const Token& token);
    void InsertElement(std::shared_ptr<DOMElement> element);
    void InsertText(const std::string& text);
    void InsertComment(const std::string& comment);
    
    // Utility
    char PeekChar(size_t offset = 0);
    char GetChar();
    void Advance(size_t count = 1);
    void SkipWhitespace();
    bool IsWhitespace(char c);
    bool IsEOF();
    
    std::vector<std::string> parse_errors;
};

// CSS Parser
class CSSParser {
private:
    std::string css_source;
    size_t current_position;
    
public:
    CSSParser();
    ~CSSParser();
    
    // Parsing
    std::vector<CSSRule> Parse(const std::string& css);
    std::vector<CSSRule> ParseFile(const std::string& filename);
    CSSValue ParseValue(const std::string& value_string);
    
    // Error handling
    bool HasErrors() const;
    std::vector<std::string> GetErrors() const;
    
private:
    // Tokenization
    std::vector<std::string> ParseSelectors();
    std::vector<CSSProperty> ParseProperties();
    CSSProperty ParseProperty();
    
    // Value parsing
    CSSValue ParseLength(const std::string& value);
    CSSValue ParseColor(const std::string& value);
    CSSValue ParseString(const std::string& value);
    
    // Utility
    char PeekChar(size_t offset = 0);
    char GetChar();
    void Advance(size_t count = 1);
    void SkipWhitespace();
    bool IsWhitespace(char c);
    bool IsEOF();
    
    std::vector<std::string> parse_errors;
};

// Layout Engine
class LayoutEngine {
private:
    std::shared_ptr<DOMDocument> document;
    float viewport_width = 1280;
    float viewport_height = 720;
    
public:
    LayoutEngine();
    ~LayoutEngine();
    
    // Layout computation
    void ComputeLayout(std::shared_ptr<DOMDocument> doc);
    void SetViewportSize(float width, float height);
    
    // Layout boxes
    DOMNode::LayoutBox ComputeElementBox(std::shared_ptr<DOMElement> element);
    DOMNode::LayoutBox ComputeTextBox(std::shared_ptr<DOMText> text);
    
    // Layout algorithms
    void ComputeBlockLayout(std::shared_ptr<DOMElement> element);
    void ComputeInlineLayout(std::shared_ptr<DOMElement> element);
    void ComputeFlexLayout(std::shared_ptr<DOMElement> element);
    void ComputeGridLayout(std::shared_ptr<DOMElement> element);
    
private:
    // Box model calculations
    float ComputeWidth(std::shared_ptr<DOMElement> element);
    float ComputeHeight(std::shared_ptr<DOMElement> element);
    float ComputeMargin(std::shared_ptr<DOMElement> element, const std::string& direction);
    float ComputePadding(std::shared_ptr<DOMElement> element, const std::string& direction);
    float ComputeBorder(std::shared_ptr<DOMElement> element, const std::string& direction);
    
    // Positioning
    void ComputeAbsolutePosition(std::shared_ptr<DOMElement> element);
    void ComputeRelativePosition(std::shared_ptr<DOMElement> element);
    void ComputeFixedPosition(std::shared_ptr<DOMElement> element);
};

// Factory functions
namespace HTMLFactory {
    std::shared_ptr<HTMLParser> CreateHTMLParser();
    std::shared_ptr<CSSParser> CreateCSSParser();
    std::shared_ptr<LayoutEngine> CreateLayoutEngine();
    std::shared_ptr<DOMDocument> CreateDocument();
}

// Utility functions
namespace HTMLUtils {
    std::string EscapeHTML(const std::string& text);
    std::string UnescapeHTML(const std::string& text);
    std::string MinifyHTML(const std::string& html);
    std::string PrettifyHTML(const std::string& html);
    std::vector<std::string> SplitSelector(const std::string& selector);
    bool IsValidTag(const std::string& tag_name);
    bool IsValidAttribute(const std::string& attribute_name, const std::string& tag_name);
}

} // namespace ComperCore

#endif // COMPER_HTML_H
