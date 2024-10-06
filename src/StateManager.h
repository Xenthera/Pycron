#pragma once

#include "Pycron.h"
#include "Graphics/Graphics.h"
#include "States/GameState.h"
#include "States/Editor/EditorState.h"

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
    Graphics* m_graphics;


public:
    explicit StateManager(Pycron* pycron, Graphics* graphics);
    ~StateManager();

    void ChangeState(StateManager::StateType state);

    void OnKeyPressed(int key);
    void OnCharPressed(char c);
    void OnMousePressed(int button);

    void RequestLoadGame();
    void RequestRunGame();
    void RequestStopGame();

    void Draw();

};


