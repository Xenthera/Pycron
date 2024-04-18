#include <algorithm>
#include "pocketpy.h"
#include "raylib.h"

using namespace std;

const int virtualScreenWidth = 240;
const int virtualScreenHeight = 136;

int screenWidth = virtualScreenWidth * 4;
int screenHeight = virtualScreenHeight * 4;

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
    const float virtualRatio = (float)screenWidth/(float)virtualScreenWidth;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

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
            int monitor = GetCurrentMonitor();
            SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            calculateBounds(virtualScreenBounds, origin);
            ToggleFullscreen();
        }

        BeginTextureMode(virtualScreen);
            ClearBackground(GRAY);
            DrawText("Hello World", 5, 5, 5, RAYWHITE);
            DrawTexture(logo, 5, 20, WHITE);
            DrawLineBezier((Vector2){80, 20}, (Vector2){virtualScreenWidth - 5, virtualScreenHeight - 5}, 1, GREEN);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 15, GREEN);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 13, YELLOW);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 11, ORANGE);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 9, RED);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 7, PURPLE);
            DrawCircle(virtualScreenWidth / 2, virtualScreenHeight / 2, 5, BLUE);

        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(virtualScreen.texture, sourceRec, virtualScreenBounds, origin, 0.0f, WHITE);
        EndDrawing();
    }

    CloseWindow();

    delete vm;

    return 0;
}
