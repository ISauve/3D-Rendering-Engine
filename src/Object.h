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

    virtual void render() {};
    virtual void cleanUp() {};
};

// Global texture counter
extern int textureCounter;

/*************************************************************
                          Children
 *************************************************************/

class SkyBox : public Object {
    GLuint _ebo;
    GLuint _cubeTex;

public:
    SkyBox(GLuint, Camera*);
    void render() override;
    void cleanUp() override;
};



class LightSource : public Object {
    GLuint _ebo;
    glm::vec3 _position;
    glm::vec3 _color;

public:
    LightSource(GLuint, Camera*, glm::vec3, glm::vec3);
    void render() override;
    void cleanUp() override;

    glm::vec3 Position() { return _position; };
    glm::vec3 Color() { return _color; };
};



class Ground : public Object {
    GLuint _ebo;
    GLuint _tex;

public:
    Ground(GLuint, Camera*);
    void render() override;
    void cleanUp() override;
};



class Shape : public Object {
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    GLuint _ebo;
    GLuint _tex;

    glm::vec3 _lightPos;
    glm::vec3 _lightColor;

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

    Shape(GLuint, Camera*, Type, glm::vec3 = glm::vec3(1.0f), glm::vec3 = glm::vec3(1.0f));

    void render() override;
    void cleanUp() override;

private:
    Type _type;
};



class Cube : public Object {
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;

    LightSource* _lightSrc;
    bool _lit;

    glm::vec3 _position;
    float _size;
    glm::vec3 _rotationAxis;
    float _rotationSpeed;

public:
    Cube(GLuint, Camera*, LightSource*);

    void render() override;
    void cleanUp() override;

    // Modifiers
    void isLit(bool b) { _lit = b; };
    void setPosition(glm::vec3 p) { _position = p; };
    void setSize(float s) { _size = s; };
    void setRotation(glm::vec3 axis, float speed) { _rotationAxis = axis; _rotationSpeed = speed; };

    void setColor(glm::vec3);           // Applies a uniform color
    void setColors(GLfloat*, int);      // Applies a custom color data for each vertex
    void set2DTexture(std::string);     // Applies texture to each face
};

#endif
