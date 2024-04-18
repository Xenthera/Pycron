cmake_minimum_required(VERSION 3.28)
project(UnnamedMicroPythonConsole)

set(CMAKE_CXX_STANDARD 17)

add_executable(UnnamedMicroPythonConsole main.cpp)

add_subdirectory(dependencies/pocketpy)
target_link_libraries(UnnamedMicroPythonConsole pocketpy)

if(EMSCRIPTEN)
    # exceptions must be enabled for emscripten
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fexceptions")
endif()
