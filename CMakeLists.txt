cmake_minimum_required(VERSION 3.9)
project(OpenGL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_SHARED_LINKER_FLAGS "-lGLAD -lGLFW -framework OpenGL")

add_executable(OpenGL Makefile README.md src/main.cpp src/Shaders.cpp src/Shaders.h src/render.cpp src/render.h src/Camera.cpp src/Camera.h src/Camera.cpp src/Object.cpp src/Object.h src/Render.cpp)