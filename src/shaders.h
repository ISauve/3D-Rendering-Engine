#ifndef OPENGL_SHADERS_H
#define OPENGL_SHADERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <OpenGL/gl.h>
#include <vector>

typedef struct {
    GLenum       type;
    const char*  filename;
    GLuint       shader;
} ShaderInfo;

GLuint LoadShaders( std::vector<ShaderInfo>& );

#endif
