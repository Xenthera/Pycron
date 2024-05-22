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
    void onKeyPressed(int key) override;

private:
    pkpy::VM* m_vm;
    PycronImage* test;
    Graphics* m_graphics;


};

