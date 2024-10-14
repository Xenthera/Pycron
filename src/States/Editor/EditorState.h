#pragma once

#include <regex>
#include "PythonTokenizer.h"
#include "../../State.h"
#include "../../Graphics/PycronImage.h"
#include "../../Pycron.h"
#include "../../Graphics/Rectangle.h"

class PycronImage;

class EditorState : public State{

public:
    EditorState(pkpy::VM* vm, Graphics* graphics);
    ~EditorState();

    void Draw() override;
    void OnEnter() override;
    void OnExit() override;
    void OnKeyPressed(int key) override;
    void OnCharPressed(char character) override;
    void OnMousePressed(int button) override;

private:
    pkpy::VM* m_vm;
    Graphics* m_graphics;
    PythonTokenizer* m_pythonTokenizer;

    // Editor images
    PycronImage* m_editorFrame;

    PycronImage* m_LineNumberDetailLeft;
    PycronImage* m_LineNumberDetailCenter;
    PycronImage* m_LineNumberDetailRight;

    // Size of the character in pixels
    uint8_t m_charWidth, m_charHeight;

    // Size of the editor in characters
    uint8_t m_width, m_height;

    uint8_t m_topLetterSpacing, m_bottomLetterSpacing, m_leftLetterSpacing, m_rightLetterSpacing;
    // Text Buffer
    std::vector<char> m_characterBuffer;
    std::vector<uint8_t> m_foregroundIndexBuffer;
    std::vector<uint8_t> m_backgroundIndexBuffer;

    // Text Data
    std::vector<std::string> m_text;
    std::vector<bool> m_dirtyFlags;

    // Theming
    uint8_t m_baseBackgroundColor;
    uint8_t m_shadowColor;
    uint8_t m_baseTextColor;
    uint8_t m_lineNumberBackgroundColor;
    uint8_t m_lineNumberTextColor;
    uint8_t m_unknownTextColor;
    uint8_t m_identifierTextColor;
    uint8_t m_keywordTextColor;
    uint8_t m_builtinTextColor;
    uint8_t m_numericalLiteralTextColor;
    uint8_t m_stringLiteralTextColor;
    uint8_t m_punctuationTextColor;
    uint8_t m_operatorTextColor;
    uint8_t m_commentTextColor;

    int m_cursorX;
    int m_cursorY;
    bool m_cursorVisible;
    float_t m_cursorBlinkTimer;
    float_t m_cursorBlinkInterval;


    int m_textWindowXOffset;
    int m_textWindowYOffset;

    int m_lineNumberWindowXOffset;
    int m_lineNumberWindowYOffset;

    int m_textWindowWidth;
    int m_textWindowHeight;

    pycron::Rectangle* m_textBounds;

    int m_scrollX;
    int m_scrollY;

    int m_lastFurthestColumn;

    bool m_dirty;

    bool m_drawShadows;

    void Clear();
    void Text(const std::string &text, int x, int y, int fg = 0, int bg = 63);

    void LoadStringToBuffer(const std::string& text);


};

