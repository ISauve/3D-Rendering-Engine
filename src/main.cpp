#include "Scene.h"

#include <thread>

using namespace std;

GLFWwindow* initWindow();
bool handleInput (Camera& c, GLFWwindow* window);

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

    // Define desired frame rate
    int FPS = 60;

    // Capture the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize the camera with the initial cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    Camera c(xpos, ypos);

    // Initialize the scene with this camera
    Scene scene(&c);

    // Enter the rendering loop
    while( !glfwWindowShouldClose(window) ) {
        auto loopStart = chrono::high_resolution_clock::now();

        // Draw the scene
        scene.draw();
        glfwSwapBuffers(window);

        // Fetch events
        glfwPollEvents();
        bool quit = handleInput(c, window);
        if (quit) {
            glfwDestroyWindow(window);
            break;
        }

        // Pause for an appropriate amount of time to achieve desired frame rate
        auto msPerFrame = int (1.0 / (FPS / 1000.0));
        this_thread::sleep_until(loopStart + chrono::milliseconds(msPerFrame));
    }

    scene.cleanUp();
    glfwTerminate();
    return  0;
}

/*************************************************************
                         Helpers
 *************************************************************/

GLFWwindow* initWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Practice", nullptr, nullptr);
    return window;
}


// Check which keys were pushed & update camera accordingly
bool handleInput (Camera& c, GLFWwindow* window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Check if cursor is in the screen
    c.Look(xpos, ypos);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) return true;

    if (glfwGetKey(window, GLFW_KEY_A)) c.Move(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)) c.Move(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_W)) c.Move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S)) c.Move(BACKWARD);
    return false;
}

