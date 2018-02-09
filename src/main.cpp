#include <iostream>
#include "shaders.h"

using namespace std;

// ------------------------------------------------
//  Global variable declarations
// ------------------------------------------------

enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum VAO_IDs { Triangle, NumVAOs };

GLuint Buffers[NumBuffers];
GLuint VAOs[NumVAOs];

const int NUM_VERTICES = 4;

// ------------------------------------------------
//  Forward declarations
// ------------------------------------------------

GLFWwindow* initWindow();
void render();

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

    // Enter the rendering loop
    while( !glfwWindowShouldClose(window) ) {
        render();

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_LINE_LOOP, 0, NUM_VERTICES);
        /* void glDrawArrays(GLenum mode, GLint first, GLsizei count)
         * Constructs a sequence of geometric primitives using the elements from the currently bound vertex array
         * starting at first and ending at first + count − 1
         * Mode specifies what kinds of primitives are constructed and is one of GL_POINTS, GL_LINES,
         * GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_PATCHES
         */
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Practice", nullptr, nullptr);
    return window;
}

void render() {
    // Create NumVAOs vertex array objects
    glGenVertexArrays(NumVAOs, VAOs);       // generate: allocate names
    glBindVertexArray(VAOs[Triangle]);      // bind: bring them into existence

    // Define the vertices of the shape(s) we're drawing
    GLfloat vertices[NUM_VERTICES][2] = {
            { -0.25,  0.25 },
            {  0.25,  0.25 },
            {  0.25, -0.25 },
            { -0.25, -0.25 },
    };

    // Send the data to the OpenGL server by storing it in a buffer object
    glGenBuffers(NumBuffers, Buffers);                                          // create the buffer object
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // fill the buffer object
    /* void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
     * Usage is how the data will be read and written after allocation
     *   - GL_STATIC_DRAW: The vertex data will be uploaded once and drawn many times (e.g. the world).
     *   - GL_DYNAMIC_DRAW: The vertex data will be created once, changed from time to time, but drawn many times more than that.
     *   - GL_STREAM_DRAW: The vertex data will be uploaded once and drawn once.
     *   - there are more, but those are 3 common ones
     */

    // Initialize our shaders & generate a shader program
    std::vector<ShaderInfo> shaders = {
            { GL_VERTEX_SHADER,     "src/triangle.vtx" },
            { GL_FRAGMENT_SHADER,   "src/triangle.frag" }
    };
    GLuint shaderProgram = LoadShaders(shaders);
    glUseProgram(shaderProgram);

    // Connect the shader ‘in’ variables to a vertex-attribute array
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
}
