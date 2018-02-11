#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "shaders.h"

using namespace std;

// ------------------------------------------------
//  Forward declarations
// ------------------------------------------------

GLFWwindow* initWindow();
void renderFlashingTriangle(GLuint, float);
void renderRectangle(GLuint);
void renderRotatingRectangle(GLuint, float);
void render3DRotatingRectangle(GLuint, float);

// ------------------------------------------------
//  Main
// ------------------------------------------------

int main(int argc, char** argv) {
    // Initialize GLFW & the window context
    glfwInit();
    GLFWwindow* window = initWindow();
    if (!window) {
        cerr << "Failed to create OpenGL context... exiting" << endl;
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD... exiting" << endl;
        return -1;
    }
    cout << "Created OpenGL " << GLVersion.major  << "." <<  GLVersion.minor << " context" <<  endl;

    // Load our shader programs
    std::vector<ShaderInfo> shaders = {
            { GL_VERTEX_SHADER,     "src/passThrough.vtx" },
            { GL_FRAGMENT_SHADER,   "src/triangle.frag" }
    };
    GLuint shaderProgram1 = LoadShaders(shaders);
    shaders = {
            { GL_VERTEX_SHADER,     "src/passThrough.vtx" },
            { GL_FRAGMENT_SHADER,   "src/passThrough.frag" }
    };
    GLuint shaderProgram2 = LoadShaders(shaders);
    shaders = {
            { GL_VERTEX_SHADER,     "src/transformation.vtx" },
            { GL_FRAGMENT_SHADER,   "src/passThrough.frag" }
    };
    GLuint shaderProgram3 = LoadShaders(shaders);

    // Enter the rendering loop
    auto t_start = chrono::high_resolution_clock::now();
    while( !glfwWindowShouldClose(window) ) {
        auto t_now = chrono::high_resolution_clock::now();
        float time = chrono::duration_cast<chrono::duration<float>>(t_now - t_start).count();

        // Enable blending to create transparency effect
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw a black background
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        renderFlashingTriangle(shaderProgram1, time);
        glDrawArrays(GL_TRIANGLES, 0, 3);   // Draws the currently bound VAO

        renderRectangle(shaderProgram2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);  // Draws the currently bound VAO using indices

        renderRotatingRectangle(shaderProgram3, time);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        render3DRotatingRectangle(shaderProgram3, time);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Flush the buffers
        glFlush();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return  0;
}

// ------------------------------------------------
//  Helpers
// ------------------------------------------------

GLFWwindow* initWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Practice", nullptr, nullptr);
    return window;
}

void renderFlashingTriangle(GLuint shaderProgram, float time) {
    // Create & bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Define the vertices of the triangle
    GLfloat vertices[][5] = {
            { -0.8f,    0.9f, 1.0f, 0.0f, 0.0f },  // Red
            { -0.9155f, 0.7f, 0.0f, 1.0f, 0.0f },  // Green
            { -0.6845f, 0.7f, 0.0f, 0.0f, 1.0f }   // Blue
    };

    // Send the data to the OpenGL server by storing it in a buffer object
    GLuint vbo;
    glGenVertexArrays(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glUseProgram(shaderProgram);

    // Connect the shader ‘in’ variables to the data in the buffer
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                          5*sizeof(float),   // Offset btwn consecutive vertices
                          0);                // Offset fr/ beginning of vertex
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                          5*sizeof(float),
                          (void*)(2*sizeof(float)));  // Need to skip over 2 attributes to read color data
    glEnableVertexAttribArray(colAttrib);

    // Vary the transparency with time
    GLint uniColor = glGetUniformLocation(shaderProgram, "transparency");
    time = (sin(time * 2.0f) + 1.0f) / 2.0;
    glUniform1f(uniColor, time);
}

void renderRectangle(GLuint shaderProgram) {
    // Create & bind a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Initialize data for the vertices we're drawing
    GLfloat positions[] = {
             -0.6f, 0.9f,  // Top-left
             -0.4f, 0.9f,  // Top-right
             -0.4f, 0.7f,  // Bottom-right
             -0.6f, 0.7f,  // Bottom-left
    };
    GLfloat colors[] = {
            1.0f, 0.0f, 1.0f,  // Pink
            0.0f, 1.0f, 1.0f,  // Light blue
            1.0f, 1.0f, 0.0f,  // Yellow
            1.0f, 1.0f, 1.0f,  // White
    };

    // Initialize a vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // We'll use glBufferSubData to load the data in 2 parts
    glBufferData(GL_ARRAY_BUFFER,                     // target
                 sizeof(positions) + sizeof(colors),  // total size
                 NULL,                                // no data (yet)
                 GL_STATIC_DRAW);                     // method

    glBufferSubData(GL_ARRAY_BUFFER,     // target
                    0,                   // no offset
                    sizeof(positions),   // size
                    positions);          // data

    glBufferSubData(GL_ARRAY_BUFFER,     // target
                    sizeof(positions),   // offset = sizeof previous data entered
                    sizeof(colors),      // size
                    colors);             // data

    // Specify indices to select which vertices to draw
    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };

    // Initialize an element array buffer object to store these indices
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Store the data in this buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Connect the shader ‘in’ variables to the data in the GL_ARRAY_BUFFER
    glUseProgram(shaderProgram);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib,   // shader attribute location
                          2,           // size (number of components per vertex)
                          GL_FLOAT,    // data type
                          GL_FALSE,    // if data should be normalized
                          0,           // offset between consecutive vertices (0 = "tightly packed")
                          0);          // offset from the start of the buffer object
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);
}

void renderRotatingRectangle(GLuint shaderProgram, float time) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat positions[] = {
            -0.2f, 0.9f,  // Top-left
             0.0f, 0.9f,  // Top-right
             0.0f, 0.7f,  // Bottom-right
            -0.2f, 0.7f,  // Bottom-left
    };
    GLfloat colors[] = {
            0.0f, 1.0f, 0.7f,  // Cyan
            0.5f, 0.0f, 1.0f,  // Purple
            1.0f, 0.0f, 0.5f,  // Pink
            0.7f, 1.0f, 0.0f,  // Yellow-green
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);

    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glUseProgram(shaderProgram);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    using namespace glm;

    // Rotate the matrix as a factor of time by creating a transformation matrix
    // 1) Translate the point to the origin
    mat4 trans1 = translate(mat4(1.0f),                // target: a 4x4 identity matrix
                            vec3(0.1f, -0.8f, 0.0f));  // transformation details

    // 2) Rotate the point around the z axis
    vec3 rotationAxis(0.0f, 0.0f, 1.0f);                // plane of rotation = z
    float rotationInRad = time * 0.3f * radians(360.f); // * 0.3 to slow down the rotation a little
    mat4 rot = rotate(mat4(1.0f), rotationInRad, rotationAxis);

    // 3) Translate the point back to it's original position
    mat4 trans2 = translate(mat4(1.0f), vec3(-0.1f, 0.8f, 0.0f));

    mat4 transformation = trans2 * rot * trans1;    // Transformations happen from right -> left

    // Pass the matrix into the shader
    GLint uniTransform = glGetUniformLocation(shaderProgram, "transformation");
    glUniformMatrix4fv(uniTransform,      // target variable
                       1,                 // how many matrices to upload (can upload an array of matrices)
                       GL_FALSE,           // should the matrix be transposed
                       value_ptr(transformation));   // specify the matrix to upload
}

void render3DRotatingRectangle(GLuint shaderProgram, float time) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat positions[] = {
            0.1f, 0.9f,  // Top-left
            0.3f, 0.9f,  // Top-right
            0.3f, 0.7f,  // Bottom-right
            0.1f, 0.7f,  // Bottom-left
    };
    GLfloat colors[] = {
            1.0f, 0.7f, 0.0f,  // Orange
            1.0f, 0.0f, 0.0f,  // Red
            1.0f, 0.3f, 1.0f,  // Pink
            0.8f, 1.0f, 0.0f,  // Yellow
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);

    GLuint indices[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0,        // Triangle 2
    };

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glUseProgram(shaderProgram);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(positions)));
    glEnableVertexAttribArray(colAttrib);

    using namespace glm;

    // Create a model transformation matrix (rotate vertices)
    mat4 trans1 = translate(mat4(1.0f), vec3(-0.2f, -0.8f, 0.0f));
    mat4 rot = rotate(mat4(1.0f), time * 0.3f * radians(360.f), vec3(0.0f, 0.0f, 1.0f));
    mat4 trans2 = translate(mat4(1.0f), vec3(0.2f, 0.8f, 0.0f));
    mat4 model = trans2 * rot * trans1;

    // Create a view transformation matrix
    mat4 view = lookAt(    // simulates a moving camera
            vec3(1.0f, 0.0f, 0.45f),  // eye (camera position)
            vec3(-0.2, 0.8f, -0.7f),  // center (where camera is pointed)
            vec3(0.0f, 0.0f, 1.0f)    // up (which axis of your model is oriented
                                      //   towards the top of the screen)
    );

    // Create a projection matrix
    mat4 proj = perspective(radians(45.0f),   // vertical FOV
                            600.0f / 600.0f,  // aspect ratio
                            1.0f,             // "near" clipping plane
                            10.0f);          // "far" clipping plane

    // Pass the matrix into the shader
    mat4 transformation = proj * view * model;
    GLint uniTransform = glGetUniformLocation(shaderProgram, "transformation");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(transformation));
}
