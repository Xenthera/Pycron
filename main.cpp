#include <algorithm>
#include <fstream>
#include "pocketpy.h"
#include "raylib.h"

using namespace std;

const int virtualScreenWidth = 360;
const int virtualScreenHeight = 203;
const int initialScale = 3;

int startupScreenWidth = virtualScreenWidth * initialScale;
int startupScreenHeight = virtualScreenHeight * initialScale;

int screenWidth = startupScreenWidth;
int screenHeight = startupScreenHeight;

Rectangle virtualScreenBounds;
Vector2 origin;

Color ColorFromHex(int hexValue) {
    // Extract red, green, blue, and alpha components from the hexadecimal value
    int r = (hexValue >> 16) & 0xFF;
    int g = (hexValue >> 8) & 0xFF;
    int b = hexValue & 0xFF;

    // Normalize the color components to the range [0, 1]
    float rf = static_cast<float>(r) / 255.0f;
    float gf = static_cast<float>(g) / 255.0f;
    float bf = static_cast<float>(b) / 255.0f;

    // Create and return the color
    return ColorFromNormalized({ rf, gf, bf, 1.0f }); // Alpha is set to 1.0 (fully opaque)
}

void calculateBounds() {

    float ar = (float)virtualScreenWidth / (float)virtualScreenHeight;
    float nar = (float)screenWidth / (float)screenHeight;

    if(nar > ar) {
        virtualScreenBounds.height = screenHeight;
        virtualScreenBounds.width = virtualScreenBounds.height * ar;
        origin.x = -(screenWidth / 2.0f - (virtualScreenBounds.width / 2.0f));
        origin.y = 0;
    }else {
        virtualScreenBounds.width = screenWidth;
        virtualScreenBounds.height = virtualScreenBounds.width / ar;
        origin.x = 0;
        origin.y = -(screenHeight / 2.0f - (virtualScreenBounds.height / 2.0f));
    }
}

int mouseX(){
    int x = GetMouseX();
    float adjX = x + origin.x;
    return adjX / virtualScreenBounds.width * virtualScreenWidth;
}

int mouseY(){
    int y = GetMouseY();
    float adjY = y + origin.y;
    return adjY / virtualScreenBounds.height * virtualScreenHeight;
}

int main() {

    SetTraceLogLevel(LOG_ERROR);

    bool flip = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(startupScreenWidth, startupScreenHeight, "test");
    SetTargetFPS(60);

    RenderTexture2D virtualScreen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    Rectangle sourceRec = { 0.0f, 0.0f, (float)virtualScreen.texture.width, -(float)virtualScreen.texture.height };
    calculateBounds();

    pkpy::VM* vm = new pkpy::VM();

    Texture2D logo = LoadTexture("resources/img.png");
    Texture2D mouse = LoadTexture("resources/mouse.png");

    // Load palette
    vector<Color> palette;

    ifstream paletteFile("resources/palette2.hex");
    string line;

    if(paletteFile.is_open()){
        while(getline(paletteFile, line)){
            palette.push_back(ColorFromHex(stoi(line, nullptr, 16)));
        }
        paletteFile.close();
    }

    try {
        pkpy::CodeObject_ code = vm->compile("return 'test'", "main.py", pkpy::EXEC_MODE, false);
        pkpy::PyObject* obj = vm->_exec(code, vm->_main);

        pkpy::Str& str = pkpy::py_cast<pkpy::Str&>(vm, obj);
    }catch (pkpy::Exception e) {

    }

    HideCursor();

    while(!WindowShouldClose()) {


        if (IsWindowResized() && !IsWindowFullscreen())
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            calculateBounds();
        }

        if (IsKeyPressed(KEY_F)) {
            if(IsWindowFullscreen()){
                ToggleFullscreen();
                SetWindowSize(startupScreenWidth, startupScreenHeight);
                screenWidth = startupScreenWidth;
                screenHeight = startupScreenHeight;
            }else{
                int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                screenWidth = GetMonitorWidth(monitor);
                screenHeight = GetMonitorHeight(monitor);
                ToggleFullscreen();
            }
            calculateBounds();
        }else if(IsKeyPressed(KEY_I)){
            flip = !flip;
        }

        BeginTextureMode(virtualScreen);
            ClearBackground(palette[1]);
            DrawText(("Hello World " + to_string(GetFPS()) + " FPS").c_str(), 5, 5, 5, RAYWHITE);
            DrawTexture(logo, 80 + cos(GetTime()) * (virtualScreenHeight / 4), (virtualScreenHeight / 2) + sin(GetTime()) * (virtualScreenHeight / 4) - 29, WHITE);
            DrawLineBezier((Vector2){80, 20}, (Vector2){virtualScreenWidth - 5, virtualScreenHeight - 5}, 1, GREEN);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 15, palette[31]);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 13, palette[28]);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 11, palette[16]);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 9, palette[15]);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 7, palette[51]);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 5, palette[47]);

            DrawRectangle(3, 19, 33, 33, BLACK);

            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    DrawRectangle(4 + i * 4, 20 + j * 4, 3, 3, palette[i + j * 8]);
                }
            }

        DrawTexture(mouse, mouseX(), mouseY(), WHITE);
        DrawText(to_string(mouseX()).c_str(), 2, 70, 5, palette[37]);
        DrawText(to_string(mouseY()).c_str(), 2, 78, 5, palette[27]);

        EndTextureMode();

        if(flip){
            // Get texture data
            Image img = LoadImageFromTexture(virtualScreen.texture);
            Color* pixels = LoadImageColors(img);

            // Modify pixel data
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    // Example: Invert colors
                    Color pixel = pixels[y * img.width + x];
                    pixels[y * img.width + x] = { (unsigned char)(255 - pixel.g), (unsigned char)(255 - pixel.r), (unsigned char)(255 - pixel.b), pixel.a };
                }
            }

            // Update texture with modified data
            UpdateTexture(virtualScreen.texture, pixels);
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(virtualScreen.texture, sourceRec, virtualScreenBounds, origin, 0.0f, WHITE);
        EndDrawing();
    }

    CloseWindow();

    delete vm;

    return 0;
}
