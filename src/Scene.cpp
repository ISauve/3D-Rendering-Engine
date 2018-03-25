#include "Scene.h"
#include "Shaders.h"

using namespace std;

Scene::Scene(Camera* c) : _c(c), _isLit(true) {
    auto timer = chrono::high_resolution_clock::now();

    // Create the skybox
    _skybox = new SkyBox(fetchShader("cubemap.vtx", "cubemap.frag"), c);

    // Create the light source (Note: needs to be done before loading any shapes/models)
    glm::vec3 lightPos(0.5f, 1.3f, 1.5f);
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
    _lightSrc = new LightSource(fetchShader("phongShader.vtx", "phongShader.frag"), c, lightPos, lightCol);

    loadShapes(c);
    loadModels(c);

    if (DEBUG) {
        std::chrono::duration<double> loadingTime = chrono::high_resolution_clock::now() - timer;
        std::cout << "Loaded scene data in " << loadingTime.count() << "s" << std::endl;
    }
}

Scene::~Scene() {
    delete _skybox;
    delete _lightSrc;
    for (auto it : _shapes)
        delete it;
    for (auto it : _models)
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
    _skybox->render();  // must always be 1st
    _lightSrc->render();
    for (auto it : _shapes)
        it->render();
    for (auto it : _models)
        it->render();

    // Check for problems
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printErr(err);

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
    _lightSrc->toggle();
    _isLit = !_isLit;

    auto state = (_isLit) ? "on" : "off";
    if (DEBUG) std::cout << "Lighting turned " << state << std::endl;

    for (auto it : _shapes)
        it->isLit(_isLit);
    for (auto it : _models)
        it->isLit(_isLit);
}

void Scene::loadShapes(Camera* c) {
    Square* ground = new Square(fetchShader("phongShader.vtx", "phongShader.frag"), c, _lightSrc);
    ground->set2DTexture("assets/grass.jpg");
    ground->setSize(5.0f);

    Cube* cube1 = new Cube(fetchShader("phongShader.vtx", "phongShader.frag"), c, _lightSrc);
    cube1->set2DTexture("assets/crate.jpeg");
    cube1->setPosition(glm::vec3(0.7, 0.7, 2.0));
    cube1->setRotation(glm::vec3(0.0, -1.0, 0.0), 0.05);
    cube1->setSize(0.1);

    Cube* cube2 = new Cube(fetchShader("phongShader.vtx", "phongShader.frag"), c, _lightSrc);
    cube2->set2DTexture("assets/stones.jpg");
    cube2->setPosition(glm::vec3(-0.2, 0.65, 0.5));
    cube2->setSize(0.15);
    cube2->setRotation(glm::vec3(0.5, 1.0, 1.0));

    Cube* cube3 = new Cube(fetchShader("phongShader.vtx", "phongShader.frag"), c, _lightSrc);
    cube3->set2DTexture("assets/metal.jpg");
    cube3->setPosition(glm::vec3(0.8, 0.9, -0.3));
    cube3->setSize(0.3);
    cube3->setRotation(glm::vec3(1.0, 0.0, 0.0), 0.03);

    _shapes.push_back(ground);
    _shapes.push_back(cube1);
    _shapes.push_back(cube2);
    _shapes.push_back(cube3);
}

void Scene::loadModels(Camera* c) {
    Model* nanosuit = new Model("assets/nanosuit/nanosuit.obj", c, fetchShader("model.vtx", "model.frag"), _lightSrc);
    nanosuit->setPosition(glm::vec3(3.0, 0.0, 2.0));
    nanosuit->setRotation(glm::vec3(0.0, -1.0, 0.0));
    nanosuit->setSize(0.06f);
    nanosuit->setBlend(false);
    _models.push_back(nanosuit);

    Model* tree = new Model("assets/Tree/Tree.obj", c, fetchShader("model.vtx", "model.frag"), _lightSrc);
    tree->setPosition(glm::vec3(2.0, 0.0, -2.5));
    _models.push_back(tree);

    for (int i=0; i<4; i++) {
        Model* tallGrass = new Model("assets/grasses/Grass_03.obj", c, fetchShader("model.vtx", "model.frag"), _lightSrc);
        Model* patchOfGrass = new Model("assets/grasses/Grass_02.obj", c, fetchShader("model.vtx", "model.frag"), _lightSrc);

        if (i == 0) {
            tallGrass->setPosition(glm::vec3(0.0, 0.0, -4.7));
            patchOfGrass->setPosition(glm::vec3(-3.0f, 0.0, -3.0));
        }
        else if (i == 1) {
            tallGrass->setPosition(glm::vec3(0.0, 0.0, 5.0));
            patchOfGrass->setPosition(glm::vec3(0.0f, 0.0, -3.0));
        }
        else if (i == 2) {
            tallGrass->setPosition(glm::vec3(5.0, 0.0, 0.0));
            tallGrass->setRotation(glm::vec3(0.0, 1.0, 0.0));
            patchOfGrass->setPosition(glm::vec3(3.3f, 0.0, -3.0));
        } else {
            tallGrass->setPosition(glm::vec3(-5.0, 0.0, 0.0));
            tallGrass->setRotation(glm::vec3(0.0, -1.0, 0.0));
            patchOfGrass->setPosition(glm::vec3(3.3f, 0.0, -3.0));
        }
        tallGrass->setSize(0.27f);
        patchOfGrass->setSize(0.60f);
        tallGrass->setBlend(false);
        patchOfGrass->setBlend(false);

        _models.push_back(tallGrass);
        _models.push_back(patchOfGrass);
    }

    Model* fern = new Model("assets/grasses/Grass_01.obj", c, fetchShader("model.vtx", "model.frag"), _lightSrc);
    fern->setPosition(glm::vec3(-2.0, 0.0, 1.0));
    fern->setBlend(false);
    _models.push_back(fern);
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