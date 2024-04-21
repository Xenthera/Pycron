//
// Created by Bobby Lucero on 4/20/24.
//

#include "Utilities.h"
namespace Utilities {
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
}
