#ifndef OPENGL_OBJECT_H
#define OPENGL_OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
class Camera;

/*************************************************************
                         Main Class
 *************************************************************/
class Object {
protected:
    GLuint _shaderProgram;
    Camera _c;

    void storeToVBO(GLfloat*, int);
    void storeToVBO(GLfloat*, int, GLfloat*, int);
    void storeToEBO(GLuint*, int);

public:
    Object(GLuint, Camera&);

    virtual void render(float) = 0;
};


/*************************************************************
                          Children
 *************************************************************/

class Triangle : Object {
public:
    explicit Triangle(GLuint s, Camera& c) : Object(s, c) {};

    void render(float) override;
};

class Square : Object {
public:
    Square(GLuint s, Camera& c) : Object(s, c) {};

    void render(float) override;
};

class RotatingSquare : Object {
public:
    RotatingSquare(GLuint s, Camera& c) : Object(s, c) {};

    void render(float) override;
};

class RotatingSquare3D : Object {
public:
    RotatingSquare3D(GLuint s, Camera& c) : Object(s, c) {};

    void render(float) override;
};

class RotatingPyramid : Object {
public:
    RotatingPyramid(GLuint s, Camera& c) : Object(s, c) {};

    void render(float) override;
};

#endif
