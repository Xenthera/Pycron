#pragma once
#include <algorithm>
#include <fstream>
#include <sstream>
#include "pocketpy.h"
#include "StateManager.h"
#include "Graphics/Graphics.h"

class Graphics;
class StateManager;

class Pycron {
public:
    static std::string PythonDirectory;

private:
    const int virtualScreenWidth = 360;
    const int virtualScreenHeight = 203;
    const int initialScale = 3;
    StateManager* m_stateManager;

public:

    pkpy::VM* m_vm;
    Graphics* m_graphics;


    Pycron();
    ~Pycron();

    void StartGameLoop();
    void bindMethods();

    static pkpy::PyObject* getRandomNumber(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getSin(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getCos(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getFPS(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getKeyPressed(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getKeyDown(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getMousePressed(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getMouseDown(pkpy::VM* vm, pkpy::ArgsView args);

    static std::string loadFileToString(const std::string& filename);
};

