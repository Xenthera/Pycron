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
    virtual void onKeyPressed(int key) = 0;
};
