#ifndef OPENGL_SHADERS_H
#define OPENGL_SHADERS_H

#include <vector>

typedef struct {
    GLenum       type;
    const char*  filename;
    GLuint       shader;
} ShaderInfo;

GLuint LoadShaders( std::vector<ShaderInfo>& );

#endif
