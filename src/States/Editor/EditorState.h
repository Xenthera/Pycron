#pragma once

#include <regex>
#include "PythonTokenizer.h"
#include "../../State.h"


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

private:
    pkpy::VM* m_vm;
    Graphics* m_graphics;
    PythonTokenizer* m_pythonTokenizer;

    uint8_t m_charWidth, m_charHeight;
    uint8_t m_textWidth, m_textHeight;
    uint8_t m_topLetterSpacing, m_bottomLetterSpacing, m_leftLetterSpacing, m_rightLetterSpacing;
    // Text Buffer
    std::vector<char> m_characterBuffer;
    std::vector<uint8_t> m_foregroundIndexBuffer;
    std::vector<uint8_t> m_backgroundIndexBuffer;

    // Text Data
    std::vector<std::string> m_text;

    // Theming
    uint8_t m_baseBackgroundColor;
    uint8_t m_baseTextColor;
    uint8_t m_lineNumberBackgroundColor;
    uint8_t m_lineNumberTextColor;

    bool m_dirty;

    void Clear();
    void Text(const std::string &text, int x, int y, int fg = 0, int bg = 63);

    void LoadStringToBuffer(const std::string& text);


};

