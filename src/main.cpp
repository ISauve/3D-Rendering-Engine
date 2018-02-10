#include <iostream>
#include "shaders.h"

using namespace std;

// ------------------------------------------------
//  Global variable declarations
// ------------------------------------------------

enum Buffer_IDs { ArrayBuffer, ElementsBuffer, NumBuffers };
enum VAO_IDs { Triangle, NumVAOs };

GLuint Buffers[NumBuffers];
GLuint VAOs[NumVAOs];

const int NUM_INDICES = 6;

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

        // Enable blending to create transparency effect
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Clear a black background
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, nullptr);
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
    GLfloat vertices[][5] = {
            { -0.5f,  0.5f, 1.0f, 0.0f, 0.0f },  // Top-left
            {  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },  // Top-right
            {  0.5f, -0.5f, 0.0f, 0.0f, 1.0f },  // Bottom-right
            { -0.5f, -0.5f, 1.0f, 1.0f, 1.0f }   // Bottom-left
    };

    // Load the indices of the points we want to draw
    GLuint elements[] = {
            0, 1, 2,        // Triangle 1
            2, 3, 0         // Triangle 2
    };

    // Send the data to the OpenGL server by storing it in a buffer object
    glGenBuffers(NumBuffers, Buffers);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    /* void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
     * Usage is how the data will be read and written after allocation
     *   - GL_STATIC_DRAW: The vertex data will be uploaded once and drawn many times (e.g. the world).
     *   - GL_DYNAMIC_DRAW: The vertex data will be created once, changed from time to time, but drawn many times more than that.
     *   - GL_STREAM_DRAW: The vertex data will be uploaded once and drawn once.
     *   - there are more, but those are 3 common ones
     */

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[ElementsBuffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Initialize our shaders & generate a shader program
    std::vector<ShaderInfo> shaders = {
            { GL_VERTEX_SHADER,     "src/triangle.vtx" },
            { GL_FRAGMENT_SHADER,   "src/triangle.frag" }
    };
    GLuint shaderProgram = LoadShaders(shaders);
    glUseProgram(shaderProgram);

    // Connect the shader ‘in’ variables to a vertex-attribute array
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    // Connect the output of the vtx shader to the input of the frag shader
    GLint colAttrib = glGetAttribLocation(shaderProgram, "vColor");
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
    /* Last 2 parameters are:
     *  1) byte offset between consecutive elements in the array
     *  2) offset from the start of the buffer object for the first set of values in the array
     *      -> have to skip over 2 floats to read the color data
     */
    glEnableVertexAttribArray(colAttrib);
}
