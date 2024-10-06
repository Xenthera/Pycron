#pragma once


#include "Graphics/Graphics.h"

class State {
protected:
    State(){}
public:
    virtual ~State() = default;
    virtual void Draw() = 0;
    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;
    virtual void OnKeyPressed(int key) = 0;
    virtual void OnCharPressed(char character) = 0;
    virtual void OnMousePressed(int button) = 0;

};
