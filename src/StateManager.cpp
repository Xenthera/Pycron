//
// Created by Bobby Lucero on 4/25/24.
//

#include "StateManager.h"

StateManager::StateManager(Pycron *pycron) : m_pycron(pycron){
    m_gameState = new GameState(this);
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
        m_currentState->OnEnter();
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
