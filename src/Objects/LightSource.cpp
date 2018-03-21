#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

LightSource::LightSource(GLuint s, Camera* c, glm::vec3 lightPos, glm::vec3 lightCol) : Object(s, c, nullptr) {

    _position = lightPos;
    _color = lightCol;
    _onColor = lightCol;
    _changed = false;

    _size = 0.05f;  // default size
    float nSize = -1 * _size;

    // Vertex data simply represents a small cube centered at the light position
    GLfloat points[] = {
            // Position                                           // Color
            _size + lightPos.x,  _size + lightPos.y, _size + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            _size + lightPos.x,  _size + lightPos.y, nSize + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            _size + lightPos.x,  nSize + lightPos.y, _size + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            _size + lightPos.x,  nSize + lightPos.y, nSize + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            nSize + lightPos.x,  _size + lightPos.y, _size + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            nSize + lightPos.x,  _size + lightPos.y, nSize + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            nSize + lightPos.x,  nSize + lightPos.y, _size + lightPos.z,  lightCol.x, lightCol.y, lightCol.z,
            nSize + lightPos.x,  nSize + lightPos.y, nSize + lightPos.z,  lightCol.x, lightCol.y, lightCol.z
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
        float nSize = -1 * _size;
        GLfloat points[] = {
                _size + _position.x,  _size + _position.y, _size + _position.z,  _color.x, _color.y, _color.z,
                _size + _position.x,  _size + _position.y, nSize + _position.z,  _color.x, _color.y, _color.z,
                _size + _position.x,  nSize + _position.y, _size + _position.z,  _color.x, _color.y, _color.z,
                _size + _position.x,  nSize + _position.y, nSize + _position.z,  _color.x, _color.y, _color.z,
                nSize + _position.x,  _size + _position.y, _size + _position.z,  _color.x, _color.y, _color.z,
                nSize + _position.x,  _size + _position.y, nSize + _position.z,  _color.x, _color.y, _color.z,
                nSize + _position.x,  nSize + _position.y, _size + _position.z,  _color.x, _color.y, _color.z,
                nSize + _position.x,  nSize + _position.y, nSize + _position.z,  _color.x, _color.y, _color.z
        };
        glBindBuffer(GL_ARRAY_BUFFER, _bufferIDs[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

        _changed = false;
    }

    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * mat4(1.0f);
    GLint uniTranSizeform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTranSizeform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
};

void LightSource::setColor(glm::vec3 p) {
    _color = p;
    _onColor = p;
    _changed = true;
};

void LightSource::toggle() {
    if (_color == _onColor) _color = glm::vec3(0.5, 0.5, 0.5);  // off = gray
    else                    _color = _onColor;

    _changed = true;
};
