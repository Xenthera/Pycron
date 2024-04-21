#pragma once
#include <fstream>
#include <algorithm>

#include "raylib.h"
#include "pocketpy.h"

class Pycron {

public:
    const int virtualScreenWidth = 360;
    const int virtualScreenHeight = 203;
    const int initialScale = 3;

    int startupScreenWidth = virtualScreenWidth * initialScale;
    int startupScreenHeight = virtualScreenHeight * initialScale;

    int screenWidth = startupScreenWidth;
    int screenHeight = startupScreenHeight;

    Rectangle virtualScreenBounds;
    Vector2 origin;
    std::vector<Color> palette;
    RenderTexture2D virtualScreen;
    Rectangle sourceRec;
    Texture2D mouse;

    bool shouldClose = false;

    pkpy::VM* vm;

    Pycron();
    ~Pycron();

    void calculateBounds();
    void StartGameLoop();
    Color ColorFromHex(int hexValue);
    void loadPalette(std::string& path);

    int mouseX();
    int mouseY();
};

