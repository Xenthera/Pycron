#pragma once
#include "raylib.h"
#include "pocketpy/vm.h"
#include "../StateManager.h"
#include <algorithm>
#include <vector>

class StateManager;

class Graphics {

private:
    // initial spawn size, used to exit full screen as well
    int m_startupScreenWidth;
    int m_startupScreenHeight;
    // current size
    int m_windowWidth;
    int m_windowHeight;

    Rectangle m_virtualScreenWindowBounds; // size of rect texture on window
    Vector2 m_origin; // position of rect texture on window
    Rectangle m_virtualScreenLocalBounds; // virtual screen bounds
    RenderTexture2D m_virtualScreen; // actual pixel screen


private:
    void renderVirtualScreen();
    void calculateScreenPositionInWindow();

public:
    // virtual screen
    int m_screenWidth = 0;
    int m_screenHeight = 0;

    bool m_windowShouldClose = false;

    static std::vector<Color> Palette;



public:
    Graphics(int screenWidth, int screenHeight, int startupScale);

    void draw(StateManager* stateManager);

    void beginDraw();
    void endDraw();

    void updateVMVars(pkpy::VM* vm);

    void loadPalette(std::string path);
    int mouseX();
    int mouseY();
    void toggleFullScreen();

    void bindMethods(pkpy::VM* vm);

    void Clear(int paletteIndex);
    void Pixel(int x, int y, int paletteIndex);
    void Circle(int x, int y, int radius, int paletteIndex);
    void Rect(int x, int y, int width, int height, int paletteIndex);
    void Text(std::string s, int x, int y, int paletteIndex);


};
