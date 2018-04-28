#ifndef OPENGL_SCENE_H
#define OPENGL_SCENE_H

#include "Objects/Object.h"
#include "Camera.h"

#include <vector>

struct EndProgramException : public std::exception {
    std::string info;
    EndProgramException(std::string i) : info(i) {}
    const char * what () const throw () { return info.c_str(); }
};

class Scene {
    Camera* _c;
    SkyBox* _skybox;
    LightSource* _lightSrc;
    Terrain* _currTerrain;

    std::vector<Object*> _objects;

    bool _isLit;

    void loadShapes();
    void loadModels();
    void loadTerrains();

    void handleErr(GLenum); // Can throw a EndProgramException

public:
    Scene(Camera* c);
    ~Scene();

    void draw(); // Can throw a EndProgramException
    void toggleLight();

    // Accessors - can all throw std::runtime_error exception
    Camera* camera();
    LightSource* lightSource();
    Terrain* currTerrain();

    // Camera modifiers
    void Look(double x, double y) { _c->Look(x, y); };
    void Move(Direction d) { _c->Move(d); };
    void Zoom(float z) { _c->Zoom(z); };
    void isDucking(bool b) { _c->isDucking(b); };
    void Jump() { _c->Jump(); };
};


#endif //OPENGL_SCENE_H
