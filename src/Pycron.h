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

};

