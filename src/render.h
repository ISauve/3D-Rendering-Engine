#ifndef OPENGL_RENDER_H
#define OPENGL_RENDER_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <thread>

void renderFlashingTriangle(GLuint, float);
void renderRectangle(GLuint);
void renderRotatingRectangle(GLuint, float);
void render3DRotatingRectangle(GLuint, float);
void renderRotatingPyramid(GLuint, float);


#endif //OPENGL_RENDER_H
