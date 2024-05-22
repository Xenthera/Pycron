//
// Created by Bobby on 5/13/2024.
//

#include "EditorState.h"
#include "../Graphics/PycronImage.h"

EditorState::EditorState(pkpy::VM *vm, Graphics *graphics) : m_vm(vm), m_graphics(graphics){
    test = m_graphics->loadImage("resources/plane2.png");
}

EditorState::~EditorState() {
    delete test;
}

void EditorState::Draw() {
    m_graphics->Clear(1);
    std::string text = "Editor State";
    m_graphics->Text(text, m_graphics->m_screenWidth / 2 - (text.size() / 2 * 6), m_graphics->m_screenHeight / 2 - (7 / 2), 5);
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            int c = x;
            m_graphics->Pixel(x, y, m_graphics->rgbToID(x * 2, y * 2, m_graphics->mouseX()));
        }
    }
    std::cout << test->width << "\n";
    m_graphics->Img(test, m_graphics->mouseX() - (test->height / 2), m_graphics->mouseY() - (test->width / 2));
}

void EditorState::OnEnter() {

}

void EditorState::OnExit() {

}

void EditorState::onKeyPressed(int key) {

}
