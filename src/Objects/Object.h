#ifndef OPENGL_OBJECT_H
#define OPENGL_OBJECT_H

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <chrono>
#include <vector>
#include <unordered_map>

#include "../Camera.h"
class Camera;
class LightSource;

static bool DEBUG = false;

/*************************************************************
                   Abstract Base Classes
 *************************************************************/

class Object {
protected:
    GLuint _shaderProgram;
    GLuint _vao;

    Camera* _c;
    LightSource* _lightSrc;

    // Buffer ID storage for clean-up purposes: individually references may also be kept
    std::vector <GLuint> _bufferIDs;
    std::vector <GLuint> _textureIDs;

    // State information
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    bool _lit;
    glm::vec3 _position;
    float _size;
    glm::vec3 _rotationAxis;
    float _rotationSpeed;

    // Helpers
    GLuint initializeVAO();
    GLuint storeToVBO(GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int, GLfloat*, int);
    GLuint storeToEBO(GLuint*, int);
    GLuint storeTex(std::string, GLenum = GL_REPEAT);
    GLuint storeCubeMap(std::vector<std::string>&);

public:
    Object(GLuint, Camera*, LightSource*);
    ~Object();

    virtual void render() {};

    // Modifiers
    void isLit(bool b) { _lit = b; };
    void setPosition(glm::vec3 p) { _position = p; };
    void setSize(float s) { _size = s; };
    void setRotation(glm::vec3 axis, float speed = 0) { _rotationAxis = axis; _rotationSpeed = speed; };
};

/*************************************************************
                       Special Objects
 *************************************************************/

class SkyBox : public Object {
public:
    SkyBox(GLuint, Camera*);

    void render() override;
};

class LightSource : public Object {
    glm::vec3 _onColor;
    glm::vec3 _color;
    bool _changed;

public:
    LightSource(GLuint, Camera*, glm::vec3, glm::vec3);

    void render() override;

    // Accessors
    glm::vec3 Position() { return _position; };
    glm::vec3 Color() { return _color; };

    // Modifiers
    void setColor(glm::vec3);
    void toggle();
};


/*************************************************************
                          Shapes
 *************************************************************/

class Shape : public Object {
protected:
    int _texture;
    int _numElements;
    bool _usesIndices;

    void unbind();

public:
    Shape(GLuint, Camera*, LightSource*);

    void render() override;
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
};

// TODO pyramid

/*************************************************************
                          Models
 *************************************************************/

class Mesh : public Object {
public:     // Forward declarations
    struct Vertex;
    struct Texture;

private:
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    bool _blend;

    // Overridden version for this class only (defn in Object.cpp)
    GLuint storeToVBO(Mesh::Vertex*, long);
    void unbind();

public:
    Mesh(GLuint, Camera*, LightSource*);

    void render() override;

    // Modifiers
    void addData(std::vector<Vertex>, std::vector<unsigned int>, std::vector<Texture>);
    void setBlend(bool b) { _blend = b; };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };
    struct Texture {
        std::string name;
        std::string path;
        GLuint id;
    };
};

// A container that loads data using assimp & turns it into a collection of mesh objects
class Model {
    std::vector<Mesh*> _meshes;
    std::string _pathRoot;

    // Processing helpers
    void processNode(aiNode*, const aiScene*, Camera*, GLuint, LightSource*);
    Mesh* processMesh(aiMesh*, const aiScene*, Camera*, GLuint, LightSource*);
    std::vector<Mesh::Texture> getTextures(aiMaterial*, aiTextureType, std::string, std::string);

public:
    Model(std::string, Camera*, GLuint, LightSource*);

    // Define common interface with objects
    void render();
    void isLit(bool b);
    void setRotation(glm::vec3 axis, float speed = 0);
    void setPosition(glm::vec3);
    void setSize(float);

    // Other modifiers
    void setBlend(bool b);
};

#endif
