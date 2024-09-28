#pragma once

#include <vector>
#include <cstdint>

struct PycronImage {
    int width;
    int height;
    std::vector<int8_t> data;

    PycronImage(int width, int height);
};

