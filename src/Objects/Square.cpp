#include "Object.h"

using namespace glm;

Square::Square(GLuint s, Camera* c, LightSource* l) : Shape(s, c, l) {
    glUseProgram(_shaderProgram);

    _numElements = 6;
    _usesIndices = true;

    GLfloat positions[] = {
            1.0f, 0.0f,  1.0f,   // Front right
            1.0f, 0.0f, -1.0f,   // Back right
            -1.0f, 0.0f, -1.0f,  // Back left
            -1.0f, 0.0f,  1.0f,  // Front left
    };
    GLfloat normals[] = {
            0.0f, 1.0f,  0.0f,
            0.0f, 1.0f,  0.0f,
            0.0f, 1.0f,  0.0f,
            0.0f, 1.0f,  0.0f,
    };
    _bufferIDs.push_back( storeToVBO(positions, sizeof(positions), normals, sizeof(normals)) );

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    _bufferIDs.push_back( storeToEBO(indices, sizeof(indices)) );

    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint normAttrib = glGetAttribLocation(_shaderProgram, "vNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(normAttrib);

    unbind();
}

void Square::set2DTexture(std::string path) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Note: the same indices defined for positional data will be used here
    GLfloat textcoords[] = {
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    };
    _bufferIDs.push_back( storeToVBO(textcoords, sizeof(textcoords)) );

    _texture = storeTex(path, GL_REPEAT);
    _textureIDs.push_back( _texture );

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(colAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

    GLint uniTextureObj = glGetUniformLocation(_shaderProgram, "textureObject");
    glUniform1i(uniTextureObj, 1);

    glActiveTexture(GL_TEXTURE0);
    GLint uniTexSample = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniTexSample, 0);

    unbind();
}