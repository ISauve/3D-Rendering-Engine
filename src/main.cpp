#include "Scene.h"

#include <thread>

using namespace std;

// Forward declarations
GLFWwindow* initWindow();
static void cursorPositionCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void keyCallback(GLFWwindow*, int, int, int, int);
void handleRepeatInput (Scene&, GLFWwindow*);

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

    // Initialize the camera with the initial cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    Camera c(xpos, ypos);

    // Initialize the scene with this camera
    Scene scene(&c);

    // Pass the scene to GLFW so it can be accessed from the input callbacks
    glfwSetWindowUserPointer(window, &scene);

    // Attach callbacks
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // Capture the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enter the rendering loop
    while( !glfwWindowShouldClose(window) ) {
        auto loopStart = chrono::high_resolution_clock::now();

        // Draw the scene
        try {
            scene.draw();
        } catch (EndProgramException e) {
            std::cerr << e.what() << std::endl;
            std::cout << "Ending program due to erroneous state..." << std::endl;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        glfwSwapBuffers(window);

        // Poll for events
        glfwPollEvents();
        handleRepeatInput(scene, window);

        // Pause for an appropriate amount of time to achieve desired frame rate
        auto msPerFrame = int (1.0 / (FPS / 1000.0));
        this_thread::sleep_until(loopStart + chrono::milliseconds(msPerFrame));
        c.Tick();
    }

    glfwTerminate();
    std::cout << "Goodbye!" << std::endl;
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

// Mouse movement callback
static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto scene = reinterpret_cast<Scene*>(glfwGetWindowUserPointer(window));
    scene->Look(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto scene = reinterpret_cast<Scene*>(glfwGetWindowUserPointer(window));
    scene->Zoom(yoffset);
}

// Use callback to handle events that only occur once when a key is pressed
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto scene = reinterpret_cast<Scene*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_L:
                scene->toggleLight();
                break;
            case GLFW_KEY_SPACE:
                scene->Jump();
            default:break;
        }
    }
}

// Use polling for events should continue to occur for as long as the key is being pressed
void handleRepeatInput (Scene& sc, GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_A)) sc.Move(LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)) sc.Move(RIGHT);
    if (glfwGetKey(window, GLFW_KEY_W)) sc.Move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S)) sc.Move(BACKWARD);
}

