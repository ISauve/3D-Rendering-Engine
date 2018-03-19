#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

using namespace glm;

Cube::Cube(GLuint s, Camera* c, LightSource* l) : Object(s, c) {
    _vao = initializeVAO();

    _start = std::chrono::high_resolution_clock::now();
    _lightSrc = l;
    _lit = false;

    _position = vec3(0.0);
    _size = 1.0f;
    _rotationAxis = vec3(0.0);
    _rotationSpeed = 0.0f;

    glUseProgram(_shaderProgram);

    // Define a centered cube & its normals
    GLfloat vertices[] = {
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,
            1.0f, -1.0f, -1.0f,     0.0f,  0.0f, -1.0f,
            1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,
            1.0f,  1.0f, -1.0f,     0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f,     0.0f,  0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  0.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,    0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,    0.0f,  0.0f, 1.0f,

            -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f, -1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f,  1.0f,    -1.0f,  0.0f,  0.0f,

            1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,
            1.0f,  1.0f, -1.0f,     1.0f,  0.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     1.0f,  0.0f,  0.0f,
            1.0f, -1.0f,  1.0f,     1.0f,  0.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     1.0f,  0.0f,  0.0f,

            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,
            1.0f, -1.0f, -1.0f,     0.0f, -1.0f,  0.0f,
            1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,
            1.0f, -1.0f,  1.0f,     0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,

            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,
            1.0f,  1.0f, -1.0f,     0.0f,  1.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,
            1.0f,  1.0f,  1.0f,     0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f
    };
    _bufferIDs.push_back( storeToVBO(vertices, sizeof(vertices)) );

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

    _textureIDs.push_back( storeTex(path, GL_CLAMP_TO_BORDER) );    // incremenents textureCounter

    // TODO: fix these coords (they mess up some textures)
    //  -> need to fix the positional data so that it always draws faces the same way

    GLfloat texCoords[] = {
            // Want:
            // 0.0f, 0.0f,     0.0f, 1.0f,    1.0f, 0.0f,       0.0f, 1.0f,     1.0f, 0.0f,    1.0f, 1.0f,

            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f,
            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f,
            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f,
            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f,
            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f,
            1.0f, 0.0f,     0.0f, 1.0f,    1.0f, 1.0f,       0.0f, 1.0f,     1.0f, 1.0f,    0.0f, 0.0f
    };
    _bufferIDs.push_back( storeToVBO(texCoords, sizeof(texCoords)) );

    GLint texAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(texAttrib);

    GLint uniTextureObj = glGetUniformLocation(_shaderProgram, "textureObject");
    glUniform1i(uniTextureObj, 1);

    GLint uniSampleTex = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniSampleTex, textureCounter-1);
};

void Cube::render() {
    // Bind the cube's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Define our rotation as a factor of time
    auto now = std::chrono::high_resolution_clock::now();
    float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
    mat4 rot(1.0f);
    if (_rotationSpeed != 0) rot = rotate(mat4(1.0f), timeDiff * _rotationSpeed * radians(360.f), _rotationAxis);

    // Generate the model matrix (scale -> rotate -> translate)
    mat4 model = translate(mat4(1.0f), _position) * rot * scale(mat4(1.0f), vec3(_size, _size, _size));

    // Pass the MVP matrix into our shader
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    if (_lit) {
        // Needed: light color, light position, model matrix, current position
        GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
        glUniform3fv(uniLightCol, 1, value_ptr(_lightSrc->Color()));

        GLint uniLightPos = glGetUniformLocation(_shaderProgram, "lightPos");
        glUniform3fv(uniLightPos, 1, value_ptr(_lightSrc->Position()));

        GLint uniModel = glGetUniformLocation(_shaderProgram, "Model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model));

        GLint uniPosn = glGetUniformLocation(_shaderProgram, "viewPos");
        glUniform3fv(uniPosn, 1, value_ptr(_c->Position()));

    } else {
        // Set lightColor = 0 so the fragment shader knows not to try to light the shape
        GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
        glUniform3fv(uniLightCol, 1, value_ptr(vec3(0.0f)));
    }

    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
};

void Cube::cleanUp() {
    glDeleteVertexArrays(1, &_vao);
    for (auto it : _bufferIDs)
        glDeleteBuffers(1, &it);
    for (auto it : _textureIDs)
        glDeleteTextures(1, &it);
};

