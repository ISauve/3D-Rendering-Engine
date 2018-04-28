#include "Scene.h"
#include "Shaders.h"

using namespace std;

Scene::Scene(Camera* c) : _c(c), _isLit(true) {
    auto timer = chrono::high_resolution_clock::now();

    // Create the skybox
    _skybox = new SkyBox(fetchShader("cubemap.vtx", "cubemap.frag"), this);

    // Create the light source
    glm::vec3 lightPos(0.0f, 10.0f, 0.0f);  // Note that light position is absolute (not relative to terrain)
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
    _lightSrc = new LightSource(fetchShader("shape.vtx", "shape.frag"), this, lightPos, lightCol);
    _lightSrc->setSize(0.5f);

    // Load the 1st terrain
    Terrain* terrain = new Terrain(fetchShader("terrain.vtx", "terrain.frag"), this, "assets/heightmap.png");
    terrain->setPosition(glm::vec3(-1 * terrain->getSize() / 2.0f, 0.0, -1 * terrain->getSize() / 2.0f));
    terrain->set2DTexture("assets/grass2.png");
    _objects.push_back(terrain);

    _currTerrain = terrain;
    c->setCurrTerrain(_currTerrain);

    loadShapes();
    loadModels();
    loadTerrains();

    if (DEBUG) {
        std::chrono::duration<double> loadingTime = chrono::high_resolution_clock::now() - timer;
        std::cout << "Loaded scene data in " << loadingTime.count() << "s" << std::endl;
    }
}

Scene::~Scene() {
    if (_skybox != nullptr) delete _skybox;
    if (_lightSrc != nullptr) delete _lightSrc;
    for (auto it : _objects)
        delete it;
}

int ticker = 0;
void Scene::draw() {
    auto timer = chrono::high_resolution_clock::now();

    // Enable blending to create transparency effect if a < 1
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);       // accept fragment if closer to camera

    // Clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);   // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render our objects
    try {
        if (_skybox != nullptr) _skybox->render();      // must always be 1st
        else if (DEBUG && ticker == 200) std::cout << "Warning: skybox is null" << std::endl;

        if (_lightSrc != nullptr) _lightSrc->render();
        else if (DEBUG && ticker == 200) std::cout << "Warning: light box is null" << std::endl;

        for (auto it : _objects)
            it->render();
    } catch (std::runtime_error& e) {
        std::string msg = "Exception thrown while attempting to render scene: ";
        throw EndProgramException(msg + e.what());
    }

    // Check for problems
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) handleErr(err);  // if not in debug mode, throws an EndProgramException

    // Flush the buffers
    glFlush();

    ticker++;
    if (DEBUG && ticker == 200) {   // periodically check how long the scene takes to draw
        ticker = 0;
        std::chrono::duration<double> drawingTime = chrono::high_resolution_clock::now() - timer;
        std::cout << "Time to draw scene: " << drawingTime.count() << "s" << std::endl;
    }
}

void Scene::toggleLight() {
    _isLit = !_isLit;
    auto state = (_isLit) ? "on" : "off";
    if (DEBUG) std::cout << "Lighting turned " << state << std::endl;

    if (_lightSrc != nullptr) _lightSrc->isLit(_isLit);
    for (auto it : _objects)
        it->isLit(_isLit);
}

void Scene::loadShapes() {
    Cube* cube1 = new Cube(fetchShader("shape.vtx", "shape.frag"), this);
    cube1->set2DTexture("assets/crate.jpeg");
    cube1->setPosition(glm::vec3(0.7, 0.7, 2.0));
    cube1->setRotation(glm::vec3(0.0, -1.0, 0.0), 0.05);
    cube1->setSize(0.1);
    _objects.push_back(cube1);

    Cube* cube2 = new Cube(fetchShader("shape.vtx", "shape.frag"), this);
    cube2->set2DTexture("assets/stones.jpg");
    cube2->setPosition(glm::vec3(-0.2, 0.65, 0.5));
    cube2->setSize(0.15);
    cube2->setRotation(glm::vec3(0.5, 1.0, 1.0));
    _objects.push_back(cube2);

    Cube* cube3 = new Cube(fetchShader("shape.vtx", "shape.frag"), this);
    cube3->set2DTexture("assets/metal.jpg");
    cube3->setPosition(glm::vec3(0.8, 0.9, -0.3));
    cube3->setSize(0.3);
    cube3->setRotation(glm::vec3(1.0, 0.0, 0.0), 0.03);
    _objects.push_back(cube3);
}

void Scene::loadModels() {
    Model* nanosuit = new Model("assets/nanosuit/nanosuit.obj", fetchShader("model.vtx", "model.frag"), this);
    nanosuit->setPosition(glm::vec3(3.0, 0.0, 2.0));
    nanosuit->setRotation(glm::vec3(0.0, -1.0, 0.0));
    nanosuit->setSize(0.06f);
    nanosuit->setBlend(false);
    _objects.push_back(nanosuit);

    Model* tree = new Model("assets/Tree/Tree.obj", fetchShader("model.vtx", "model.frag"), this);
    tree->setPosition(glm::vec3(5.0, 0.0, -0.5));
    _objects.push_back(tree);

    Model* patchOfGrass = new Model("assets/grasses/Grass_02.obj", fetchShader("model.vtx", "model.frag"), this);
    patchOfGrass->setPosition(glm::vec3(3.3f, 0.0, -3.0));
    patchOfGrass->setSize(0.60f);
    patchOfGrass->setBlend(false);
    _objects.push_back(patchOfGrass);

    Model* fern = new Model("assets/grasses/Grass_01.obj", fetchShader("model.vtx", "model.frag"), this);
    fern->setPosition(glm::vec3(-2.0, 0.0, 1.0));
    fern->setBlend(false);
    _objects.push_back(fern);
}


void Scene::loadTerrains() {
    // TODO load other terrains
}

Camera* Scene::camera() {
    if (_c == nullptr ) throw std::runtime_error(std::string("camera access attempted, camera is null"));
    return _c;
};

LightSource* Scene::lightSource() {
    if (_lightSrc == nullptr )
        throw std::runtime_error(std::string("light source access attempted, light source is null"));
    return _lightSrc;
};

Terrain* Scene::currTerrain() {
    if (_currTerrain == nullptr )
        throw std::runtime_error(std::string("current terrain access attempted, current terrain is null"));
    return _currTerrain;
};

// If in debug mode, print the error but continue the program - otherwise, kill the program via EndProgramException
void Scene::handleErr(GLenum err) {
    switch (err) {
        // Error descriptions can be found at https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetError.xhtml
        case GL_INVALID_ENUM:
            if (!DEBUG) throw EndProgramException("GL Error found: invalid enum");
            std::cerr << "GL Error found: invalid enum" << std::endl;
            break;
        case GL_INVALID_VALUE:
            if (!DEBUG) throw EndProgramException("GL Error found: invalid value");
            std::cerr << "GL Error found: invalid value" << std::endl;
            break;
        case GL_INVALID_OPERATION:
            if (!DEBUG) throw EndProgramException("GL Error found: invalid operation");
            std::cerr << "GL Error found: invalid operation"  << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            if (!DEBUG) throw EndProgramException("GL Error found: invalid framebuffer operation");
            std::cerr << "GL Error found: invalid framebuffer operation"  << std::endl;
            break;

        // Certain errors should end the program regardless of the mode we're in
        case GL_OUT_OF_MEMORY: throw EndProgramException("GL Error found: out of memory");
        case GL_STACK_UNDERFLOW: throw EndProgramException("GL Error found: stack underflow");
        case GL_STACK_OVERFLOW: throw EndProgramException("GL Error found: stack overflow");

        default:
            if (!DEBUG) throw EndProgramException("GL Error found: unknown error: " + std::to_string(err));
            std::cerr << "GL Error found: unknown error: " << err << std::endl;
            break;
    }
}