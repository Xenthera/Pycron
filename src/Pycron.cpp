//
// Created by Bobby Lucero on 4/20/24.
//
#include "Pycron.h"
#include "Utilities.h"
#include "Graphics/Graphics.h"

Pycron::Pycron() {
    SetTraceLogLevel(LOG_ERROR);

    graphics = new Graphics{virtualScreenWidth, virtualScreenHeight, initialScale};
    graphics->loadPalette((std::string &) "../resources/palette2.hex");

    vm = new pkpy::VM();

    try {
        pkpy::CodeObject_ code = vm->compile("return 'test'", "main.py", pkpy::EXEC_MODE, false);
        pkpy::PyObject* obj = vm->_exec(code, vm->_main);

        auto& str = pkpy::py_cast<pkpy::Str&>(vm, obj);
    }catch (pkpy::Exception e) {

    }

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
        graphics->draw();
    }
}