//
// Created by Bobby on 5/13/2024.
//

#include <vector>
#include <string>
#include <algorithm>
#include <raylib.h>
#include "EditorState.h"
#include "../../Utilities.h"



EditorState::EditorState(pkpy::VM *vm, Graphics *graphics) : m_vm(vm), m_graphics(graphics){
    m_pythonTokenizer = new PythonTokenizer();

    std::string randomSource = Pycron::loadFileToString("../python/particles.py");

    m_editorFrame = m_graphics->loadImage("../resources/EditorFrame.png");
    m_LineNumberDetailLeft = m_graphics->loadImage("../resources/LineNumberDetailLeft.png");
    m_LineNumberDetailCenter = m_graphics->loadImage("../resources/LineNumberDetailCenter.png");
    m_LineNumberDetailRight = m_graphics->loadImage("../resources/LineNumberDetailRight.png");

    m_baseBackgroundColor = 56;
    m_shadowColor = 28;
    m_baseTextColor = 42;
    m_lineNumberBackgroundColor = 57;
    m_lineNumberTextColor  = 7;
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

    m_charWidth = m_leftLetterSpacing + m_graphics->GetCurrentFontWidth() + m_rightLetterSpacing; // Final size of char with spacing. If the literal width of the font is n, the final width is n + spacing.
    m_charHeight = m_topLetterSpacing + m_graphics->GetCurrentFontHeight() + m_bottomLetterSpacing;

    // Hardcoded text editor element positions based on interface graphics
    m_textWindowXOffset = 26;
    m_textWindowYOffset = 20;

    m_lineNumberWindowXOffset = 3;
    m_lineNumberWindowYOffset = 20;

    m_textWindowWidth = 330;
    m_textWindowHeight = 168;

    m_textBounds = new pycron::Rectangle(m_textWindowXOffset, m_textWindowYOffset, m_textWindowWidth, m_textWindowHeight);

    m_scrollX = 0;
    m_scrollY = 0;

    m_cursorBlinkTimer = 0;
    m_cursorBlinkInterval = 0.5;

    m_width = (int)(m_textBounds->width / m_charWidth);
    m_height = (int)(m_textBounds->height / m_charHeight);

    m_characterBuffer = std::vector<char>(m_width * m_height);
    m_foregroundIndexBuffer = std::vector<uint8_t>(m_width * m_height);
    m_backgroundIndexBuffer = std::vector<uint8_t>(m_width * m_height);

    m_dirtyFlags = std::vector<bool>(m_height);

    m_drawShadows = true;

    Clear();

    LoadStringToBuffer(randomSource);

}

EditorState::~EditorState() {
    delete m_pythonTokenizer;
    delete m_editorFrame;
    delete m_textBounds;
}

void EditorState::Draw() {
    // Set text and color buffers if needed
    if(m_dirty){
        Clear();
        m_dirty = false;

        int cursorPos = m_scrollY + m_cursorY;

        for (int i = 0; i < m_height; ++i) {

            int index = i + m_scrollY;

            if(index > m_text.size() - 1) break;

            // Text handling
            auto tokens = m_pythonTokenizer->tokenizeLine(m_text[index]);

            int currentPos = 0;
            if(index == cursorPos){
                std::string lineNumber = std::to_string(cursorPos);
                int offset = std::max((int)lineNumber.size() - 3, 0);
                bool collision = false;
                for (int j = 0; j < offset; ++j) {
                    if(m_text[index].size() < offset) break;
                    if(m_text[index][j] != ' '){
                        collision = true;
                        break;
                    }
                }
                if(!collision) offset = 0;
                currentPos = offset;
            }



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
                Text(tokens[j].value, currentPos, i, color, m_baseBackgroundColor);
                currentPos += (int)tokens[j].value.size();
            }
        }
    }

    m_graphics->Clear(57);

    // draw text with info from color buffers
    for (int i = 0; i < m_characterBuffer.size(); ++i) {
        int x = (i % m_width) * m_charWidth + m_textBounds->x;
        int y = (i / m_width) * m_charHeight + m_textBounds->y;
        m_graphics->Rect(x, y, m_charWidth, m_charHeight, m_backgroundIndexBuffer[i]);
        if(m_drawShadows) m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing + 1, y + m_topLetterSpacing + 1, m_shadowColor);
        m_graphics->Char(m_characterBuffer[i], x + m_leftLetterSpacing, y + m_topLetterSpacing, m_foregroundIndexBuffer[i]);
    }


    // Draw the cursor
    if(m_cursorVisible) {
        int currentLine = m_scrollY + m_cursorY;
        std::string lineNumber = std::to_string(currentLine);
        int offset = std::max((int)lineNumber.size() - 3, 0);

        bool collision = false;
        for (int i = 0; i < offset; ++i) {
            if(m_text[currentLine].size() < offset) break;
            if(m_text[currentLine][i] != ' '){
                collision = true;
                break;
            }
        }

        if(!collision) offset = 0;

        int x = (m_cursorX + offset) * m_charWidth + m_textBounds->x;
        int y = m_cursorY * m_charHeight + m_textBounds->y;

        int index = m_cursorY * m_width + (m_cursorX + offset);

        if(m_drawShadows) m_graphics->Rect(x, y, m_charWidth + 1, m_charHeight + 1, m_shadowColor);
        m_graphics->Rect(x - 1, y - 1, m_charWidth + 1, m_charHeight + 1, m_foregroundIndexBuffer[index]);
        std::string s(1, m_characterBuffer[index]);
        m_graphics->Text(s, x, y, m_backgroundIndexBuffer[index]);
    }

    // Cursor blink timer TODO: probably move to an update function
    m_cursorBlinkTimer += GetFrameTime();
    if(m_cursorBlinkTimer > m_cursorBlinkInterval){
        m_cursorVisible = !m_cursorVisible;
        m_cursorBlinkTimer = 0.0;
    }
    // Editor frame image
    m_graphics->Img(m_editorFrame, 0, 0);

    // Line Numbers
    for (int i = 0; i < m_height; ++i) {
        int lineNum = i + m_scrollY + 1;
        std::string lineNumber = std::to_string(lineNum);
        int delta = std::max(0, 3 - (int)lineNumber.size());
        lineNumber = std::string(delta, ' ') + lineNumber;
        lineNumber = lineNumber.substr(lineNumber.size() - 3);

        int highlight = i == m_cursorY ? m_lineNumberTextColor : m_commentTextColor;

        if(lineNum > 999){
            lineNumber[0] = ' ';
            int dotX = m_lineNumberWindowXOffset;
            int dotY = m_lineNumberWindowYOffset + (i * m_charHeight) + m_graphics->GetCurrentFontHeight() - 1;
            m_graphics->Pixel(dotX, dotY, highlight);
            m_graphics->Pixel(dotX + 2, dotY, highlight);
            m_graphics->Pixel(dotX + 4, dotY, highlight);
        }

        m_graphics->Text(lineNumber, m_lineNumberWindowXOffset, m_lineNumberWindowYOffset + (i * m_charHeight), highlight);
    }

    // Line Number Detail
    if(m_scrollY + 1 + m_cursorY > 999){
        int lineNum = m_scrollY + 1 + m_cursorY;
        std::string lineNumber = std::to_string(lineNum);

        int lineNumberDetailXOffset = 3;
        int lineNumberDetailYOffset = 3;

        int yOffset = m_lineNumberWindowYOffset - lineNumberDetailYOffset + (m_cursorY * (m_graphics->GetCurrentFontHeight() + 1));

        m_graphics->Img(m_LineNumberDetailLeft, m_lineNumberWindowXOffset - lineNumberDetailXOffset, yOffset);
        for (int i = 0; i < lineNumber.size(); ++i) {
            int offset = m_lineNumberWindowXOffset + (i * 6);
            m_graphics->Img(m_LineNumberDetailCenter, offset, yOffset);
        }
        m_graphics->Img(m_LineNumberDetailRight, m_lineNumberWindowXOffset + (lineNumber.size() * 6), yOffset);
        m_graphics->Text(lineNumber, m_lineNumberWindowXOffset, yOffset + lineNumberDetailYOffset, m_lineNumberTextColor);
    }


    //m_graphics->RectBorder(m_textBounds->x, m_textBounds->y, m_textBounds->width, m_textBounds->height, 23);

}

void EditorState::OnEnter() {

}

void EditorState::OnExit() {

}

void EditorState::OnKeyPressed(int key) {
    if(key == KEY_LEFT && m_cursorX > 0){
        m_cursorX--;
    }
    if(key == KEY_RIGHT && m_cursorX < m_width - 1){
        m_cursorX++;
    }
    if(key == KEY_UP){
        if(IsKeyDown(KEY_LEFT_SHIFT)){
            m_scrollY -= m_height;
            bool success = m_scrollY > 0;
            if(!success && m_cursorY > 0) m_cursorY--;
        }else{
            if( m_cursorY > 0)
            {
                m_cursorY--;
            }
            else
            {
                m_scrollY--;
            }
        }

        if(m_scrollY < 0){
            m_scrollY = 0;
        }

    }

    if(key == KEY_DOWN )
    {
        if(IsKeyDown(KEY_LEFT_SHIFT)){
            m_scrollY += m_height;
            bool success = m_scrollY + m_height <= m_text.size() - 1;
            if(!success && m_cursorY < std::min(m_height - 1, static_cast<int>(m_text.size()) - 1)) m_cursorY++;
        }else{
            if(m_cursorY < std::min(m_height - 1, static_cast<int>(m_text.size()) - 1))
            {
                m_cursorY++;
            }
            else
            {
                m_scrollY++;
            }
        }

        if(m_scrollY + m_height > m_text.size() - 1){
            m_scrollY = m_text.size() - 1 - m_height;
        }

    }


    m_cursorVisible = true;
    m_cursorBlinkTimer = 0;

    m_dirty = true;

    if(key == KEY_GRAVE) m_drawShadows = !m_drawShadows;
}

void EditorState::OnCharPressed(char character){

}

void EditorState::OnMousePressed(int button) {
    int x = m_graphics->mouseX();
    int y = m_graphics->mouseY();

    if(Utilities::RectContainsPoint(m_textBounds, x, y)){
        x -= m_textWindowXOffset;
        y -= m_textWindowYOffset;

        x /= m_charWidth;
        y /= m_charHeight;

        m_cursorX = x;
        m_cursorY = y;

        m_cursorVisible = true;
        m_cursorBlinkTimer = 0;
        m_dirty = true;
    }

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




