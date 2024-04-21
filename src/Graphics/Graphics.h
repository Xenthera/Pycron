#pragma once
#include "raylib.h"
#include <algorithm>
#include <vector>

class Graphics {

private:
    // initial spawn size, used to exit full screen as well
    int startupScreenWidth;
    int startupScreenHeight;
    // current size
    int windowWidth;
    int windowHeight;

    Rectangle virtualScreenWindowBounds; // size of rect texture on window
    Vector2 origin; // position of rect texture on window
    Rectangle virtualScreenLocalBounds; // virtual screen bounds
    RenderTexture2D virtualScreen; // actual pixel screen

    std::vector<Color> palette;

private:
    void renderVirtualScreen();
    void calculateScreenPositionInWindow();

public:
    // virtual screen
    int screenWidth;
    int screenHeight;

    bool windowShouldClose;

public:
    Graphics(int screenWidth, int screenHeight, int startupScale);

    void draw();

    void loadPalette(std::string path);
    int mouseX();
    int mouseY();
    void toggleFullScreen();


};
