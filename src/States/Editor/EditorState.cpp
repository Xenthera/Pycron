//
// Created by Bobby on 5/13/2024.
//

#include <vector>
#include <string>

#include "EditorState.h"
#include "../../Graphics/PycronImage.h"
#include "../../Pycron.h"


EditorState::EditorState(pkpy::VM *vm, Graphics *graphics) : m_vm(vm), m_graphics(graphics){
    m_pythonTokenizer = new PythonTokenizer();

    Token a(TokenType::Keyword, "Test");

    std::string randomSource = Pycron::loadFileToString("../python/main.py");

    m_baseBackgroundColor = 59;
    m_baseTextColor = 51;
    m_lineNumberBackgroundColor = 58;
    m_lineNumberTextColor  = 61;

    m_topLetterSpacing = 1;
    m_bottomLetterSpacing = 1;
    m_leftLetterSpacing = 1;
    m_rightLetterSpacing = 1;

    m_charWidth = m_leftLetterSpacing + m_graphics->GetCurrentFontWidth() + m_rightLetterSpacing; // Final size of char with spacing. If the literal width of the font is n, the final width is n + spacing.
    m_charHeight = m_topLetterSpacing + m_graphics->GetCurrentFontHeight() + m_bottomLetterSpacing;

    m_textWidth = (int)(m_graphics->m_screenWidth / m_charWidth);
    m_textHeight = (int)(m_graphics->m_screenHeight / m_charHeight);

    m_characterBuffer = std::vector<char>(m_textWidth * m_textHeight);
    m_foregroundIndexBuffer = std::vector<uint8_t>(m_textWidth * m_textHeight);
    m_backgroundIndexBuffer = std::vector<uint8_t>(m_textWidth * m_textHeight);

    Clear();


    std::string testText = "This is a test\n[Test]\nThis is the end of the test.";

    LoadStringToBuffer(randomSource);

}

EditorState::~EditorState() {
    delete m_pythonTokenizer;
}

void EditorState::Draw() {
    m_graphics->Clear(0);
    

    for (int i = 0; i < m_characterBuffer.size(); ++i) {
        int x = (i % m_textWidth) * m_charWidth;
        int y = (i / m_textWidth) * m_charHeight;
        m_graphics->Rect(x, y, m_charWidth, m_charHeight, m_backgroundIndexBuffer[i]);
        m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing + 1, y + m_topLetterSpacing + 1, 0);
        m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing, y + m_topLetterSpacing, m_foregroundIndexBuffer[i]);
    }

    if(m_dirty){
        Clear();
        m_dirty = false;
        for (int i = 0; i < m_text.size(); ++i) {
            std::string lineNumber = std::to_string(i);
            int size = 2;
            int diff = size - (int)lineNumber.size();
            if(diff > 0) lineNumber = std::string(diff, ' ') + lineNumber;
            Text(lineNumber, 0, i, m_lineNumberTextColor, m_lineNumberBackgroundColor);
            Text(m_text[i], size, i, m_baseTextColor, m_baseBackgroundColor);
        }
    }

}

void EditorState::OnEnter() {

}

void EditorState::OnExit() {

}

void EditorState::OnKeyPressed(int key) {
    std::cout << key << ". \n";
}

void EditorState::Clear() {
    for (int i = 0; i < m_textWidth * m_textHeight; ++i) {
        m_characterBuffer[i] = ' ';
        m_foregroundIndexBuffer[i] = m_baseTextColor;
        m_backgroundIndexBuffer[i] = m_baseBackgroundColor;
    }
}

void EditorState::Text(const std::string &text, int x, int y, int fg, int bg) {
    for (int i = 0; i < text.size(); ++i) {
        int charX = x + i;
        int charY = y;
        if(charX < 0 || charY < 0 || charX >= m_textWidth || charY >= m_textHeight){
            return;
        }
        char c = text[i];
        int index = y * m_textWidth + (x + i);
        m_characterBuffer[index] = c;
        m_backgroundIndexBuffer[index] = bg;
        m_foregroundIndexBuffer[index] = fg;


    }
}

void EditorState::OnCharPressed(char character) {

}

void EditorState::LoadStringToBuffer(const std::string &text) {
    m_text.clear();


    std::vector<std::string> lines;
    size_t start = 0;
    size_t end = 0;

    while((end = text.find('\n', start)) != std::string::npos){
        lines.push_back(text.substr(start, end - start));
        start = end + 1;
    }

    lines.push_back(text.substr(start));

    for (const auto& line : lines) {
        m_text.push_back(line);
    }
    
    m_dirty = true;
}




