#include "Scene.h"
#include "Shaders.h"

using namespace std;

Scene::Scene(Camera* c) {
    // Create the skybox
    _skybox = new SkyBox(fetchShader("cubemap.vtx", "cubemap.frag"), c);

    // Load objects in the scene
    _objects.push_back(new RotatingShape(fetchShader("passThrough3D.vtx", "passThrough.frag"), c, RotatingShape::Type::PYRAMID));
    _objects.push_back(new RotatingShape(fetchShader("passThrough.vtx", "triangle.frag"), c, RotatingShape::Type::TRIANGLE_2D));
    _objects.push_back(new RotatingShape(fetchShader("passThrough.vtx", "passThrough.frag"), c, RotatingShape::Type::SQUARE_3D));
}

void Scene::draw() {
    // Enable blending to create transparency effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);       // accept fragment if closer to camera

    // Clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);   // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render our objects
    _skybox->render();  // always 1st
    for (auto it : _objects)
        it->render();

    // Check for problems
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printErr(err);

    // Flush the buffers
    glFlush();
}

void Scene::printErr(GLenum err) {
    switch (err) {
        // Error descriptions can be found at https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetError.xhtml
        case GL_INVALID_ENUM:
            std::cerr << "Error: invalid enum" << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << "Error: invalid value"  << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "Error: invalid op"  << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "Error: invalid frame buffer op"  << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "Error: out of memory" << std::endl;
            break;
        case GL_STACK_UNDERFLOW:
            std::cerr << "Error: stack underflow"  << std::endl;
            break;
        case GL_STACK_OVERFLOW:
            std::cerr << "Error: stack overflow" << std::endl;
            break;
        default:
            std::cerr << "Unknown error: " << err << std::endl;
            break;
    }
}

void Scene::cleanUp() {
    _skybox->cleanUp();
    delete _skybox;

    for (auto it : _objects) {
        it->cleanUp();
        delete it;
    }
}