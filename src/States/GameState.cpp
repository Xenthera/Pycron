//
// Created by Bobby Lucero on 4/25/24.
//

#include "GameState.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../Pycron.h"

GameState::GameState(pkpy::VM* vm, Graphics* graphics) :m_vm(vm), m_graphics(graphics){
    m_updateFunction = nullptr;
    m_previousError = "";
}

void GameState::Draw() {
        try{
            if(m_updateFunction != nullptr){
                m_vm->call(m_updateFunction);
            }
        } catch(pkpy::Exception e){
            m_previousError = e.summary();
            std::cout << e.summary() << "\n";
            m_errorThrown = true;
        }
}

void GameState::OnEnter() {
    PreProcessScripts();
}

void GameState::OnExit() {
    m_vm->_lazy_modules.clear();
    m_updateFunction = m_vm->None;
}

void GameState::onKeyPressed(int key) {

}

void GameState::PreProcessScripts() {
    m_previousError = "";
    m_errorThrown = false;
    std::unordered_map<std::string, std::string> pythonSources = readPythonFiles(Pycron::PythonDirectory);
    loadPythonModules(pythonSources);
    std::string main = pythonSources[MAIN_FILE];

    try{
        pkpy::CodeObject_ code = m_vm->compile(main, MAIN_FILE, pkpy::EXEC_MODE, false);
        m_vm->_exec(code, m_vm->_main);
        m_updateFunction = m_vm->eval("update");
//        if(m_updateFunction == nullptr){
//            m_previousError = "";
//        }
    }catch(pkpy::Exception e){
        m_previousError = e.summary();
        std::cout << e.summary() << "\n";
        m_errorThrown = true;
    }
}

std::unordered_map<std::string, std::string> GameState::readPythonFiles(const std::string &dir) {
    std::unordered_map<std::string, std::string> fileContents;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            std::filesystem::path path(entry.path());
            std::string fileName = path.filename().string();
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                fileContents[fileName] = buffer.str();
                file.close();
            } else {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
            }
        }
    }

    return fileContents;
}

void GameState::loadPythonModules(std::unordered_map<std::string, std::string> &fileContents) {
    for(const auto& pair : fileContents){
        try{
            if(pair.first != MAIN_FILE){
                size_t pos = pair.first.find_last_of(".");
                if(pos == std::string::npos || pos == 0){
                    throw pkpy::Exception("Invalid file name");
                }
                std::string moduleName = pair.first.substr(0, pos);
                m_vm->_lazy_modules[moduleName.c_str()] = pair.second;
            }
        }catch(pkpy::Exception e){
            std::cout << e.summary() << std::endl;
        }
    }
}

