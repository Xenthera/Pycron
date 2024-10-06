//
// Created by Bobby Lucero on 4/25/24.
//

#include "StateManager.h"

StateManager::StateManager(Pycron *pycron, Graphics *graphics) : m_pycron(pycron){
    m_gameState = new GameState(m_pycron->m_vm, graphics);
    m_editorState = new EditorState(m_pycron->m_vm, graphics);
    m_currentState = nullptr;
    ChangeState(EDITOR);
}

StateManager::~StateManager() {
    m_currentState = nullptr;
    delete m_gameState;
    delete m_editorState;
}


void StateManager::ChangeState(StateManager::StateType state) {
    if(m_currentState){
        m_currentState->OnExit();
    }

    if(state == StateType::GAME){
        m_currentState = m_gameState;
    }else if(state == StateType::EDITOR){
        m_currentState = m_editorState;
    }

    if(m_currentState){
        if(m_currentState == m_gameState){
            m_currentState->OnEnter();
            if(m_gameState->m_errorThrown){
                m_pycron->m_graphics->Text(m_gameState->m_previousError, 2, 2, 59);
            }
        }else{
            m_currentState->OnEnter();
        }
    }
}

void StateManager::Draw() {

    if(IsKeyPressed(KEY_ENTER)){
        if(m_currentState == m_gameState){
            ChangeState(EDITOR);
        }else{
            ChangeState(GAME);
        }
    }

    if(m_currentState){
        if(m_currentState == m_gameState){
            if(m_gameState->m_errorThrown){
                m_pycron->m_graphics->Text(m_gameState->m_previousError, 2, 2, 59);
            }
            else{
                m_currentState->Draw();
            }
        }
        else{
            m_currentState->Draw();
        }
    }
}

void StateManager::OnKeyPressed(int key) {
    m_currentState->OnKeyPressed(key);
}

void StateManager::OnCharPressed(char c) {
    m_currentState->OnCharPressed(c);
}

void StateManager::OnMousePressed(int button) {
    m_currentState->OnMousePressed(button);
}

