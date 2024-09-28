#pragma once
#include "../State.h"

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
    PycronImage* test;
    Graphics* m_graphics;
    int m_charWidth, m_charHeight;
    int m_textWidth, m_textHeight;
    int m_horizontalLetterSpacing, m_verticalLetterSpacing;
    std::vector<char> m_characterBuffer;

    void Clear();
    void Text(const std::string& text, int x, int y);


};

