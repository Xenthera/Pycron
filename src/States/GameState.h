#pragma once
#include "../State.h"

class GameState : public State {

public:

    const std::string MAIN_FILE = "main.py";
    std::string m_previousError;
    bool m_errorThrown = false;

    GameState(pkpy::VM* vm, Graphics* graphics);

    void Draw() override;
    void OnEnter() override;
    void OnExit() override;
    void OnKeyPressed(int key) override;
    void OnCharPressed(char character) override;


private:

    pkpy::VM* m_vm;
    pkpy::PyObject* m_updateFunction;
    Graphics* m_graphics;

    std::unordered_map<std::string, std::string> readPythonFiles(const std::string& dir);
    void loadPythonModules(std::unordered_map<std::string, std::string>& fileContents);
    void PreProcessScripts();

};

