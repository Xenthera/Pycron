#pragma once
#include "raylib.h"
#include "pocketpy/vm.h"
#include "../StateManager.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <array>

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
    RenderTexture2D m_virtualScreen;
    std::vector<uint8_t> m_virtualScreenColorBuffer;
    Image m_virtualScreenImageBuffer;

    static void swap(float &a, float &b);


private:
    void renderVirtualScreen();
    void calculateScreenPositionInWindow();

    void h_line(int x1, int y, int x2, int paletteIndex);
    void v_line(int x, int y1, int y2, int paletteIndex);
    void fillBottomFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int paletteIndex);
    void fillTopFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int paletteIndex);
public:
    // virtual screen
    int m_screenWidth = 0;
    int m_screenHeight = 0;

    bool m_windowShouldClose = false;


    std::unordered_map<unsigned int, unsigned int> m_paletteByColor; // <hex color, id>
    std::unordered_map<unsigned int, unsigned int> m_paletteByID;    // <id, hex color>



public:
    Graphics(int screenWidth, int screenHeight, int startupScale);

    void draw(StateManager* stateManager);

    void copyBufferToGPU();

    void updateVMVars(pkpy::VM* vm);

    void loadPalette(std::string path);
    int mouseX();
    int mouseY();
    void toggleFullScreen();

    void bindMethods(pkpy::VM* vm);

    void Clear(int paletteIndex);
    void Pixel(int x, int y, int paletteIndex);
    void Circle(int x, int y, int radius, int paletteIndex);
    void Ellipse(int x, int y, int width, int height, int paletteIndex);
    void EllipseBorder(int x, int y, int width, int height, int paletteIndex);
    void Rect(int x, int y, int width, int height, int paletteIndex);
    void RectBorder(int x, int y, int width, int height, int paletteIndex);
    void Text(std::string s, int x, int y, int paletteIndex);
    void Triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int paletteIndex);

    int GetPixel(int x, int y);


};
