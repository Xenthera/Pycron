#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

class PixelFont {
private:
    int m_width;
    int m_height;
    std::string m_filePath;
    std::unordered_map<int, std::string> m_asciiTable;

    const int MISSING_CHAR = 1234567;

public:
    PixelFont(int width, int height, std::string path);

public:
    int GetWidth();
    int GetHeight();
    std::string GetCharData(int asciiValue);
    std::string Dump();
    void ReadFontFile();
};
