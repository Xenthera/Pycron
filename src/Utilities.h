#pragma once
#include "raylib.h"
#include "Utilities.h"
#include "Graphics/Rectangle.h"


namespace Utilities {
    Color ColorFromHex(int hexValue);
    bool RectContainsPoint(pycron::Rectangle* r, int x, int y);
}
