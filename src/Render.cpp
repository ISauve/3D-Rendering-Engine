#include "Object.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

void Triangle::render(float time) {
    // Define the vertices of the triangle
    GLfloat vertices[] = {
             -0.8f,     0.1f, 1.0f, 0.0f, 0.0f ,  // Red
             -0.9155f, -0.1f, 0.0f, 1.0f, 0.0f ,  // Green
             -0.6845f, -0.1f, 0.0f, 0.0f, 1.0f    // Blue
    };

    // Send the data to the OpenGL server by storing it in a buffer object
    storeToVBO(vertices, sizeof(vertices));

    glUseProgram(_shaderProgram);

    // Connect the shader ‘in’ variables to the data in the buffer
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

    // Vary the transparency with time
    GLint uniColor = glGetUniformLocation(_shaderProgram, "transparency");
    time = (sin(time * 2.0f) + 1.0f) / 2.0;
    glUniform1f(uniColor, time);

    // Create a model matrix
    mat4 model(1.0f);

    // Create a projection matrix
    mat4 proj = perspective(radians(45.0f),   // vertical FOV
                            600.0f / 600.0f,  // aspect ratio
                            1.0f,             // "near" clipping plane
                            10.0f);           // "far" clipping plane

    // Pass the MPV matrix into our shader
    mat4 MVP = proj * _c.ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform,      // target variable
                       1,                 // how many matrices to upload (can upload an array of matrices)
                       GL_FALSE,          // should the matrix be transposed
                       value_ptr(MVP));   // specify the matrix to upload

    // Draw the currently bound VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);
};


void Square::render(float time) {
    // Initialize data for the vertices we're drawing
    GLfloat positions[] = {
            -0.1f,  0.1f,  // Top-left
             0.1f,  0.1f,  // Top-right
             0.1f, -0.1f,  // Bottom-right
            -0.1f, -0.1f,  // Bottom-left
    };
    GLfloat colors[] = {
            1.0f, 0.0f, 1.0f,  // Pink
            0.0f, 1.0f, 1.0f,  // Light blue
            1.0f, 1.0f, 0.0f,  // Yellow
            1.0f, 1.0f, 1.0f,  // White
    };
    storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    // Specify indices to select which vertices to draw
    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };
    storeToEBO(indices, sizeof(indices));

    // Connect the shader ‘in’ variables to the data in the GL_ARRAY_BUFFER
    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib,   // shader attribute location
                          2,           // size (number of components per vertex)
                          GL_FLOAT,    // data type
                          GL_FALSE,    // if data should be normalized
                          0,           // offset between consecutive vertices (0 = "tightly packed")
                          0);          // offset from the start of the buffer object
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    // Pass the MPV matrix into our shader
    mat4 model = translate(mat4(1.0f), vec3(-0.5f, 0.0f, 0.0f));
    mat4 proj = perspective(radians(45.0f), 600.0f / 600.0f, 1.0f, 10.0f);

    mat4 MVP = proj * _c.ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    // Draw the currently bound VAO using indices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void RotatingSquare::render(float time) {
    GLfloat positions[] = {
            -0.1f,  0.1f,  // Top-left
             0.1f,  0.1f,  // Top-right
             0.1f, -0.1f,  // Bottom-right
            -0.1f, -0.1f,  // Bottom-left
    };
    GLfloat colors[] = {
            0.0f, 1.0f, 0.7f,  // Cyan
            0.5f, 0.0f, 1.0f,  // Purple
            1.0f, 0.0f, 0.5f,  // Pink
            0.7f, 1.0f, 0.0f,  // Yellow-green
    };
    storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };
    storeToEBO(indices, sizeof(indices));

    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    using namespace glm;

    // Rotate the matrix around z as a factor of time
    vec3 rotationAxis(0.0f, 0.0f, 1.0f);
    float rotationInRad = time * 0.3f * radians(360.f); // * 0.3 to slow down the rotation a little
    mat4 rot = rotate(mat4(1.0f), rotationInRad, rotationAxis);

    // Pass the MPV matrix into our shader
    mat4 model = translate(mat4(1.0f), vec3(-0.1f, 0.0f, 0.0f)) * rot;    // Transformations happen from right -> left
    mat4 proj = perspective(radians(45.0f), 600.0f / 600.0f, 1.0f, 10.0f);

    mat4 MVP = proj * _c.ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void RotatingSquare3D::render(float time) {
    GLfloat positions[] = {
            -0.1f,  0.1f,  // Top-left
             0.1f,  0.1f,  // Top-right
             0.1f, -0.1f,  // Bottom-right
            -0.1f, -0.1f,  // Bottom-left
    };
    GLfloat colors[] = {
            1.0f, 0.7f, 0.0f,  // Orange
            1.0f, 0.0f, 0.0f,  // Red
            1.0f, 0.3f, 1.0f,  // Pink
            0.8f, 1.0f, 0.0f,  // Yellow
    };
    storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };
    storeToEBO(indices, sizeof(indices));

    glUseProgram(_shaderProgram);
    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    // Rotate the matrix around z as a factor of time
    mat4 rot1 = rotate(mat4(1.0f), time * 0.3f * radians(360.f), vec3(0.0f, 0.0f, 1.0f));

    // 'Tilt' the square by rotating it in the X axis
    mat4 rot2 = rotate(mat4(1.0f), radians(-70.0f), vec3(1.0f, 0.0f, 0.0f));

    // Pass the MPV matrix into our shader
    mat4 model = translate(mat4(1.0f), vec3(0.2f, 0.0f, 1.0f)) * rot2 * rot1;
    mat4 proj = perspective(radians(45.0f), 600.0f / 600.0f, 1.0f, 10.0f);

    mat4 MVP = proj * _c.ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void RotatingPyramid::render(float time) {
    // Equilateral pyramid with side length s = 0.3
    float a = 0.3f / 2.0f;
    float h = a * sqrt(3.0f);
    float hh = h / 2.0f;

    GLfloat positions[] = {
            // Front face
             0.0f,  hh,  0.0f,  // Top
            -a,    -hh,  hh,    // Bottom-left
             a,    -hh,  hh,    // Bottom-right

            // Back left face
             0.0f,  hh,  0.0f,  // Top
            -a,    -hh,  hh,    // Bottom-left
             0.0f, -hh, -hh,    // Back

            // Back right face
            0.0f,   hh,  0.0f,  // Top
            a,     -hh,  hh,    // Bottom-right
            0.0f,  -hh, -hh,    // Back

            // Bottom
            -a,    -hh,  hh,   // Bottom-left
             a,    -hh,  hh,   // Bottom-right
             0.0f, -hh, -hh,   // Back
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
    storeToVBO(positions, sizeof(positions), colors, sizeof(colors));

    GLuint indices[] = {
            0, 1, 2,    // Front face
            3, 4, 5,    // Back left
            6, 7, 8,    // Back right
            9, 10, 11   // Bottom
    };
    storeToEBO(indices, sizeof(indices));

    glUseProgram(_shaderProgram);

    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    // Rotate the matrix around y as a factor of time
    mat4 rot = rotate(mat4(1.0f), time * 0.1f * radians(360.f), vec3(0.0f, 1.0f, 0.0f));
    mat4 model = translate(mat4(1.0f), vec3(0.8f, 0.0f, 0.0f)) * rot;

    // Pass the MPV matrix into our shader
    mat4 proj = perspective(radians(45.0f), 600.0f / 600.0f, 1.0f, 10.0f);
    mat4 MVP = proj * _c.ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}
