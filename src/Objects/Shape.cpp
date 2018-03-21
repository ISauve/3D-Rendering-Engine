#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

using namespace glm;

// Note: don't unbind in this ctor because the child class constructors still haven't been called
Shape::Shape(GLuint s, Camera* c, LightSource* l) : Object(s, c, l), _texture(0) {}

void Shape::render() {
    // Bind the shapes's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Bind texture data (no effect if a texture isn't set)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture); // this binding is global, so needs to be set for each draw

    mat4 rot(1.0f);
    if (_rotationAxis != vec3(0.0f)) {
        // Speed is set -> rotation as a factor of time in the specified axis
        if (_rotationSpeed != 0) {
            auto now = std::chrono::high_resolution_clock::now();
            float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
            rot = rotate(mat4(1.0f), timeDiff * _rotationSpeed * radians(360.f), _rotationAxis);
        }

        // Speed is not set -> static rotation so object simply faces the given axis
        else {
            vec3 forward = vec3(0.0f, 0.0f, 1.0f);
            float angle = acos( dot(forward, _rotationAxis) / (length(forward) * length(_rotationAxis)) );
            rot = rotate(mat4(1.0f), angle, _rotationAxis);
        }
    }

    // Generate the model matrix (scale -> rotate -> translate)
    mat4 model = translate(mat4(1.0f), _position) * rot * scale(mat4(1.0f), vec3(_size, _size, _size));

    // Pass the MVP matrix into our shader
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    // Set the appropriate lighting data
    if (_lit) {
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
    if (_usesIndices)   glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, nullptr);
    else                glDrawArrays(GL_TRIANGLES, 0, _numElements);

    unbind();
};

// Set the bound data back to defaults
void Shape::unbind() {
    // Shapes have a maximum of 1 texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}
