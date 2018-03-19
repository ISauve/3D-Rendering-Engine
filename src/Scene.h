#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include "Object.h"

#include <vector>

class Scene {
    std::vector<Object*> _objects;
    SkyBox* _skybox;
    LightSource* _lightSrc;

    static int _texID;

    void printErr(GLenum);

public:
    Scene(Camera* c);

    void draw();
    void cleanUp();
};


#endif //OPENGL_SCENE_H
