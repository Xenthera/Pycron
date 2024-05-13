#pragma once
#include "../State.h"

class EditorState : public State{

public:
    EditorState(pkpy::VM* vm);

    void Draw(Graphics* graphics) override;
    void OnEnter() override;
    void OnExit() override;
    void onKeyPressed(int key) override;

private:
    pkpy::VM* m_vm;


};

