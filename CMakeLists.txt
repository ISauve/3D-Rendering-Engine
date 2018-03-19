cmake_minimum_required(VERSION 3.9)
project(openGL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SHARED_LINKER_FLAGS "-lGLAD -lGLFW -framework OpenGL")
set(SOURCE_FILES Makefile README.md src src/main.cpp src/Cube.cpp)

add_executable(openGL ${SOURCE_FILES})
