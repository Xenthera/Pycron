//
// Created by Bobby on 5/13/2024.
//

#include "EditorState.h"

EditorState::EditorState(pkpy::VM *vm) : m_vm(vm){

}

void EditorState::Draw(Graphics *graphics) {
    graphics->Clear(1);
    std::string text = "Editor State";
    graphics->Text(text, graphics->m_screenWidth / 2 - (text.size() / 2 * 6), graphics->m_screenHeight / 2 - (7 / 2), 5);
    for (int y = 0; y < 128; ++y) {
        for (int x = 0; x < 128; ++x) {
            int c = x;
            graphics->Pixel(x, y, graphics->rgbToID(x * 2, y * 2, graphics->mouseX()));
        }
    }
}

void EditorState::OnEnter() {

}

void EditorState::OnExit() {

}

void EditorState::onKeyPressed(int key) {

}
