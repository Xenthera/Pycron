//
// Created by Bobby Lucero on 4/20/24.
//

#include <fstream>
#include "Graphics.h"
#include "../Utilities.h"

Graphics::Graphics(int screenWidth, int screenHeight, int startupScale) : screenWidth(screenWidth), screenHeight(screenHeight){
    startupScreenWidth = screenWidth * startupScale;
    startupScreenHeight = screenHeight * startupScale;
    windowWidth = startupScreenWidth;
    windowHeight = startupScreenHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(startupScreenWidth, startupScreenHeight, "test");
    SetTargetFPS(60);
    virtualScreen = LoadRenderTexture(screenWidth, screenHeight);
    virtualScreenLocalBounds = {0.0f, 0.0f, (float)virtualScreen.texture.width, -(float)virtualScreen.texture.height };
    calculateScreenPositionInWindow();
}

void Graphics::draw() {

    windowShouldClose = WindowShouldClose();

    if (IsWindowResized()) {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        calculateScreenPositionInWindow();
    }

    BeginTextureMode(virtualScreen);
    //////////
        ClearBackground(palette[1]);
        DrawText(("Hello World " + std::to_string(GetFPS()) + " FPS").c_str(), 5, 5, 5, RAYWHITE);

        DrawRectangle(3, 19, 33, 33, BLACK);

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                DrawRectangle(4 + i * 4, 20 + j * 4, 3, 3, palette[i + j * 8]);
            }
        }

        DrawCircleLines(mouseX(), mouseY(),3, palette[18]);
    //////////
    EndTextureMode();

    renderVirtualScreen();
}

void Graphics::renderVirtualScreen() {
    BeginDrawing();
        ClearBackground(palette[0]);
        DrawTexturePro(virtualScreen.texture, virtualScreenLocalBounds, virtualScreenWindowBounds, origin, 0.0f, WHITE);
    EndDrawing();
}

void Graphics::loadPalette(std::string &path) {
    std::ifstream paletteFile(path);
    std::string line;

    if(paletteFile.is_open()){
        while(getline(paletteFile, line)){
            palette.push_back(Utilities::ColorFromHex(stoi(line, nullptr, 16)));
        }
        paletteFile.close();
    }
}

void Graphics::calculateScreenPositionInWindow() {
    float virtualAspectRatio = (float)screenWidth / (float)screenHeight;
    float windowAspectRatio = (float)windowWidth / (float)windowHeight;

    if(windowAspectRatio > virtualAspectRatio) {
        virtualScreenWindowBounds.height = (float)windowHeight;
        virtualScreenWindowBounds.width = virtualScreenWindowBounds.height * virtualAspectRatio;
        origin.x = -(windowWidth / 2.0f - (virtualScreenWindowBounds.width / 2.0f));
        origin.y = 0;
    }else {
        virtualScreenWindowBounds.width = (float)windowWidth;
        virtualScreenWindowBounds.height = virtualScreenWindowBounds.width / virtualAspectRatio;
        origin.x = 0;
        origin.y = -(windowHeight / 2.0f - (virtualScreenWindowBounds.height / 2.0f));
    }
}

int Graphics::mouseX() {
    float x = GetMouseX();
    float adjX = x + origin.x;
    return (int)(adjX / virtualScreenWindowBounds.width * screenWidth);
}

int Graphics::mouseY() {
    float y = GetMouseY();
    float adjY = y + origin.y;
    return (int)(adjY / virtualScreenWindowBounds.height * screenHeight);
}

void Graphics::toggleFullScreen() {
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
        SetWindowSize(startupScreenWidth, startupScreenHeight);
        windowWidth = startupScreenWidth;
        windowHeight = startupScreenHeight;
    } else {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        windowWidth = GetMonitorWidth(monitor);
        windowHeight = GetMonitorHeight(monitor);
        ToggleFullscreen();
    }
    calculateScreenPositionInWindow();
}




