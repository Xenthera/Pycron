#pragma once
#include "../State.h"
class GameState : public State  {

public:

    const std::string MAIN_FILE = "main.py";
    std::string m_previousError;
    bool m_errorThrown = false;

    GameState(StateManager* stateManager, pkpy::VM* vm);

    void Draw(Graphics* graphics) override;
    void OnEnter() override;
    void OnExit() override;
    void onKeyPressed(int key) override;

private:

    pkpy::VM* m_vm;
    pkpy::PyObject* m_updateFunction;


    std::unordered_map<std::string, std::string> readPythonFiles(const std::string& dir);
    void loadPythonModules(std::unordered_map<std::string, std::string>& fileContents);
    void PreProcessScripts();

};

