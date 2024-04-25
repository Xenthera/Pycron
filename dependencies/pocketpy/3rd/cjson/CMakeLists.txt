cmake_minimum_required(VERSION 3.10)

project(cjson)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(../../include)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(
    cjson
    STATIC
        src/cJSON.c
        src/cJSONw.cpp
)
