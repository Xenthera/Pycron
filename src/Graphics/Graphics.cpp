//
// Created by Bobby Lucero on 4/20/24.
//

#include <fstream>
#include "Graphics.h"
#include "../Utilities.h"

std::vector<Color> Graphics::Palette;


Graphics::Graphics(int screenWidth, int screenHeight, int startupScale) : m_screenWidth(screenWidth), m_screenHeight(screenHeight){
    m_startupScreenWidth = screenWidth * startupScale;
    m_startupScreenHeight = screenHeight * startupScale;
    m_windowWidth = m_startupScreenWidth;
    m_windowHeight = m_startupScreenHeight;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(m_startupScreenWidth, m_startupScreenHeight, "test");
    SetTargetFPS(60);
    m_virtualScreen = LoadRenderTexture(screenWidth, screenHeight);
    m_origin = {0,0};
    m_virtualScreenLocalBounds = {0.0f, 0.0f, (float)m_virtualScreen.texture.width, -(float)m_virtualScreen.texture.height };
    m_virtualScreenWindowBounds = {0.0f, 0.0f, (float)m_windowWidth, (float)m_windowHeight};
    calculateScreenPositionInWindow();
}

void Graphics::draw(StateManager* stateManager) {

    m_windowShouldClose = WindowShouldClose();

    if (IsWindowResized()) {
        m_windowWidth = GetScreenWidth();
        m_windowHeight = GetScreenHeight();
        calculateScreenPositionInWindow();
    }

    BeginTextureMode(m_virtualScreen);

    stateManager->Draw(this);

    EndTextureMode();

    renderVirtualScreen();
}

void Graphics::renderVirtualScreen() {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(m_virtualScreen.texture, m_virtualScreenLocalBounds, m_virtualScreenWindowBounds, m_origin, 0.0f, WHITE);
    EndDrawing();
}

void Graphics::loadPalette(std::string path) {
    std::ifstream paletteFile(path);
    std::string line;

    if(paletteFile.is_open()){
        while(getline(paletteFile, line)){
            Palette.push_back(Utilities::ColorFromHex(stoi(line, nullptr, 16)));
        }
        paletteFile.close();
    }
}

void Graphics::calculateScreenPositionInWindow() {
    float virtualAspectRatio = (float)m_screenWidth / (float)m_screenHeight;
    float windowAspectRatio = (float)m_windowWidth / (float)m_windowHeight;

    if(windowAspectRatio > virtualAspectRatio) {
        m_virtualScreenWindowBounds.height = (float)m_windowHeight;
        m_virtualScreenWindowBounds.width = m_virtualScreenWindowBounds.height * virtualAspectRatio;
        m_origin.x = -(m_windowWidth / 2.0f - (m_virtualScreenWindowBounds.width / 2.0f));
        m_origin.y = 0;
    }else {
        m_virtualScreenWindowBounds.width = (float)m_windowWidth;
        m_virtualScreenWindowBounds.height = m_virtualScreenWindowBounds.width / virtualAspectRatio;
        m_origin.x = 0;
        m_origin.y = -(m_windowHeight / 2.0f - (m_virtualScreenWindowBounds.height / 2.0f));
    }
}

int Graphics::mouseX() {
    float x = GetMouseX();
    float adjX = x + m_origin.x;
    return (int)(adjX / m_virtualScreenWindowBounds.width * m_screenWidth);
}

int Graphics::mouseY() {
    float y = GetMouseY();
    float adjY = y + m_origin.y;
    return (int)(adjY / m_virtualScreenWindowBounds.height * m_screenHeight);
}

void Graphics::toggleFullScreen() {
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
        SetWindowSize(m_startupScreenWidth, m_startupScreenHeight);
        m_windowWidth = m_startupScreenWidth;
        m_windowHeight = m_startupScreenHeight;
    } else {
        int monitor = GetCurrentMonitor();
        m_windowWidth = GetMonitorWidth(monitor);
        m_windowHeight = GetMonitorHeight(monitor);
        SetWindowSize(m_windowWidth, m_windowHeight);
        ToggleFullscreen();

    }
    calculateScreenPositionInWindow();
}

void Graphics::bindMethods(pkpy::VM *vm) {
//    vm->bind(vm->builtins, "clear(color: int)", reinterpret_cast<pkpy::NativeFuncC>(Clear));
//    vm->bind(vm->builtins, "pixel(x: int, y: int, color: int)", reinterpret_cast<pkpy::NativeFuncC>(Pixel));
//    vm->bind(vm->builtins, "circle(x: int, y: int, radius: float, color: int)", reinterpret_cast<pkpy::NativeFuncC>(Circle));

    vm->bind(vm->builtins, "clear(color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){{
        int index = pkpy::py_cast<int>(vm, args[0]);
        Clear(index);
        return vm->None;
    }});

    vm->bind(vm->builtins, "pixel(x: int, y: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){{
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[2]);
        this->Pixel(x, y, paletteIndex);
        return vm->None;
    }});

    vm->bind(vm->builtins, "circle(x: int, y: int, radius: float, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){{
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float radius = pkpy::py_cast<float>(vm, args[2]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[3]);
        this->Circle(x, y, radius, paletteIndex);
        return vm->None;
    }});

    vm->bind(vm->builtins, "rectangle(x: int, y: int, width: int, height: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){{
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float width = pkpy::py_cast<float>(vm, args[2]);
        float height = pkpy::py_cast<float>(vm, args[3]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[4]);
        this->Rect(x, y, width, height, paletteIndex);
        return vm->None;
    }});

    vm->bind(vm->builtins, "text(t: string, x: int, y: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        pkpy::PyObject* func_str = vm->builtins->attr("str");
        pkpy::PyObject* result = vm->call(func_str,  args[0]);
        std::string s = pkpy::py_cast<pkpy::Str&>(vm, result).str();

        float x = pkpy::py_cast<float>(vm, args[1]);
        float y = pkpy::py_cast<float>(vm, args[2]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[3]);
        this->Text(s, x, y, paletteIndex);
        return vm->None;
    });
}

void Graphics::Clear(int paletteIndex) {
    if(paletteIndex < 0 || paletteIndex >= Palette.size()) paletteIndex = 0;
    ClearBackground(Palette[paletteIndex]);
}

void Graphics::Pixel(int x, int y, int paletteIndex) {
    DrawPixel(x, y, Palette[paletteIndex]);
}

void Graphics::Circle(int x, int y, int radius, int paletteIndex) {
    DrawCircle(x, y, radius, Palette[paletteIndex]);
}

void Graphics::Rect(int x, int y, int width, int height, int paletteIndex) {
    DrawRectangle(x, y, width, height, Palette[paletteIndex]);
}

void Graphics::Text(std::string s, int x, int y, int paletteIndex) {
    DrawText(s.c_str(), x, y, 5, Palette[paletteIndex]);
}

void Graphics::beginDraw() {
    BeginTextureMode(m_virtualScreen);
}

void Graphics::endDraw() {
    EndTextureMode();
}

void Graphics::updateVMVars(pkpy::VM* vm) {
    vm->builtins->attr().set("mouseX", pkpy::py_var(vm, mouseX()));
    vm->builtins->attr().set("mouseY", pkpy::py_var(vm, mouseY()));
    vm->builtins->attr().set("width", pkpy::py_var(vm, m_screenWidth));
    vm->builtins->attr().set("height", pkpy::py_var(vm, m_screenHeight));
}




