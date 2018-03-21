#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;


SkyBox::SkyBox(GLuint s, Camera* c) : Object(s, c, nullptr) {

    // Vertex data simply represents a large cube
    GLfloat points[] = {
            1.0f,  1.0f, 1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  -1.0f, 1.0f,
            1.0f,  -1.0f, -1.0f,
            -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  -1.0f, 1.0f,
            -1.0f,  -1.0f, -1.0f
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

    // Load the cubemap textures
    std::vector<std::string> faces {
            "assets/skybox/right.jpg",
            "assets/skybox/left.jpg",
            "assets/skybox/top.jpg",
            "assets/skybox/bottom.jpg",
            "assets/skybox/front.jpg",
            "assets/skybox/back.jpg"
    };
    _textureIDs.push_back( storeCubeMap(faces) );

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint sampleCube = glGetUniformLocation(_shaderProgram, "skybox");
    glUniform1i(sampleCube, 0);
};


void SkyBox::render() {
    // Bind the skybox's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Turn off the depth test (so that it always gets overwritten)
    glDepthMask(GL_FALSE);

    // Remove the translation component of the view matrix
    mat4 view = mat4(mat3(_c->ViewMatrix()));

    // Pass the MVP matrix into our shader
    mat4 model = mat4(1.0f);
    mat4 MVP = _c->ProjMatrix() * view * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // Reset the depth test
    glDepthMask(GL_TRUE);
}
