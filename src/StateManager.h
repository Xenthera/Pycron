#pragma once

#include "Pycron.h"
#include "Graphics/Graphics.h"
#include "States/GameState.h"
#include "States/EditorState.h"

class Pycron;
class Graphics;
class GameState;
class EditorState;

class StateManager {
private:
    enum StateType  {
        EDITOR,
        MENU,
        GAME
    };

    State* m_currentState;
    Pycron* m_pycron;
    GameState* m_gameState;
    EditorState* m_editorState;


public:
    explicit StateManager(Pycron* pycron);
    ~StateManager();

    void RequestStateChange(StateType state);

    void RequestLoadGame();
    void RequestRunGame();
    void RequestStopGame();

    void Draw(Graphics* graphics);

};


