#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>
#include <chrono>

using namespace glm;

/*************************************************************
                          Constructors
 *************************************************************/

Shape::Shape(GLuint s, Camera* c, Type type, vec3 lightPos, vec3 lightCol) : Object(s, c) {
    _start = std::chrono::high_resolution_clock::now();
    _vao = initializeVAO();

    _type = type;
    _lightPos = lightPos;
    _lightColor = lightCol;

    switch (type) {
            break;
        case TRIANGLE_2D:
            initialize2DTriangle();
            break;
        case SQUARE_3D:
            initialize3DSquare();
            break;
        case PYRAMID:
            initializePyramid();
            break;
        case STONE_PYRAMID:
            initializeStonePyramid();
            break;
        default:
            std::cerr << "Error initializing RotatingShape: " << type << " is not a valid shape type." << std::endl;
            break;
    }
}

void Shape::initialize2DTriangle() {
    // Equilateral pyramid with side length s = 0.2
    float a = 0.1f;
    float h = a * sqrt(3.0f);

    GLfloat vertices[] = {
            0.0f,       h,      1.0f, 0.0f, 0.0f,  // Top: red
            -1.0f * a,  0.0f,   0.0f, 1.0f, 0.0f,  // Bottom left: green
            1.0f * a,   0.0f,   0.0f, 0.0f, 1.0f   // Bottom right: blue
    };

    // Send the data to the OpenGL server by storing it in a buffer object
    _vbo = storeToVBO(vertices, sizeof(vertices));

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float),   // Offset btwn consecutive vertices
                          0);                // Offset fr/ beginning of vertex
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float),
                          (void*)(2*sizeof(float)));  // Need to skip over 2 attributes to read color data
    glEnableVertexAttribArray(colAttrib);
}

void Shape::initialize3DSquare() {
    GLfloat positions[] = {
            -0.1f,  0.1f,  // Top-left
            0.1f,  0.1f,   // Top-right
            0.1f, -0.1f,   // Bottom-right
            -0.1f, -0.1f,  // Bottom-left
    };
    GLfloat colors[] = {
            1.0f, 0.7f, 0.0f,  // Orange
            1.0f, 0.0f, 0.0f,  // Red
            1.0f, 0.3f, 1.0f,  // Pink
            0.8f, 1.0f, 0.0f,  // Yellow
    };
    _vbo = storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };
    _ebo = storeToEBO(indices, sizeof(indices));

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                          0,                           // Consecutive values are tightly packed
                          (void*)(sizeof(positions))); // Color data is located after the positional data
    glEnableVertexAttribArray(colAttrib);
};

void Shape::initializePyramid() {
    // Pyramid with bottom side length s = 0.3, height = 0.3
    float a = 0.15f;
    float h = 0.3f;
    float hh = a * sqrt(3.0f) / 2.0f;

    GLfloat positions[] = {
            // Back face
            0.0f,       h,     hh,    // Peak
            -1.0f * a,  0.0f,  0.0f,  // Bottom left
            1.0f * a,   0.0f,  0.0f,  // Bottom right

            // Front left face
            0.0f,       h,     hh,    // Peak
            -1.0f * a,  0.0f,  0.0f,  // Bottom left
            0.0f,       0.0f,  h,     // Bottom front

            // Front right face
            0.0f,       h,     hh,    // Peak
            0.0f,       0.0f,  h,     // Bottom front
            1.0f * a,   0.0f,  0.0f,  // Bottom right

            // Bottom face
            -1.0f * a, 0.0f,  0.0f,  // Bottom left
            1.0f * a,  0.0f,  0.0f,  // Bottom right
            0.0f,      0.0f,  h,     // Bottom front
    };
    GLfloat colors[] = {
            // Green face
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.7f,
            0.7f, 1.0f, 0.0f,

            // Red face
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.5f,
            1.0f, 0.5f, 0.0f,

            // Blue face
            0.0f, 0.0f, 1.0f,
            0.0f, 0.8f, 1.0f,
            0.8f, 0.0f, 1.0f,

            // Yellow face
            1.0f, 1.0f, 0.0f,
            1.0f, 0.6f, 0.0f,
            1.0f, 1.0f, 0.8f,
    };
    _vbo = storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    GLuint indices[] = {
            0, 1, 2,    // Front face
            3, 4, 5,    // Back left
            6, 7, 8,    // Back right
            9, 10, 11   // Bottom
    };
    _ebo = storeToEBO(indices, sizeof(indices));

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);
};

void Shape::initializeStonePyramid() {
    float a = 0.15f;
    float h = 0.3f;
    float hh = a * sqrt(3.0f) / 2.0f;

    // Calculate normals
    vec3 peak = vec3(0.0f, h, hh);
    vec3 bottomL = vec3(-1.0f * a, 0.0f, 0.0f);
    vec3 bottomR = vec3(1.0f * a, 0.0f, 0.0f);
    vec3 bottomFr = vec3(0.0f, 0.0f, h);

    vec3 norm1 = normalize(cross(bottomR - peak, bottomL - peak));
    vec3 norm2 = normalize(cross(bottomL - peak, bottomFr - peak));
    vec3 norm3 = normalize(cross(peak - bottomL, bottomFr - bottomL));
    vec3 norm4 = normalize(cross(bottomFr - bottomL, bottomFr - bottomL));

    GLfloat vertices[] = {
            // Position                // Texture    // Normals
            // Back face
            0.0f,       h,     hh,     0.5f, 1.0f,   norm1.x, norm1.y, norm1.z,
            -1.0f * a,  0.0f,  0.0f,   0.0f, 0.0f,   norm1.x, norm1.y, norm1.z,
            1.0f * a,   0.0f,  0.0f,   1.0f, 0.0f,   norm1.x, norm1.y, norm1.z,

            // Front left face
            0.0f,       h,     hh,     0.5f, 1.0f,   norm2.x, norm2.y, norm2.z,
            -1.0f * a,  0.0f,  0.0f,   0.0f, 0.0f,   norm2.x, norm2.y, norm2.z,
            0.0f,       0.0f,  h,      1.0f, 0.0f,   norm2.x, norm2.y, norm2.z,

            // Front right face
            0.0f,       h,     hh,     0.5f, 1.0f,   norm3.x, norm3.y, norm3.z,
            0.0f,       0.0f,  h,      0.0f, 0.0f,   norm3.x, norm3.y, norm3.z,
            1.0f * a,   0.0f,  0.0f,   1.0f, 0.0f,   norm3.x, norm3.y, norm3.z,

            // Bottom face
            -1.0f * a, 0.0f,  0.0f,    0.5f, 1.0f,   norm4.x, norm4.y, norm4.z,
            1.0f * a,  0.0f,  0.0f,    0.0f, 0.0f,   norm4.x, norm4.y, norm4.z,
            0.0f,      0.0f,  h,       1.0f, 0.0f,   norm4.x, norm4.y, norm4.z
    };
    _vbo = storeToVBO(vertices, sizeof(vertices));

    // Load the texture
    _tex = storeTex("assets/stones.jpg", GL_CLAMP_TO_BORDER);

    // Tell OpenGL where to find/how to interpret the vertex data
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint texAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    GLint normAttrib = glGetAttribLocation(_shaderProgram, "vNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(normAttrib);

    GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
    glUniform3fv(uniLightCol, 1, value_ptr(_lightColor));

    GLint uniLightPos = glGetUniformLocation(_shaderProgram, "lightPos");
    glUniform3fv(uniLightPos, 1, value_ptr(_lightPos));

    GLint uniSampleTex = glGetUniformLocation(_shaderProgram, "textureSample");
    glUniform1i(uniSampleTex, 0);
};


/*************************************************************
                          Rendering
 *************************************************************/

void Shape::render() {
    switch (_type) {
            break;
        case TRIANGLE_2D:
            render2DTriangle();
            break;
        case SQUARE_3D:
            render3DSquare();
            break;
        case PYRAMID:
            renderPyramid();
            break;
        case STONE_PYRAMID:
            renderStonePyramid();
            break;
        default: break;
    }
}

void Shape::render2DTriangle() {
    // Bind the triangle's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Vary the transparency with time
    auto now = std::chrono::high_resolution_clock::now();
    float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
    timeDiff = (sin(timeDiff * 2.0f) + 1.0f) / 2.0;

    GLint uniColor = glGetUniformLocation(_shaderProgram, "transparency");
    glUniform1f(uniColor, timeDiff);

    // Pass the MVP matrix into our shader
    mat4 model = translate(mat4(1.0f), vec3(-0.2f, -0.1f, 0.0f));
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform,      // target variable
                       1,                 // how many matrices to upload (can upload an array of matrices)
                       GL_FALSE,          // should the matrix be transposed
                       value_ptr(MVP));   // specify the matrix to upload

    // Draw the currently bound VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shape::render3DSquare() {
    // Bind the square's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Define a rotation around z as a factor of time
    auto now = std::chrono::high_resolution_clock::now();
    float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
    mat4 rot1 = rotate(mat4(1.0f), timeDiff * 0.3f * radians(360.f), vec3(0.0f, 0.0f, 1.0f));

    // 'Tilt' the square by rotating it in the X axis
    mat4 rot2 = rotate(mat4(1.0f), radians(-70.0f), vec3(1.0f, 0.0f, 0.0f));

    // Pass the MVP matrix into our shader
    mat4 model = translate(mat4(1.0f), vec3(0.2f, 0.0f, 1.0f)) * rot2 * rot1;
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Shape::renderPyramid() {
    // Bind the pyramid's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Define a rotation around y as a factor of time
    auto now = std::chrono::high_resolution_clock::now();
    float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
    mat4 rot = rotate(mat4(1.0f), timeDiff * 0.1f * radians(360.f), vec3(0.0f, 1.0f, 0.0f));

    // Pass the MVP matrix into our shader
    mat4 trans1 = translate(mat4(1.0f), vec3(0.0f, 0.0f, -0.129904f)); // translate to the center before rotation
    mat4 trans2 = translate(mat4(1.0f), vec3(0.8f, -0.15f, 0.129904f));
    mat4 model = trans2 * rot * trans1;
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void Shape::renderStonePyramid() {
    // Bind the pyramid's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Define a translation in x as a factor of time
    auto now = std::chrono::high_resolution_clock::now();
    float timeDiff = std::chrono::duration_cast<std::chrono::duration<float>>(now - _start).count();
    mat4 model = translate(mat4(1.0f), vec3(sin(timeDiff), 0.0f, 0.5f));

    // Pass the MVP matrix into our shader
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    // Pass just the model matrix to the shader
    GLint uniModel = glGetUniformLocation(_shaderProgram, "Model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}


/*************************************************************
                            Other
 *************************************************************/

void Shape::cleanUp() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    if (_ebo != 0) glDeleteBuffers(1, &_ebo);
    if (_tex != 0) glDeleteTextures(1, &_tex);
    _vao = 0;
    _vbo = 0;
    _ebo = 0;
    _tex = 0;
}
