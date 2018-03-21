#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Mesh::Mesh(GLuint s, Camera* c, LightSource* l) : Object(s, c, l), _blend (true) {
    unbind();
};

void Mesh::addData(std::vector<Vertex> vert, std::vector<unsigned int> in, std::vector<Texture> tex) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    _vertices = vert;
    _indices = in;
    _textures = tex;

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

    unbind();
};

void Mesh::render() {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Some models dont have RGBA textures so blending messes with them
    if (!_blend) glDisable(GL_BLEND);

    // Bind the textures
    for(int i=0; i < _textures.size(); i++) {
        // Tell the shader where to find which texture by binding each texture to a unique texture unit
        glActiveTexture(GL_TEXTURE0 + i);
        GLuint uniLoc = glGetUniformLocation(_shaderProgram, _textures[i].name.c_str());
        glUniform1i(uniLoc, i);

        glBindTexture(GL_TEXTURE_2D, _textures[i].id);
    }

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

    // Pass the MVP matrix into our shader
    mat4 model = translate(mat4(1.0f), _position) * rot * scale(mat4(1.0f), vec3(_size, _size, _size));
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

    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);

    // Re-enable blending
    if (!_blend) glEnable(GL_BLEND);

    unbind();
};

void Mesh::unbind() {
    // Unbind all the textures
    for(int i=0; i < _textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}