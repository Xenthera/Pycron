//
// Created by Bobby Lucero on 4/20/24.
//
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include "Pycron.h"
#include "Utilities.h"
#include "Graphics/Graphics.h"


std::string loadFileToString(const std::string& filename) {
    std::ifstream file(filename); // Open the file
    std::stringstream buffer; // String stream to hold file content
    if (file.is_open()) { // Check if file is open
        buffer << file.rdbuf(); // Read the entire file into the buffer
        file.close(); // Close the file
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
    return buffer.str(); // Return the content string
}

Pycron::Pycron() {
    SetTraceLogLevel(LOG_ERROR);

    graphics = new Graphics{virtualScreenWidth, virtualScreenHeight, initialScale};
    graphics->loadPalette("../resources/palette2.hex");

    stateManager = new StateManager(this);

    vm = new pkpy::VM();
    bindMethods(vm);

    graphics->bindMethods(vm);

    std::string python = loadFileToString("../python/main.py");

    graphics->beginDraw();
    graphics->Clear(0);
    try {
        pkpy::CodeObject_ code = vm->compile(python, "main.py", pkpy::EXEC_MODE, false);
        vm->_exec(code, vm->_main);
        graphics->searchForDrawFunc(vm);
    }catch (pkpy::Exception e) {
        std::cout << e.summary() << std::endl;
    }
    graphics->endDraw();

}

Pycron::~Pycron(){
    CloseWindow();
    delete vm;
    delete graphics;
}


void Pycron::StartGameLoop() {

    while (!graphics->windowShouldClose) {
        if (IsKeyPressed(KEY_F)) {
            graphics->toggleFullScreen();
        }
        graphics->draw(this->stateManager);
    }
}

void Pycron::bindMethods(pkpy::VM *vm) {
    vm->bind(vm->builtins, "rnd(min: int, max: int) -> int", getRandomNumber);
    vm->bind(vm->builtins, "sin(num: float) -> float", getSin);
    vm->bind(vm->builtins, "cos(num: float) -> float", getCos);
    vm->bind(vm->builtins, "fps() -> int", getFPS);
}


pkpy::PyObject* Pycron::getRandomNumber(pkpy::VM* vm, pkpy::ArgsView args) {
    int min = pkpy::py_cast<int>(vm, args[0]);
    int max = pkpy::py_cast<int>(vm, args[1]);
    // Seed the random number generator with a random device
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define a uniform distribution for the range [min, max]
    std::uniform_int_distribution<int> distribution(min, max);

    // Generate a random number within the specified range
    return pkpy::py_var(vm, distribution(gen));
}

pkpy::PyObject* Pycron::getSin(pkpy::VM* vm, pkpy::ArgsView args) {
    auto num = pkpy::py_cast<double>(vm, args[0]);
    return pkpy::py_var(vm, sin(num));
}

pkpy::PyObject* Pycron::getCos(pkpy::VM* vm, pkpy::ArgsView args) {
    auto num = pkpy::py_cast<double>(vm, args[0]);
    return pkpy::py_var(vm, cos(num));
}

pkpy::PyObject* Pycron::getFPS(pkpy::VM* vm, pkpy::ArgsView args) {
    return pkpy::py_var(vm, GetFPS());
}

