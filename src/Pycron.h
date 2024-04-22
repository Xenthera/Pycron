#pragma once
#include <algorithm>
#include "pocketpy.h"
#include "Graphics/Graphics.h"

class Pycron {

private:
    const int virtualScreenWidth = 360;
    const int virtualScreenHeight = 203;
    const int initialScale = 3;
    Graphics* graphics;
    pkpy::VM* vm;

public:
    Pycron();
    ~Pycron();

    void StartGameLoop();
    void bindMethods(pkpy::VM* vm);

    static pkpy::PyObject* getRandomNumber(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getSin(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getCos(pkpy::VM* vm, pkpy::ArgsView args);
    static pkpy::PyObject* getFPS(pkpy::VM* vm, pkpy::ArgsView args);
};

