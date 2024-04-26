#pragma once

#include "State.h"
#include "Pycron.h"
#include "Graphics/Graphics.h"
#include "GameState.h"

class Pycron;
class State;
class Graphics;
class GameState;

class StateManager {
private:
    enum StateType  {
        EDITOR,
        MENU,
        GAME
    };

    State* m_currentState;
    Pycron* m_pycron;

    GameState* gameState;


public:
    explicit StateManager(Pycron* pycron);

    void RequestStateChange(StateType state);

    void RequestLoadGame();
    void RequestRunGame();
    void RequestStopGame();

    void Draw(Graphics* graphics);

};


