//
// Created by Bobby on 5/13/2024.
//

#include <vector>
#include <string>

#include "EditorState.h"
#include "../../Graphics/PycronImage.h"
#include "../../Pycron.h"
#include <raylib.h>


EditorState::EditorState(pkpy::VM *vm, Graphics *graphics) : m_vm(vm), m_graphics(graphics){
    m_pythonTokenizer = new PythonTokenizer();

    Token a(TokenType::Keyword, "Test");

    std::string randomSource = Pycron::loadFileToString("../python/main.py");

    m_baseBackgroundColor = 56;
    m_shadowColor = 28;
    m_baseTextColor = 42;
    m_lineNumberBackgroundColor = 57;
    m_lineNumberTextColor  = 6;
    m_unknownTextColor  = 4;
    m_identifierTextColor  = 63;
    m_keywordTextColor  = 31;
    m_builtinTextColor  = 23;
    m_numericalLiteralTextColor = 32;
    m_stringLiteralTextColor = 6;
    m_punctuationTextColor = 52;
    m_operatorTextColor = 45;
    m_commentTextColor = 60;

    m_topLetterSpacing = 0;
    m_bottomLetterSpacing = 1;
    m_leftLetterSpacing = 0;
    m_rightLetterSpacing = 1;

    m_cursorX = 0;
    m_cursorY = 0;

    m_cursorBlinkTimer = 0;
    m_cursorBlinkInterval = 0.5;

    m_charWidth = m_leftLetterSpacing + m_graphics->GetCurrentFontWidth() + m_rightLetterSpacing; // Final size of char with spacing. If the literal width of the font is n, the final width is n + spacing.
    m_charHeight = m_topLetterSpacing + m_graphics->GetCurrentFontHeight() + m_bottomLetterSpacing;

    m_width = (int)(m_graphics->m_screenWidth / m_charWidth);
    m_height = (int)(m_graphics->m_screenHeight / m_charHeight);

    m_characterBuffer = std::vector<char>(m_width * m_height);
    m_foregroundIndexBuffer = std::vector<uint8_t>(m_width * m_height);
    m_backgroundIndexBuffer = std::vector<uint8_t>(m_width * m_height);

    m_dirtyFlags = std::vector<bool>(m_height);

    m_drawShadows = true;

    Clear();

    LoadStringToBuffer(randomSource);

    auto tokens = m_pythonTokenizer->tokenizeLine("for i in range(100):");

}

EditorState::~EditorState() {
    delete m_pythonTokenizer;
}

void EditorState::Draw() {

    // Set text and color buffers if needed
    if(m_dirty){
        Clear();
        m_dirty = false;
        for (int i = 0; i < m_text.size(); ++i) {
            // Line numbers TODO: maybe not have this as part of the buffer, instead as a custom bar. (Allows for more custom functionality such as bookmarks)
//            std::string lineNumber = std::to_string(std::abs(m_cursorY - i));
//            if(i == m_cursorY) lineNumber = std::to_string(m_cursorY);
            std::string lineNumber = std::to_string(i);
            int size = 2;
            int diff = size - (int)lineNumber.size();
            if(diff > 0) lineNumber = std::string(diff, ' ') + lineNumber;
            Text(lineNumber, 0, i, i == m_cursorY ? m_lineNumberTextColor : m_commentTextColor, m_lineNumberBackgroundColor);

            // Text handling
            auto tokens = m_pythonTokenizer->tokenizeLine(m_text[i]);

            int currentPos = 0;
            for (int j = 0; j < tokens.size(); ++j) {
                int color = m_baseTextColor;
                TokenType type = tokens[j].type;
                if(type == TokenType::Identifier){
                    color = m_identifierTextColor;
                }else if(type == TokenType::Keyword){
                    color = m_keywordTextColor;
                }else if(type == TokenType::Builtin){
                    color = m_builtinTextColor;
                }else if(type == TokenType::NumericalLiteral){
                    color = m_numericalLiteralTextColor;
                }else if(type == TokenType::StringLiteral){
                    color = m_stringLiteralTextColor;
                }else if(type == TokenType::Punctuation){
                    color = m_punctuationTextColor;
                }else if(type == TokenType::Operator){
                    color = m_operatorTextColor;
                }else if(type == TokenType::Comment){
                    color = m_commentTextColor;
                }else if(type == TokenType::Unknown){
                    color = m_baseTextColor;
                }
                Text(tokens[j].value, size + currentPos, i, color, m_baseBackgroundColor);
                currentPos += tokens[j].value.size();
            }
        }
    }

    m_graphics->Clear(0);

    // draw text with info from color buffers
    for (int i = 0; i < m_characterBuffer.size(); ++i) {
        int x = (i % m_width) * m_charWidth;
        int y = (i / m_width) * m_charHeight;
        m_graphics->Rect(x, y, m_charWidth, m_charHeight, m_backgroundIndexBuffer[i]);
        if(m_drawShadows) m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing + 1, y + m_topLetterSpacing + 1, m_shadowColor);
        m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing, y + m_topLetterSpacing, m_foregroundIndexBuffer[i]);
    }


    if(m_cursorVisible) {
        int x = (m_cursorX + 2) * m_charWidth;
        int y = m_cursorY * m_charHeight;
        int index = m_cursorY * m_width + (m_cursorX + 2);

        if(m_drawShadows) m_graphics->Rect(x, y, m_charWidth + 1, m_charHeight + 1, m_shadowColor);
        m_graphics->Rect(x - 1, y - 1, m_charWidth + 1, m_charHeight + 1, m_foregroundIndexBuffer[index]);
//        m_graphics->Line(x - 1, y, x - 1, y + m_charHeight - 1, 63);
        std::string s(1, m_characterBuffer[index]);
        m_graphics->Text(s, x, y, m_backgroundIndexBuffer[index]);
    }

    // Cursor blink
    m_cursorBlinkTimer += GetFrameTime();
    if(m_cursorBlinkTimer > m_cursorBlinkInterval){
        m_cursorVisible = !m_cursorVisible;
        m_cursorBlinkTimer = 0.0;
    }

}

void EditorState::OnEnter() {

}

void EditorState::OnExit() {

}

void EditorState::OnKeyPressed(int key) {
    if(key == KEY_LEFT && m_cursorX > 0){
        m_cursorX--;
    }
    if(key == KEY_RIGHT && m_cursorX < m_width - 1 - 2){
        m_cursorX++;
    }
    if(key == KEY_UP && m_cursorY > 0){
        m_cursorY--;
    }
    if(key == KEY_DOWN && m_cursorY < m_height - 1){
        m_cursorY++;
    }

    m_cursorVisible = true;
    m_cursorBlinkTimer = 0;

    m_dirty = true;

    if(key == KEY_GRAVE) m_drawShadows = !m_drawShadows;
}

void EditorState::OnCharPressed(char character){

}

void EditorState::Clear() {
    for (int i = 0; i < m_width * m_height; ++i) {
        m_characterBuffer[i] = ' ';
        m_foregroundIndexBuffer[i] = m_baseTextColor;
        m_backgroundIndexBuffer[i] = m_baseBackgroundColor;
    }
}

void EditorState::Text(const std::string &text, int x, int y, int fg, int bg) {
    for (int i = 0; i < text.size(); ++i) {
        int charX = x + i;
        int charY = y;
        if(charX < 0 || charY < 0 || charX >= m_width || charY >= m_height){
            return;
        }
        char c = text[i];
        int index = y * m_width + (x + i);
        m_characterBuffer[index] = c;
        m_backgroundIndexBuffer[index] = bg;
        m_foregroundIndexBuffer[index] = fg;


    }
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




