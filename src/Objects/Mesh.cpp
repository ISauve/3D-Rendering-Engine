#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Mesh::Mesh(GLuint s, Camera* c) : Object(s, c) {
    _vao = initializeVAO();

    _blend = true;

    _position = vec3(0.0f);
    _size = 1.0f;
};

void Mesh::addData(std::vector<Vertex> vert, std::vector<unsigned int> in, std::vector<Texture> tex) {
    _vertices = vert;
    _indices = in;
    _textures = tex;

    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    _bufferIDs.push_back( storeToVBO(&_vertices[0], _vertices.size() * sizeof(Vertex)) );
    _bufferIDs.push_back( storeToEBO(&_indices[0], _indices.size() * sizeof(unsigned int)) );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Load the textures
    for (auto& it : _textures) {
        it.id = storeTex( it.path );
        _textureIDs.push_back( it.id );
    }
};

void Mesh::render() {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    if (!_blend) glDisable(GL_BLEND);

    // Bind the textures
    for(int i=0; i < _textures.size(); i++) {
        // Tell the shader where to find which texture by activating a unique texture unit
        glActiveTexture(GL_TEXTURE0 + i);
        GLuint uniLoc = glGetUniformLocation(_shaderProgram, _textures[i].name.c_str());
        glUniform1i(uniLoc, i);

        glBindTexture(GL_TEXTURE_2D, _textures[i].id);
    }

    // Pass the MVP matrix into our shader
    mat4 model = translate(mat4(1.0f), _position) * scale(mat4(1.0f), vec3(_size, _size, _size));
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

    // Re-enable blending
    glEnable(GL_BLEND);
};