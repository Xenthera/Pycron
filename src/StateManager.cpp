//
// Created by Bobby Lucero on 4/25/24.
//

#include "StateManager.h"

StateManager::StateManager(Pycron *pycron) : m_pycron(pycron){
    m_gameState = new GameState(this, m_pycron->m_vm);
    RequestStateChange(GAME);
}

void StateManager::RequestStateChange(StateManager::StateType state) {
    if(m_currentState){
        m_currentState->OnExit();
    }

    if(state == StateType::GAME){
        m_currentState = m_gameState;
    }

    if(m_currentState){
        // Game state needs ability to draw during code loading
        if(state == GAME){
            m_pycron->m_graphics->beginDraw();
            m_currentState->OnEnter();
            m_pycron->m_graphics->endDraw();
        }else{
            m_currentState->OnEnter();
        }
    }
}

void StateManager::Draw(Graphics *graphics) {
    if(m_currentState){
        m_currentState->Draw(graphics);
    }
}

StateManager::~StateManager() {
    m_currentState = nullptr;
    delete m_gameState;
}
