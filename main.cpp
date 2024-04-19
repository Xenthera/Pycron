#include <algorithm>
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

void calculateBounds(Rectangle& bounds, Vector2& origin) {

    float ar = (float)virtualScreenWidth / (float)virtualScreenHeight;
    float nar = (float)screenWidth / (float)screenHeight;

    if(nar > ar) {
        bounds.height = screenHeight;
        bounds.width = bounds.height * ar;
        origin.x = -(screenWidth / 2.0f - (bounds.width / 2.0f));
        origin.y = 0;
    }else {
        bounds.width = screenWidth;
        bounds.height = bounds.width / ar;
        origin.x = 0;
        origin.y = -(screenHeight / 2.0f - (bounds.height / 2.0f));
    }
}

int main() {

    SetTraceLogLevel(LOG_ERROR);

    bool flip = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(startupScreenWidth, startupScreenHeight, "test");
    SetTargetFPS(-1);

    RenderTexture2D virtualScreen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    Rectangle sourceRec = { 0.0f, 0.0f, (float)virtualScreen.texture.width, -(float)virtualScreen.texture.height };
    Rectangle virtualScreenBounds;
    Vector2 origin;
    calculateBounds(virtualScreenBounds, origin);

    pkpy::VM* vm = new pkpy::VM();

    Texture2D logo = LoadTexture("resources/img.png");

    try {
        pkpy::CodeObject_ code = vm->compile("return 'test'", "main.py", pkpy::EXEC_MODE, false);
        pkpy::PyObject* obj = vm->_exec(code, vm->_main);

        pkpy::Str& str = pkpy::py_cast<pkpy::Str&>(vm, obj);
    }catch (pkpy::Exception e) {

    }

    while(!WindowShouldClose()) {
        if (IsWindowResized() && !IsWindowFullscreen())
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            calculateBounds(virtualScreenBounds, origin);
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
            calculateBounds(virtualScreenBounds, origin);
        }else if(IsKeyPressed(KEY_I)){
            flip = !flip;
        }

        BeginTextureMode(virtualScreen);
            ClearBackground(BLUE);
            DrawText(("Hello World " + to_string(GetFPS()) + " FPS").c_str(), 5, 5, 5, RAYWHITE);
            DrawTexture(logo, 80 + cos(GetTime()) * (virtualScreenHeight / 4), (virtualScreenHeight / 2) + sin(GetTime()) * (virtualScreenHeight / 4) - 29, WHITE);
            DrawLineBezier((Vector2){80, 20}, (Vector2){virtualScreenWidth - 5, virtualScreenHeight - 5}, 1, GREEN);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 15, GREEN);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 13, YELLOW);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 11, ORANGE);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 9, RED);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 7, PURPLE);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 5, BLUE);

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
