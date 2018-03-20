#include "Object.h"

using namespace glm;

Square::Square(GLuint s, Camera* c, LightSource* l) : Shape(s, c, l) {
    _numElements = 6;
    _usesIndices = true;

    GLfloat positions[] = {
            5.0f, -0.5f,  5.0f,   // Front right
            5.0f, -0.5f, -5.0f,   // Back right
            -5.0f, -0.5f, -5.0f,  // Back left
            -5.0f, -0.5f,  5.0f,  // Front left
    };
    _bufferIDs.push_back( storeToVBO(positions, sizeof(positions)) );

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    _bufferIDs.push_back( storeToEBO(indices, sizeof(indices)) );

    glUseProgram(_shaderProgram);

    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
}

void Square::set2DTexture(std::string path) {
    // Note: the same indices defined for positional data will be used here
    GLfloat textcoords[] = {
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    };
    _bufferIDs.push_back( storeToVBO(textcoords, sizeof(textcoords)) );

    _textureIDs.push_back( storeTex(path, GL_REPEAT) ); // increments the global texture counter

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(colAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

    GLint uniTextureObj = glGetUniformLocation(_shaderProgram, "textureObject");
    glUniform1i(uniTextureObj, 1);

    GLint uniTexSample = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniTexSample, textureCounter-1);
}