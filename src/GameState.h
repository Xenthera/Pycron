#pragma once
#include "State.h"
class GameState : public State  {

public:
    GameState(StateManager* stateManager);

    void Draw(Graphics* graphics) override;
    void OnEnter() override;
    void OnExit() override;
    void onKeyPressed(int key) override;
};

