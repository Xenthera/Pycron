//
// Created by Bobby Lucero on 4/20/24.
//

#include <fstream>
#include "Graphics.h"
#include "../Utilities.h"
#include <raymath.h>


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
    m_virtualScreenLocalBounds = {0.0f, 0.0f, (float)m_virtualScreen.texture.width, (float)m_virtualScreen.texture.height };
    m_virtualScreenWindowBounds = {0.0f, 0.0f, (float)m_windowWidth, (float)m_windowHeight};

    m_virtualScreenImageBuffer = GenImageColor(m_screenWidth, m_screenHeight, BLACK);
    m_virtualScreenColorBuffer = {};

    for (int i = 0; i < screenWidth * screenHeight; ++i) {
        m_virtualScreenColorBuffer.push_back(GetRandomValue(0, 5));
    }

    calculateScreenPositionInWindow();
}

void Graphics::draw(StateManager* stateManager) {

    m_windowShouldClose = WindowShouldClose();

    if (IsWindowResized()) {
        m_windowWidth = GetScreenWidth();
        m_windowHeight = GetScreenHeight();
        calculateScreenPositionInWindow();
    }

    stateManager->Draw(this);
    copyBufferToGPU();
    renderVirtualScreen();
}

void Graphics::copyBufferToGPU() {
    Color* pixel_data = LoadImageColors(m_virtualScreenImageBuffer);
    for (int i = 0; i < m_screenWidth * m_screenHeight; ++i) {
        pixel_data[i] = GetColor(m_paletteByID[m_virtualScreenColorBuffer[i]]);
    }
    UpdateTexture(m_virtualScreen.texture, pixel_data);
    UnloadImageColors(pixel_data);
}

void Graphics::renderVirtualScreen() {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(m_virtualScreen.texture, m_virtualScreenLocalBounds, m_virtualScreenWindowBounds, m_origin, 0.0f, WHITE);
        DrawText(std::to_string(GetFPS()).c_str(), 10, 10, 30, YELLOW);

    EndDrawing();
}

void Graphics::loadPalette(std::string path) {
    std::ifstream paletteFile(path);
    std::string line;

    if(paletteFile.is_open()){
        int idx = 0;
        while(getline(paletteFile, line)){
            int color = stoi(line, nullptr, 16);
            //Palette.push_back();
            color = color << 8 | 0xFF;
            m_paletteByColor.insert({color, idx});
            m_paletteByID.insert({idx, color});
            idx++;
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
    vm->bind(vm->builtins, "clear(color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        int index = pkpy::py_cast<int>(vm, args[0]);
        Clear(index);
        return vm->None;
    });

    vm->bind(vm->builtins, "pixel(x: int, y: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[2]);
        this->Pixel(x, y, paletteIndex);
        return vm->None;
    });

    vm->bind(vm->builtins, "get_pixel(x: int, y: int) -> int", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);

        return pkpy::py_var(vm, this->GetPixel(x, y));
    });

    vm->bind(vm->builtins, "circle(x: int, y: int, radius: float, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float radius = pkpy::py_cast<float>(vm, args[2]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[3]);
        this->Circle(x, y, radius, paletteIndex);
        return vm->None;
    });

    vm->bind(vm->builtins, "rectangle(x: int, y: int, width: int, height: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float width = pkpy::py_cast<float>(vm, args[2]);
        float height = pkpy::py_cast<float>(vm, args[3]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[4]);
        this->Rect(x, y, width, height, paletteIndex);
        return vm->None;
    });

    vm->bind(vm->builtins, "triangle(x1: int, y1: int, x2: int, y2: int, x3: int, y3: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x1 = pkpy::py_cast<float>(vm, args[0]);
        float y1 = pkpy::py_cast<float>(vm, args[1]);
        float x2 = pkpy::py_cast<float>(vm, args[2]);
        float y2 = pkpy::py_cast<float>(vm, args[3]);
        float x3 = pkpy::py_cast<float>(vm, args[4]);
        float y3 = pkpy::py_cast<float>(vm, args[5]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[6]);
        this->Triangle(x1, y1, x2, y2, x3, y3, paletteIndex);
        return vm->None;
    });

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
    for (int y = 0; y < m_screenHeight; ++y) {
        for (int x = 0; x < m_screenWidth; ++x) {
            Pixel(x, y, paletteIndex);
        }
    }
}

void Graphics::Pixel(int x, int y, int paletteIndex) {
    paletteIndex = Clamp(paletteIndex, 0, m_paletteByID.size() - 1);
    if(x < 0 || y < 0 || x >= m_screenWidth || y >= m_screenHeight) return;

    m_virtualScreenColorBuffer[y * m_screenWidth + x] = paletteIndex;
}

void Graphics::Circle(int x, int y, int radius, int paletteIndex) {
    Ellipse(x, y, radius, radius, paletteIndex);
}

void Graphics::Ellipse(int x, int y, int w, int h, int paletteIndex){

    int x0 = x - w;
    int y0 = y - h;
    int x1 = x + w;
    int y1 = y + h;

    if(x0 > x1 || y0 > y1)
        return;

    int a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1;
    int dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
    int err = dx + dy + b1 * a * a, e2;

    if (x0 > x1) { x0 = x1; x1 += a; }
    if (y0 > y1) y0 = y1;
    y0 += (b + 1) / 2; y1 = y0 - b1;
    a *= 8 * a; b1 = 8 * b * b;

    int prevY = y0;
    do
    {
        Pixel( x1, y0, paletteIndex);
        Pixel( x0, y0, paletteIndex);
        Pixel( x0, y1, paletteIndex);
        Pixel( x1, y1, paletteIndex);
        if(y0 != prevY && (y0 - y) != h){
            h_line(x1, y0, x - (x1 - x) + 1, paletteIndex);
            h_line(x1, y - (y0 - y), x - (x1 - x) + 1, paletteIndex);
        }
        prevY = y0;
        e2 = 2 * err;
        if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
        if (e2 >= dx || 2 * err > dy) { x0++; x1--; err += dx += b1; } /* x step */
    } while (x0 <= x1);

    while (y0-y1 < b)
    {  /* too early stop of flat ellipses a=1 */
        Pixel( x0 - 1, y0,    paletteIndex); /* -> finish tip of ellipse */
        Pixel( x1 + 1, y0++,  paletteIndex);
        Pixel( x0 - 1, y1,    paletteIndex);
        Pixel( x1 + 1, y1--,  paletteIndex);
    }


    h_line(x - w, y, x + w, paletteIndex);
}

void Graphics::EllipseBorder(int x, int y, int w, int h, int paletteIndex){

    int x0 = x - w;
    int y0 = y - h;
    int x1 = x + w;
    int y1 = y + h;

    if(x0 > x1 || y0 > y1)
        return;

    int a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1;
    int dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;
    int err = dx + dy + b1 * a * a, e2;

    if (x0 > x1) { x0 = x1; x1 += a; }
    if (y0 > y1) y0 = y1;
    y0 += (b + 1) / 2; y1 = y0 - b1;
    a *= 8 * a; b1 = 8 * b * b;

    do
    {
        Pixel( x1, y0, paletteIndex);
        Pixel( x0, y0, paletteIndex);
        Pixel( x0, y1, paletteIndex);
        Pixel( x1, y1, paletteIndex);

        e2 = 2 * err;
        if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
        if (e2 >= dx || 2 * err > dy) { x0++; x1--; err += dx += b1; } /* x step */
    } while (x0 <= x1);

    while (y0-y1 < b)
    {  /* too early stop of flat ellipses a=1 */
        Pixel( x0 - 1, y0,    paletteIndex); /* -> finish tip of ellipse */
        Pixel( x1 + 1, y0++,  paletteIndex);
        Pixel( x0 - 1, y1,    paletteIndex);
        Pixel( x1 + 1, y1--,  paletteIndex);
    }

}




void Graphics::Rect(int x, int y, int width, int height, int paletteIndex) {
    for (int i = 0; i < height; ++i) {
        h_line(x, y + i, x + width - 1, paletteIndex);
    }
}

void Graphics::RectBorder(int x, int y, int width, int height, int paletteIndex) {
    h_line(x, y, x + width - 1, paletteIndex);
    h_line(x, y + height - 1, x + width - 1, paletteIndex);
    v_line(x, y + 1, y + height - 2, paletteIndex);
    v_line(x + width - 1, y + 1, y + height - 2, paletteIndex);
}

void Graphics::Text(std::string s, int x, int y, int paletteIndex) {

}

void Graphics::updateVMVars(pkpy::VM* vm) {
    vm->builtins->attr().set("mouseX", pkpy::py_var(vm, mouseX()));
    vm->builtins->attr().set("mouseY", pkpy::py_var(vm, mouseY()));
    vm->builtins->attr().set("width", pkpy::py_var(vm, m_screenWidth));
    vm->builtins->attr().set("height", pkpy::py_var(vm, m_screenHeight));
}

void Graphics::h_line(int x1, int y, int x2, int paletteIndex) {
    if(y < 0 || y >= m_screenHeight) return;
    int startX = x1;
    int endX = x2;
    if(x1 > x2){
        startX = x2;
        endX = x1;
    }
    for (int i = startX; i <= endX; ++i) {
        Pixel(i, y, paletteIndex);
    }
}

void Graphics::v_line(int x, int y1, int y2, int paletteIndex) {
    if(x < 0 || x >= m_screenWidth) return;
    int startY = y1;
    int endY = y2;
    if(y1 > y2){
        startY = y2;
        endY = y1;
    }
    for (int i = startY; i <= endY; ++i) {
        Pixel(x, i, paletteIndex);
    }
}

int Graphics::GetPixel(int x, int y) {
    if(x < 0 || y < 0 || x >= m_screenWidth || y >= m_screenHeight) return 0;

    return m_virtualScreenColorBuffer[y * m_screenWidth + x];
}

void Graphics::fillBottomFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int paletteIndex)
{
    float invslope1 = (x2 - x1) / (y2 - y1);
    float invslope2 = (x3 - x1) / (y3 - y1);

    float curx1 = x1;
    float curx2 = x1;

    for (int scanlineY = y1; scanlineY <= y2; scanlineY++)
    {
        h_line((int)curx1, scanlineY, (int)curx2, paletteIndex);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void Graphics::fillTopFlatTriangle(float x1, float y1, float x2, float y2, float x3, float y3, int paletteIndex)
{
    float invslope1 = (x3 - x1) / (y3 - y1);
    float invslope2 = (x3 - x2) / (y3 - y2);

    float curx1 = x3;
    float curx2 = x3;

    for (int scanlineY = y3; scanlineY > y1; scanlineY--)
    {
        h_line((int)curx1, scanlineY, (int)curx2, paletteIndex);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}


void Graphics::Triangle(int x1, int y1, int x2, int y2, int x3, int y3, int paletteIndex)
{
    int aX = x1;
    int aY = y1;
    int bX = x2;
    int bY = y2;
    int cX = x3;
    int cY = y3;

    if(bY < aY){
        int t = bY;
        bY = aY;
        aY = t;
        t = bX;
        bX = aX;
        aX = t;
    }

    if(cY < aY){
        int t = cY;
        cY = aY;
        aY = t;
        t = cX;
        cX = aX;
        aX = t;
    }

    if(cY < bY){
        int t = cY;
        cY = bY;
        bY = t;
        t = cX;
        cX = bX;
        bX = t;
    }

    //std::cout << aX << " " << aY << " " << bX << " " << bY << " " << cX << " " << cY << "\n";

    /* here we know that v1.y <= v2.y <= v3.y */
    /* check for trivial case of bottom-flat triangle */
    if (bY == cY)
    {
        fillBottomFlatTriangle(aX, aY, bX, bY, cX, cY, paletteIndex);
    }
        /* check for trivial case of top-flat triangle */
    else if (aY == bY)
    {
        fillTopFlatTriangle(aX, aY, bX, bY, cX, cY, paletteIndex);
    }
    else
    {
        /* general case - split the triangle in a topflat and bottom-flat one */
        int dX = aX + ((float)(bY - aY) / (float)(cY - aY)) * (cX - aX);
        int dY = bY;

        fillBottomFlatTriangle(aX, aY, bX, bY, dX, dY, paletteIndex);
        fillTopFlatTriangle(bX, bY, dX, dY, cX, cY, paletteIndex);
    }
}






