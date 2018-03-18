#ifndef OPENGL_SHADERS_H
#define OPENGL_SHADERS_H

#include <vector>
#include <string>

typedef struct {
    GLenum       type;
    const char*  filename;
    GLuint       shader;
} ShaderInfo;

GLuint loadShaders( std::vector<ShaderInfo>& );

GLuint fetchShader(std::string, std::string);

#endif
