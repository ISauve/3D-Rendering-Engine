#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

using namespace glm;

Cube::Cube(GLuint s, Camera* c, LightSource* l) : Shape(s, c, l) {
    _numElements = 36;
    _usesIndices = false;

    // Define a centered cube & its normals
    GLfloat vertices[] = {
            // Position             // Normal
            1.0f, -1.0f, -1.0f,     0.0f,  0.0f, -1.0f,     // Back face
            1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,
            1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f, 1.0f,      // Front face
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f,     0.0f,  0.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f,     0.0f,  0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  0.0f, 1.0f,

            -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,    // Left face
            -1.0f,  1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,

            1.0f, -1.0f,  1.0f,     1.0f,  0.0f,  0.0f,     // Right face
            1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,
            1.0f,  1.0f, -1.0f,     1.0f,  0.0f,  0.0f,

            1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,     // Bottom face
            1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,

            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     // Top face
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,
            1.0f,  1.0f, -1.0f,     0.0f,  1.0f,  0.0f
    };
    _bufferIDs.push_back( storeToVBO(vertices, sizeof(vertices)) );

    glUseProgram(_shaderProgram);

    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint normAttrib = glGetAttribLocation(_shaderProgram, "vNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE,  6 * sizeof(float), (void*)( 3 * sizeof(float) ));
    glEnableVertexAttribArray(normAttrib);
}

void Cube::setColor(vec3 color) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    GLfloat colors[] = {
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,

            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,

            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,

            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,

            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,

            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z,
            color.x, color.y, color.z,      color.x, color.y, color.z,      color.x, color.y, color.z
    };
    _bufferIDs.push_back( storeToVBO(colors, sizeof(colors)) );

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttrib);
}

void Cube::setColors(GLfloat* colors, int sizeC) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    _bufferIDs.push_back( storeToVBO(colors, sizeC) );
    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttrib);
};

void Cube::set2DTexture(std::string path) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    _texture = storeTex(path, GL_CLAMP_TO_BORDER);
    _textureIDs.push_back( _texture );

    GLfloat texCoords[] = {
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
            0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,
    };
    _bufferIDs.push_back( storeToVBO(texCoords, sizeof(texCoords)) );

    GLint texAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texAttrib);

    GLint uniTextureObj = glGetUniformLocation(_shaderProgram, "textureObject");
    glUniform1i(uniTextureObj, 1);

    GLint uniSampleTex = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniSampleTex, 1);   // only binding 1 texture
};

