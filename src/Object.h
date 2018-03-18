#ifndef OPENGL_OBJECT_H
#define OPENGL_OBJECT_H

#include <glad/glad.h>
#include <string>
#include <chrono>
#include <vector>

#include "Camera.h"
class Camera;

/*************************************************************
                     Abstract Base Class
 *************************************************************/
class Object {
protected:
    GLuint _shaderProgram;
    GLuint _vao;
    GLuint _vbo;
    Camera* _c;

    // Helpers
    GLuint initializeVAO();
    GLuint storeToVBO(GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int);
    GLuint storeToEBO(GLuint*, int);
    GLuint storeTex(std::string, GLenum);
    GLuint storeCubeMap(std::vector<std::string>&);

public:
    Object(GLuint s, Camera* c) : _shaderProgram(s), _c(c) {};

    virtual void render() {};
    virtual void cleanUp() {};
};


/*************************************************************
                          Children
 *************************************************************/

class SkyBox : public Object {
    GLuint _ebo;
    GLuint _cubeTex;

public:
    SkyBox(GLuint s, Camera* c);
    void render() override;
    void cleanUp() override;
};


class RotatingShape : public Object {
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    GLuint _ebo;
    GLuint _tex;

    void initialize2DTriangle();
    void initialize3DSquare();
    void initializePyramid();
    void initializeStonePyramid();

    void render2DTriangle();
    void render3DSquare();
    void renderPyramid();
    void renderStonePyramid();

public:
    enum Type {TRIANGLE_2D, SQUARE_3D, PYRAMID, STONE_PYRAMID};

    RotatingShape(GLuint, Camera*, Type);

    void render() override;
    void cleanUp() override;

private:
    Type _type;
};

#endif
