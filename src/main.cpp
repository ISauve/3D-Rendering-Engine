#include "Object.h"
#include "Camera.h"
#include "Shaders.h"

#include <iostream>
#include <thread>

using namespace std;

GLFWwindow* initWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Practice", nullptr, nullptr);
    return window;
}

// Load the requested shader program
GLuint fetchShader(string vtx, string frag) {
    vtx = "src/shaders/" + vtx;
    frag = "src/shaders/" + frag;
    vector<ShaderInfo> shaders = {
            { GL_VERTEX_SHADER,   vtx.c_str() },
            { GL_FRAGMENT_SHADER, frag.c_str() }
    };
    return LoadShaders(shaders);
}

// Check which keys were pushed & update camera accordingly
void handleInput (Camera& c, GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_LEFT)) c.Look(LEFT);
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) c.Look(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_UP)) c.Look(UP);
    if (glfwGetKey(window, GLFW_KEY_DOWN)) c.Look(DOWN);

    if (glfwGetKey(window, GLFW_KEY_A)) c.Move(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)) c.Move(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_W)) c.Move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S)) c.Move(BACKWARD);
}

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
    GLuint shaderProgram1 = fetchShader("passThrough.vtx",   "triangle.frag");
    GLuint shaderProgram2 = fetchShader("passThrough.vtx",   "passThrough.frag");
    GLuint shaderProgram3 = fetchShader("passThrough3D.vtx", "passThrough.frag");

    // Define desired frame rate
    int FPS = 60;

    Camera c;

    // Enter the rendering loop
    auto start = chrono::high_resolution_clock::now();
    while( !glfwWindowShouldClose(window) ) {
        auto loopStart = chrono::high_resolution_clock::now();

        // Enable blending to create transparency effect
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);   // accept fragment if closer to camera

        // Draw a black background
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto now = chrono::high_resolution_clock::now();
        float diff = chrono::duration_cast<chrono::duration<float>>(now - start).count();

        // Render our objects
        Triangle t(shaderProgram1, c);
        t.render(diff);

        Square s(shaderProgram2, c);
        s.render(0);

        RotatingSquare r(shaderProgram2, c);
        r.render(diff);

        RotatingSquare3D r3d(shaderProgram2, c);
        r3d.render(diff);

        RotatingPyramid p(shaderProgram3, c);
        p.render(diff);

        // Flush the buffers
        glFlush();

        glfwSwapBuffers(window);
        glfwPollEvents();
        handleInput(c, window);

        // Pause for an appropriate amount of time to acheive desired frame rate
        auto msPerFrame = int (1.0 / (FPS / 1000.0));
        this_thread::sleep_until(loopStart + chrono::milliseconds(msPerFrame));
    }

    glfwTerminate();
    return  0;
}

