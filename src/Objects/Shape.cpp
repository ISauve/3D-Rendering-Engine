#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

using namespace glm;

Shape::Shape(GLuint s, Camera* c, LightSource* l) : Object(s,c) {
    _vao = initializeVAO();

    _start = std::chrono::high_resolution_clock::now();

    _lightSrc = l;
    _lit = true;

    _position = vec3(0.0);
    _size = 1.0f;
    _rotationAxis = vec3(0.0);
    _rotationSpeed = 0.0f;

    _texture = 0;
    // _numElements, _usesIndices set by the child class constructors
}

void Shape::render() {
    // Bind the shapes's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);
    glBindTexture(GL_TEXTURE_2D, _texture); // necessary if we're drawing a texture - otherwise, noop

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

    // Draw the shapes
    if (_usesIndices) {
        glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, _numElements);
    }
};