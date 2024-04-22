//
// Created by Bobby Lucero on 4/20/24.
//

#include <fstream>
#include "Graphics.h"
#include "../Utilities.h"

std::vector<Color> Graphics::palette;


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
    updateFunction = nullptr;
    calculateScreenPositionInWindow();

    std::cout << origin.x << " : " << origin.y << std::endl;
    std::cout << virtualScreenLocalBounds.width << " : " << virtualScreenLocalBounds.height << std::endl;
    std::cout << virtualScreenWindowBounds.width << " : " << virtualScreenWindowBounds.height << std::endl;


}

void Graphics::draw(pkpy::VM* vm) {

    vm->builtins->attr().set("mouseX", pkpy::py_var(vm, mouseX()));
    vm->builtins->attr().set("mouseY", pkpy::py_var(vm, mouseY()));

    windowShouldClose = WindowShouldClose();

    if (IsWindowResized()) {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();
        calculateScreenPositionInWindow();
    }

    BeginTextureMode(virtualScreen);
    //////////
        try{
            if(updateFunction != nullptr)
                vm->call(updateFunction);
        } catch(pkpy::Exception e){
            std::cout << e.summary() << std::endl;
        }

        Circle(150,100,50,1);
        Text("Hello from C++", 120, 95, 9);
    //////////
    EndTextureMode();

    renderVirtualScreen();
}

void Graphics::renderVirtualScreen() {
    BeginDrawing();
        //ClearBackground(palette[16]);
        DrawTexturePro(virtualScreen.texture, virtualScreenLocalBounds, virtualScreenWindowBounds, origin, 0.0f, WHITE);
    EndDrawing();
}

void Graphics::loadPalette(std::string path) {
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
        windowWidth = GetMonitorWidth(monitor);
        windowHeight = GetMonitorHeight(monitor);
        SetWindowSize(windowWidth, windowHeight);
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

    vm->bind(vm->builtins, "text(t: string, x: int, y: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){{
        std::string s = pkpy::py_cast<pkpy::Str&>(vm, args[0]).str();
        float x = pkpy::py_cast<float>(vm, args[1]);
        float y = pkpy::py_cast<float>(vm, args[2]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[3]);
        this->Text(s, x, y, paletteIndex);
        return vm->None;
    }});
}

void Graphics::Clear(int paletteIndex) {
    if(paletteIndex < 0 || paletteIndex >= palette.size()) paletteIndex = 0;
    ClearBackground(palette[paletteIndex]);
}

void Graphics::Pixel(int x, int y, int paletteIndex) {
    DrawPixel(x, y, palette[paletteIndex]);
}

void Graphics::Circle(int x, int y, int radius, int paletteIndex) {
    DrawCircle(x, y, radius, palette[paletteIndex]);
}

void Graphics::Text(std::string s, int x, int y, int paletteIndex) {
    DrawText(s.c_str(), x, y, 5, palette[paletteIndex]);
}

void Graphics::searchForDrawFunc(pkpy::VM* vm) {
    updateFunction = vm->eval("update");
    if(updateFunction == nullptr){
        std::cout << "Can't find update function" << std::endl;
    }
}

void Graphics::beginDraw() {
    BeginTextureMode(virtualScreen);
}

void Graphics::endDraw() {
    EndTextureMode();
}




