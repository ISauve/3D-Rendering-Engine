#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Ground::Ground(GLuint s, Camera* c) : Object(s, c) {
    _vao = initializeVAO();

    GLfloat positions[] = {
            5.0f, -0.5f,  5.0f,   // Front right
            5.0f, -0.5f, -5.0f,   // Back right
            -5.0f, -0.5f, -5.0f,  // Back left
            -5.0f, -0.5f,  5.0f,  // Front left
    };
    GLfloat textcoords[] = {
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    };
    _vbo = storeToVBO(positions, sizeof(positions), textcoords, sizeof(textcoords));

    GLuint indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    _ebo = storeToEBO(indices, sizeof(indices));

    _tex = storeTex("assets/grass.jpg", GL_REPEAT);

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(colAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    GLint uniTexSample = glGetUniformLocation(_shaderProgram, "textureSample");
    glUniform1i(uniTexSample, 1);
}

void Ground::render() {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Define a custom project matrix w/ a much closer clipping range
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * mat4(1.0f);
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Ground::cleanUp() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
    glDeleteTextures(1, &_tex);
    _vao = 0;
    _vbo = 0;
    _ebo = 0;
    _tex = 0;
}

