//
// Created by Bobby Lucero on 4/20/24.
//

#include <random>
#include <cmath>
#include "Pycron.h"
#include "Utilities.h"
#include "Graphics/Graphics.h"

std::string Pycron::PythonDirectory = "./python";



Pycron::Pycron() {
    SetTraceLogLevel(LOG_ERROR);
    m_vm = new pkpy::VM();
    bindMethods();

    m_graphics = new Graphics{virtualScreenWidth, virtualScreenHeight, initialScale};
    m_graphics->loadPalette("resources/thecosmos.hex");
    m_graphics->bindMethods(m_vm);
    m_graphics->updateVMVars(m_vm);
    m_stateManager = new StateManager(this, m_graphics);



    std::string python = loadFileToString("../python/main.py");

}

Pycron::~Pycron(){
    CloseWindow();
    delete m_vm;
    delete m_graphics;
    delete m_stateManager;
}


void Pycron::StartGameLoop() {

    while (!m_graphics->m_windowShouldClose) {
        if (IsKeyPressed(KEY_F)) {
            m_graphics->toggleFullScreen();
        }
        m_graphics->updateVMVars(m_vm);
        m_graphics->draw(this->m_stateManager);

        char c = (char)GetCharPressed();

        while(c > 0){
            m_stateManager->OnCharPressed(c);
            c = (char)GetCharPressed();
        }

        for (int key = KeyboardKey::KEY_BACK; key < KeyboardKey::KEY_KP_EQUAL; ++key) {
            if(IsKeyPressed(key) || IsKeyPressedRepeat(key)){
                m_stateManager->OnKeyPressed(key);
            }
        }

        for (int button = MouseButton::MOUSE_BUTTON_LEFT; button < MouseButton::MOUSE_BUTTON_BACK; ++button) {
            if(IsMouseButtonPressed(button)){
                m_stateManager->OnMousePressed(button);
            }
        }
    }
}

void Pycron::bindMethods() {
    m_vm->bind(m_vm->builtins, "rnd(min: float, max: float) -> int", getRandomNumber);
    m_vm->bind(m_vm->builtins, "sin(num: float) -> float", getSin);
    m_vm->bind(m_vm->builtins, "cos(num: float) -> float", getCos);
    m_vm->bind(m_vm->builtins, "fps() -> int", getFPS);
    m_vm->bind(m_vm->builtins, "keyp(keycode: int) -> bool", getKeyPressed);
    m_vm->bind(m_vm->builtins, "key(keycode: int) -> bool", getKeyDown);
    m_vm->bind(m_vm->builtins, "mousep(button: int) -> bool", getMousePressed);
    m_vm->bind(m_vm->builtins, "mouse(button: int) -> bool", getMouseDown);
    m_vm->bind(m_vm->builtins, "debug(msg: any) -> None", [](pkpy::VM* vm, pkpy::ArgsView args){
        pkpy::PyObject* func_str = vm->builtins->attr("str");
        pkpy::PyObject* result = vm->call(func_str,  args[0]);
        std::string s = pkpy::py_cast<pkpy::Str&>(vm, result).str();

        std::cout << s.c_str() << "\n";
        return vm->None;
    });
    m_vm->bind(m_vm->builtins, "fmod(a: float, b: float) -> float", [](pkpy::VM* vm, pkpy::ArgsView args){
        float a = pkpy::py_cast<float>(vm, args[0]);
        float b = pkpy::py_cast<float>(vm, args[1]);

        float mod;
        // Handling negative values
        if (a < 0)
            mod = -a;
        else
            mod =  a;
        if (b < 0)
            b = -b;

        // Finding mod by repeated subtraction

        while (mod >= b)
            mod = mod - b;

        // Sign of result typically depends
        // on sign of a.
        if (a < 0)
            return pkpy::py_var(vm, -mod);

        return pkpy::py_var(vm, mod);
    });

}


pkpy::PyObject* Pycron::getRandomNumber(pkpy::VM* vm, pkpy::ArgsView args) {
    float min = pkpy::py_cast<float>(vm, args[0]);
    float max = pkpy::py_cast<float>(vm, args[1]);
    // Seed the random number generator with a random device
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Define a uniform distribution for the range [min, max]
    std::uniform_real_distribution<float> distribution(min, max);

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

pkpy::PyObject *Pycron::getKeyPressed(pkpy::VM *vm, pkpy::ArgsView args) {
    bool pressed = IsKeyPressed(pkpy::py_cast<int>(vm, args[0]));
    return pkpy::py_var(vm, pressed);
}

pkpy::PyObject *Pycron::getKeyDown(pkpy::VM *vm, pkpy::ArgsView args) {
    bool held = IsKeyDown(pkpy::py_cast<int>(vm, args[0]));
    return pkpy::py_var(vm, held);
}

pkpy::PyObject *Pycron::getMousePressed(pkpy::VM *vm, pkpy::ArgsView args) {
    bool held = IsMouseButtonPressed(pkpy::py_cast<int>(vm, args[0]));
    return pkpy::py_var(vm, held);
}

pkpy::PyObject *Pycron::getMouseDown(pkpy::VM *vm, pkpy::ArgsView args) {
    bool held = IsMouseButtonDown(pkpy::py_cast<int>(vm, args[0]));
    return pkpy::py_var(vm, held);
}

std::string Pycron::loadFileToString(const std::string &filename) {
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

