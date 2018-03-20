#ifndef OPENGL_OBJECT_H
#define OPENGL_OBJECT_H

#include <glad/glad.h>
#include <string>
#include <chrono>
#include <vector>

#include "../Camera.h"
class Camera;

/*************************************************************
                     Abstract Base Class
 *************************************************************/
class Object {
protected:
    GLuint _shaderProgram;

    GLuint _vao;
    std::vector <GLuint> _bufferIDs;
    std::vector <GLuint> _textureIDs;

    Camera* _c;

    // Helpers
    GLuint initializeVAO();
    GLuint storeToVBO(GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int, GLfloat*, int);
    GLuint storeToEBO(GLuint*, int);
    GLuint storeTex(std::string, GLenum);
    GLuint storeCubeMap(std::vector<std::string>&);

public:
    Object(GLuint s, Camera* c) : _shaderProgram(s), _c(c) {};
    ~Object();

    virtual void render() {};
};

// Global texture counter
extern int textureCounter;

/*************************************************************
                       Special Objects
 *************************************************************/

class SkyBox : public Object {
public:
    SkyBox(GLuint, Camera*);

    void render() override;
};


class LightSource : public Object {
    glm::vec3 _position;
    glm::vec3 _color;
    bool _changed;

public:
    LightSource(GLuint, Camera*, glm::vec3, glm::vec3);

    void render() override;

    // Accessors
    glm::vec3 Position() { return _position; };
    glm::vec3 Color() { return _color; };

    // Modifiers
    void setPosition(glm::vec3);
    void setColor(glm::vec3);
};


/*************************************************************
                          Shapes
 *************************************************************/

class Shape : public Object {
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;

    LightSource* _lightSrc;
    bool _lit;

    glm::vec3 _position;
    float _size;
    glm::vec3 _rotationAxis;
    float _rotationSpeed;

protected:
    int _numElements;
    bool _usesIndices;

public:
    Shape(GLuint, Camera*, LightSource*);

    void render() override;

    // Modifiers
    void isLit(bool b) { _lit = b; };
    void setPosition(glm::vec3 p) { _position = p; };
    void setSize(float s) { _size = s; };
    void setRotation(glm::vec3 axis, float speed) { _rotationAxis = axis; _rotationSpeed = speed; };
};

class Cube : public Shape {
public:
    Cube(GLuint, Camera*, LightSource*);

    void setColor(glm::vec3);           // Applies a uniform color
    void setColors(GLfloat*, int);      // Applies a custom color data for each vertex
    void set2DTexture(std::string);     // Applies texture to each face
};

class Square : public Shape {
public:
    Square(GLuint, Camera*, LightSource*);

    void set2DTexture(std::string);
    // TODO add rotation, color, transparency, etc
};

// TODO pyramid

/*************************************************************
                          Models
 *************************************************************/

// TODO

#endif
