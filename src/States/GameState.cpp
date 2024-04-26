//
// Created by Bobby Lucero on 4/25/24.
//

#include "GameState.h"

GameState::GameState(StateManager *stateManager) : State(stateManager) {

}


void GameState::Draw(Graphics *graphics) {
    graphics->Text("Test", 10, 10, 10);
}

void GameState::OnEnter() {
    //TODO: Python preprocess scripts
}

void GameState::OnExit() {

}

void GameState::onKeyPressed(int key) {

}

