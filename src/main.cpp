#include "render.h"
#include "shaders.h"
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
    vtx = "src/" + vtx;
    frag = "src/" + frag;
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
    GLuint shaderProgram1 = fetchShader("passThrough.vtx",      "triangle.frag");
    GLuint shaderProgram2 = fetchShader("passThrough.vtx",      "passThrough.frag");
    GLuint shaderProgram3 = fetchShader("transformation.vtx",   "passThrough.frag");
    GLuint shaderProgram4 = fetchShader("transformation3d.vtx", "passThrough.frag");

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
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);   // accept fragment if closer to camera

        // Draw a black background
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto now = chrono::high_resolution_clock::now();
        float diff = chrono::duration_cast<chrono::duration<float>>(now - start).count();

        // Render our objects
        renderFlashingTriangle(shaderProgram1, diff);
        glDrawArrays(GL_TRIANGLES, 0, 3);   // Draws the currently bound VAO

        renderRectangle(shaderProgram2);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);  // Draws the currently bound VAO using indices

        renderRotatingRectangle(shaderProgram3, diff);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        render3DRotatingRectangle(shaderProgram3, diff);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        renderRotatingPyramid(shaderProgram4, diff, c);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

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

