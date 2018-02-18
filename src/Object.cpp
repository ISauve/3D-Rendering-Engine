#include "Object.h"

Object::Object(GLuint s, Camera& c) : _shaderProgram(s), _c(c) {
    // Create & bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void Object::storeToVBO(GLfloat* vertices, int size) {
    // Create, bind, and load data into a vertex buffer object
    GLuint vbo;
    glGenVertexArrays(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void Object::storeToVBO(GLfloat* positions, int sizeP, GLfloat* colors, int sizeC) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // We'll use glBufferSubData to load the data in 2 parts
    glBufferData(GL_ARRAY_BUFFER,  // target
                 sizeP + sizeC,    // total size
                 NULL,             // no data (yet)
                 GL_STATIC_DRAW);  // method

    glBufferSubData(GL_ARRAY_BUFFER,  // target
                    0,                // no offset
                    sizeP,            // size
                    positions);       // data

    glBufferSubData(GL_ARRAY_BUFFER,  // target
                    sizeP,            // offset = sizeof previous data entered
                    sizeC,            // size
                    colors);          // data
}

void Object::storeToEBO(GLuint* indices, int size) {
    // Create, bind, and load data into an element buffer object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}
