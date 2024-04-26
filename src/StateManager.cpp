//
// Created by Bobby Lucero on 4/25/24.
//

#include "StateManager.h"

StateManager::StateManager(Pycron *pycron) : m_pycron(pycron){
    gameState = new GameState(this);
    m_currentState = gameState;

}

void StateManager::RequestStateChange(StateManager::StateType state) {
    if(m_currentState){
        m_currentState->OnExit();
    }

    if(state == StateType::GAME){
        m_currentState = gameState;
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
