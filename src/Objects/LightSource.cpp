#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

LightSource::LightSource(GLuint s, Camera* c, glm::vec3 lightPos, glm::vec3 lightCol) : Object(s, c) {
    _vao = initializeVAO();

    _position = lightPos;
    _color = lightCol;

    _changed = false;

    float sz = 0.05f;
    float ns = -1 * sz;

    // Vertex data simply represents a small cube centered at the light position
    GLfloat points[] = {
            // Position                                           // Color
            sz + lightPos.x,  sz + lightPos.y, sz + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            sz + lightPos.x,  sz + lightPos.y, ns + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            sz + lightPos.x,  ns + lightPos.y, sz + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            sz + lightPos.x,  ns + lightPos.y, ns + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            ns + lightPos.x,  sz + lightPos.y, sz + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            ns + lightPos.x,  sz + lightPos.y, ns + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            ns + lightPos.x,  ns + lightPos.y, sz + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            ns + lightPos.x,  ns + lightPos.y, ns + lightPos.z,  lightCol.x, lightCol.y, lightCol.z
    };
    _bufferIDs.push_back( storeToVBO(points, sizeof(points)) );

    GLuint indices[] = {
            5, 7, 3,
            3, 1, 5,

            6, 7, 5,
            5, 4, 6,

            3, 2, 0,
            0, 1, 3,

            6, 4, 0,
            0, 2, 6,

            5, 1, 0,
            0, 4, 5,

            7, 6, 3,
            3, 6, 2
    };
    _bufferIDs.push_back( storeToEBO(indices, sizeof(indices)) );

    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(colAttrib);
}

void LightSource::render() {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    if (_changed) {
        // Update the vertex data
        float sz = 0.05f;
        float ns = -1 * sz;
        GLfloat points[] = {
                sz + _position.x,  sz + _position.y, sz + _position.z,  _color.x, _color.y, _color.z,
                sz + _position.x,  sz + _position.y, ns + _position.z,  _color.x, _color.y, _color.z,
                sz + _position.x,  ns + _position.y, sz + _position.z,  _color.x, _color.y, _color.z,
                sz + _position.x,  ns + _position.y, ns + _position.z,  _color.x, _color.y, _color.z,
                ns + _position.x,  sz + _position.y, sz + _position.z,  _color.x, _color.y, _color.z,
                ns + _position.x,  sz + _position.y, ns + _position.z,  _color.x, _color.y, _color.z,
                ns + _position.x,  ns + _position.y, sz + _position.z,  _color.x, _color.y, _color.z,
                ns + _position.x,  ns + _position.y, ns + _position.z,  _color.x, _color.y, _color.z
        };
        glBindBuffer(GL_ARRAY_BUFFER, _bufferIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        _changed = false;
    }

    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * mat4(1.0f);
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
};

void LightSource::setPosition(glm::vec3 p) {
    _position = p;
    _changed = true;
};

void LightSource::setColor(glm::vec3 c) {
    _color = c;
    _changed = true;
};
