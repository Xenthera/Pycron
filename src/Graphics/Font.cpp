//
// Created by Bobby on 5/10/2024.
//

#include "Font.h"
#include <nlohmann/json.hpp>
#include <utility>
#include <fstream>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

PixelFont::PixelFont(int width, int height, std::string path) : m_width(width), m_height(height), m_filePath(std::move(path)){
    std::cout << Dump() << std::endl;
    ReadFontFile();
}


int PixelFont::GetWidth() {
    return this->m_width;
}

int PixelFont::GetHeight() {
    return this->m_height;
}

std::string PixelFont::Dump() {
    json j;
    j["width"] = this->m_width;
    j["height"] = this->m_height;
    for(const auto& pair : this->m_asciiTable){
        j["asciiTable"][std::to_string(pair.first)] = pair.second;
    }
    return j.dump(4);
}


void PixelFont::ReadFontFile() {
    std::ifstream json_file(m_filePath);
    if(!json_file.is_open()){
        std::cerr << "Could not open font file: " << m_filePath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << json_file.rdbuf();
    json data = json::parse(buffer.str());

    this->m_width = data["width"].template get<int>();
    this->m_height = data["height"].template get<int>();
    json asciiJson = data["asciiTable"];

    for(auto it = asciiJson.begin(); it != asciiJson.end(); ++it){
        int key = std::stoi(it.key());
        m_asciiTable[key] = it.value();
    }
}


std::string PixelFont::GetCharData(int asciiValue) {
    if(!m_asciiTable.count(asciiValue)){
        return m_asciiTable[MISSING_CHAR];
    }

    return m_asciiTable[asciiValue];
}

