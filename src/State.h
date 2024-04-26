#pragma once

#include "Graphics/Graphics.h"
#include "StateManager.h"

class Graphics;
class StateManager;

class State {
private:
    StateManager* m_stateManager;
protected:
    explicit State(StateManager* stateManager) : m_stateManager(stateManager){}
public:
    virtual ~State() = default;
    virtual void Draw(Graphics* graphics) = 0;
    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;
    virtual void onKeyPressed(int key) = 0;
};
