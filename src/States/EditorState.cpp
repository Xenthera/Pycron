//
// Created by Bobby on 5/13/2024.
//

#include "EditorState.h"
#include "../Graphics/PycronImage.h"


EditorState::EditorState(pkpy::VM *vm, Graphics *graphics) : m_vm(vm), m_graphics(graphics){
    test = m_graphics->loadImage("resources/plane2.png");

    m_horizontalLetterSpacing = 1;
    m_verticalLetterSpacing = 1;

    m_charWidth = m_graphics->GetCurrentFontWidth() + m_horizontalLetterSpacing; // Final size of char with spacing. If the literal width of the font is n, the final width is n + spacing.
    m_charHeight = m_graphics->GetCurrentFontHeight() + m_verticalLetterSpacing;

    m_textWidth = (int)(m_graphics->m_screenWidth / m_charWidth);
    m_textHeight = (int)(m_graphics->m_screenHeight / m_charHeight);

    m_characterBuffer = std::vector<char>(m_textWidth * m_textHeight);

    Clear();

}

EditorState::~EditorState() {
    delete test;
}

void EditorState::Draw() {
    m_graphics->Clear(1);
    std::string text = "Editor State";

    for (int i = 0; i < m_characterBuffer.size(); ++i) {
        m_graphics->Char(m_characterBuffer[i], (i % m_textWidth) * m_charWidth, (i / m_textWidth) * m_charHeight, 5);
    }
    Clear();
    Text("This is a big long test!!!@%)(!*@#%PALSKDGJ", ((float)m_graphics->mouseX() / m_graphics->m_screenWidth) * m_textWidth, ((float)m_graphics->mouseY() / m_graphics->m_screenHeight) * m_textHeight);

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
    }
}

void EditorState::Text(const std::string &text, int x, int y) {
    for (int i = 0; i < text.size(); ++i) {
        int charX = x + i;
        int charY = y;
        if(charX < 0 || charY < 0 || charX >= m_textWidth || charY >= m_textHeight){
            return;
        }
        char c = text[i];
        m_characterBuffer[y * m_textWidth + (x + i)] = c;
    }
}

void EditorState::OnCharPressed(char character) {

}


