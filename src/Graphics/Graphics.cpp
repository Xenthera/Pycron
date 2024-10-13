//
// Created by Bobby Lucero on 4/20/24.
//

#include "Graphics.h"
#include "PycronImage.h"
#include "Font.h"
#include "../Utilities.h"
#include "../StateManager.h"

#include <fstream>
#include <raymath.h>




Graphics::Graphics(int screenWidth, int screenHeight, int startupScale) : m_screenWidth(screenWidth), m_screenHeight(screenHeight){

    m_NormalFont = new PixelFont(5, 7, "resources/fonts/main.font");
    m_currentFont = m_NormalFont;

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
    m_virtualScreenColorBuffer.resize(screenWidth * screenHeight);

    calculateScreenPositionInWindow();

}

Graphics::~Graphics() {
    delete m_NormalFont;
    m_NormalFont = nullptr;
    m_currentFont = nullptr;
}

void Graphics::draw(StateManager* stateManager) {
    m_windowShouldClose = WindowShouldClose();

    if (IsWindowResized()) {
        m_windowWidth = GetScreenWidth();
        m_windowHeight = GetScreenHeight();
        calculateScreenPositionInWindow();
    }

    stateManager->Draw();
    copyBufferToGPU();
    renderVirtualScreen();
}

void Graphics::copyBufferToGPU() {
    Color* pixel_data = LoadImageColors(m_virtualScreenImageBuffer);
    for (int i = 0; i < m_screenWidth * m_screenHeight; ++i) {
        uint32_t rgb = m_paletteByID[m_virtualScreenColorBuffer[i]];
        uint8_t r = rgb >> 16 & 0xFF;
        uint8_t g = rgb >> 8 & 0xFF;
        uint8_t b = rgb & 0xFF;

        double dR = (double)r * 0.2126;
        double dG = (double)g * 0.7152;
        double dB = (double)b * 0.0722;

        int gray = (int)(dR + dG + dB);
        double naiveGray = (r + g + b) / 3.0;
        //gray = (int)naiveGray;
        unsigned int out = 255;
        out |= gray << 8;
        out |= gray << 16;
        out |= gray << 24;
        pixel_data[i] = GetColor(rgb);
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

int Graphics::rgbToID(int r, int g, int b) {
    r = Clamp(r, 0, 255);
    g = Clamp(g, 0, 255);
    b = Clamp(b, 0, 255);
    uint8_t idx = 0;
    Color c = GetColor(m_paletteByID[idx]);
    double minDist = std::pow(r - c.r, 2) +
                     std::pow(g - c.g, 2) +
                     std::pow(b - c.b, 2);

    for(const auto& paletteColor : m_paletteByID){
        c = GetColor(paletteColor.second);
        double dist = std::pow(r - c.r, 2) +
                      std::pow(g - c.g, 2) +
                      std::pow(b - c.b, 2);
        if(dist < minDist){
            minDist = dist;
            idx = paletteColor.first;
        }
    }

    return idx;
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

    vm->bind(vm->builtins, "getPixel(x: int, y: int) -> int", [this](pkpy::VM* vm, pkpy::ArgsView args){
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

    vm->bind(vm->builtins, "rect(x: int, y: int, width: int, height: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float width = pkpy::py_cast<float>(vm, args[2]);
        float height = pkpy::py_cast<float>(vm, args[3]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[4]);
        this->Rect(x, y, width, height, paletteIndex);
        return vm->None;
    });

    vm->bind(vm->builtins, "rectBorder(x: int, y: int, width: int, height: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x = pkpy::py_cast<float>(vm, args[0]);
        float y = pkpy::py_cast<float>(vm, args[1]);
        float width = pkpy::py_cast<float>(vm, args[2]);
        float height = pkpy::py_cast<float>(vm, args[3]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[4]);
        this->RectBorder(x, y, width, height, paletteIndex);
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

    vm->bind(vm->builtins, "line(x1: int, y1: int, x2: int, y2: int, color: int)", [this](pkpy::VM* vm, pkpy::ArgsView args){
        float x1 = pkpy::py_cast<float>(vm, args[0]);
        float y1 = pkpy::py_cast<float>(vm, args[1]);
        float x2 = pkpy::py_cast<float>(vm, args[2]);
        float y2 = pkpy::py_cast<float>(vm, args[3]);
        float paletteIndex = pkpy::py_cast<float>(vm, args[4]);
        this->Line(x1, y1, x2, y2, paletteIndex);
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

void Graphics::Line(int x0, int y0, int x1, int y1, int paletteIndex) {
    int dx = x1 - x0; // Change in x
    int dy = y1 - y0; // Change in y
    int abs_dx = std::abs(dx);
    int abs_dy = std::abs(dy);

    int sx = (dx > 0) ? 1 : -1; // Step direction in x
    int sy = (dy > 0) ? 1 : -1; // Step direction in y

    // Choose the primary axis for iteration
    if (abs_dx > abs_dy) {
        int err = abs_dx / 2; // Error value
        for (int i = 0; i <= abs_dx; i++) {
            Pixel(x0, y0, paletteIndex); // Plot the pixel
            err -= abs_dy; // Update error term
            if (err < 0) {
                y0 += sy; // Move in y direction
                err += abs_dx; // Update error term
            }
            x0 += sx; // Always move in x direction
        }
    } else {
        int err = abs_dy / 2; // Error value
        for (int i = 0; i <= abs_dy; i++) {
            Pixel(x0, y0, paletteIndex); // Plot the pixel
            err -= abs_dx; // Update error term
            if (err < 0) {
                x0 += sx; // Move in x direction
                err += abs_dy; // Update error term
            }
            y0 += sy; // Always move in y direction
        }
    }
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

void Graphics::Text(const std::string& s, int x, int y, int paletteIndex) {
    int currentX = 0;
    int currentY = y;

    for (int i = 0; i < s.size(); ++i) {
        char c = s[i];
        // Handle new lines
        if(c != '\n') {
            std::string bitData = m_currentFont->GetCharData((int) c);
            for (int j = 0; j < bitData.size(); ++j) {
                if (bitData[j] == '1'){
                    Pixel(x + (j % m_currentFont->GetWidth()) + ((m_currentFont->GetWidth() + 1) * currentX),currentY + (j / m_currentFont->GetWidth()), paletteIndex);
                    }

            }
            currentX++;
        }else{
            currentX = 0;
            currentY += m_currentFont->GetHeight() + 1;
        }
    }
}

void Graphics::Char(char c, int x, int y, int paletteIndex) {
        std::string bitData = m_currentFont->GetCharData((int)c);
        for (int j = 0; j < bitData.size(); ++j) {
            if(bitData[j] == '1')
                Pixel(x + (j % m_currentFont->GetWidth()), y + (j / m_currentFont->GetWidth()), paletteIndex);

        }
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

// https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/olcPixelGameEngine.h#L2456
void Graphics::Triangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int paletteIndex)
{
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Pixel(i, ny, paletteIndex); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
    if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
    if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1);
    if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1);
    if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
    if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i < dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
        next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
        next2:
        if (minx > t1x) minx = t1x;
        if (minx > t2x) minx = t2x;
        if (maxx < t1x) maxx = t1x;
        if (maxx < t2x) maxx = t2x;
        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
        // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;
    }
    next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        std::swap(dy1, dx1);
        changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0; t2xp = 0;
        if (t1x < t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i < dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i < dx1) i++;
        }
        next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
        next4:

        if (minx > t1x) minx = t1x;
        if (minx > t2x) minx = t2x;
        if (maxx < t1x) maxx = t1x;
        if (maxx < t2x) maxx = t2x;
        drawline(minx, maxx, y);
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y > y3) return;
    }


}

PycronImage* Graphics::loadImage(std::string path) {
    Image img = LoadImage(path.c_str());
    ImageRotateCW(&img);
    ImageFlipHorizontal(&img);
    if(img.width == 0 && img.height == 0) std::cerr << "Image at path " << path << " is empty\n";
    Color* colors = LoadImageColors(img);
    auto* idImg = new PycronImage(img.width, img.height);
    for (int i = 0; i < idImg->data.size(); ++i) {
        if(colors[i].a == 0){
            idImg->data[i] = -1;
        }else{
            idImg->data[i] = (int8_t)rgbToID(colors[i].r, colors[i].g, colors[i].b);
        }
    }
    return idImg;
}

void Graphics::Img(PycronImage* img, int x, int y) {
    for (int i = 0; i < img->data.size(); ++i) {
        if(img->data[i] == -1) continue;
        Pixel(x + i / img->width, y + i % img->width, img->data[i]);
    }
}

int Graphics::GetCurrentFontWidth(){
    return m_currentFont->GetWidth();
}
int Graphics::GetCurrentFontHeight(){
    return m_currentFont->GetHeight();
}






