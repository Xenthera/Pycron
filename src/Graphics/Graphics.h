#pragma once
#include "raylib.h"
#include "pocketpy/vm.h"
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


private:
    void renderVirtualScreen();
    void calculateScreenPositionInWindow();

public:
    // virtual screen
    int screenWidth;
    int screenHeight;

    bool windowShouldClose;

    static std::vector<Color> palette;

    pkpy::PyObject* updateFunction;


public:
    Graphics(int screenWidth, int screenHeight, int startupScale);

    void draw(pkpy::VM* vm);

    void beginDraw();
    void endDraw();

    void loadPalette(std::string path);
    int mouseX();
    int mouseY();
    void toggleFullScreen();

    void bindMethods(pkpy::VM* vm);

    void searchForDrawFunc(pkpy::VM* vm);


    static void Clear(pkpy::VM* vm, pkpy::ArgsView args);
    static void Pixel(pkpy::VM* vm, pkpy::ArgsView args);
    static void Circle(pkpy::VM* vm, pkpy::ArgsView args);


};
