#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include "Objects/Object.h"

#include <vector>

class Scene {
    SkyBox* _skybox;
    LightSource* _lightSrc;
    std::vector<Shape*> _shapes;
    std::vector<Model*> _models;

    bool _isLit;

    void loadShapes(Camera*);
    void loadModels(Camera*);
    void printErr(GLenum);

public:
    Scene(Camera* c);
    ~Scene();

    void draw();
    void toggleLight();
};


#endif //OPENGL_SCENE_H
