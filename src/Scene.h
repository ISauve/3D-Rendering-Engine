#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include "Objects/Object.h"

#include <vector>

class Scene {
    Camera* _c;

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

    // Camera modifiers
    void Look(double x, double y) { _c->Look(x, y); };
    void Move(Direction d) { _c->Move(d); };
    void Zoom(float z) { _c->Zoom(z); };
    void isDucking(bool b) { _c->isDucking(b); };
    void Jump() { _c->Jump(); };
};


#endif //OPENGL_SCENE_H
