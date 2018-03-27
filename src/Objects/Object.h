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
    GLuint storeToEBO(GLuint*, int);
    GLuint storeTex(std::string, GLenum = GL_REPEAT);
    GLuint storeCubeMap(std::vector<std::string>&);

public:
    Object(GLuint, Camera*, LightSource*);
    virtual ~Object();

    virtual void render() {};

    // Modifiers
    virtual void isLit(bool b) { _lit = b; };
    virtual void setPosition(glm::vec3 p) { _position = p; };
    virtual void setSize(float s) { _size = s; };
    virtual void setRotation(glm::vec3 axis) { _rotationAxis = axis; _rotationSpeed = 0; };
    virtual void setRotation(glm::vec3 axis, float speed) { _rotationAxis = axis; _rotationSpeed = speed; };
    // Note: Have to define 2 versions of setRotation bc can't set default arguments on virtual functions
};

/*************************************************************
                       Special Objects
 *************************************************************/

class SkyBox : public Object {
public:
    SkyBox(GLuint, Camera*);
    ~SkyBox() final { glDeleteProgram(_shaderProgram); };

    void render() override;

    // Set helpful error messages for base class modifiers that don't make sense
    void isLit(bool) override                   { std::cerr << "Error: can't change skybox lighting\n"; };
    void setPosition(glm::vec3) override        { std::cerr << "Error: can't change skybox position\n"; };
    void setSize(float) override                { std::cerr << "Error: can't change skybox size\n"; };
    void setRotation(glm::vec3) override        { std::cerr << "Error: can't change skybox rotation\n"; };
    void setRotation(glm::vec3, float) override { std::cerr << "Error: can't change skybox rotation\n"; };
};


class LightSource : public Object {
    glm::vec3 _onColor;
    glm::vec3 _color;
    bool _changed;

public:
    LightSource(GLuint, Camera*, glm::vec3, glm::vec3);
    ~LightSource() final { glDeleteProgram(_shaderProgram); };

    void render() override;

    // Accessors
    glm::vec3 Position() { return _position; };
    glm::vec3 Color() { return _color; };

    // Modifiers
    void setColor(glm::vec3);
    void isLit(bool) override;
    void setSize(float) override;

    // Base class modifiers that don't make sense
    void setRotation(glm::vec3) override        { std::cerr << "Error: rotation not implemented for light sources\n"; };
    void setRotation(glm::vec3, float) override { std::cerr << "Error: rotation not implemented for light sources\n"; };
};


class Terrain : public Object {
    const float SIZE = 800.0f;  // size of each square terrain object
    const int VTX_COUNT = 128;  // number of vertices along each side

    int _texture;
    int _heightMap;
    int _numIndices;

    void unbind();

public:
    Terrain(GLuint, Camera*, LightSource*);
    ~Terrain() final { glDeleteProgram(_shaderProgram); };

    void render() override;

    void set2DTexture(std::string);
    void setHeightMap(std::string);

    // Base class modifiers that don't make sense
    void setRotation(glm::vec3) override        { std::cerr << "Error: can't change terrain rotation\n"; };
    void setRotation(glm::vec3, float) override { std::cerr << "Error: can't change terrain rotation\n"; };
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
    virtual ~Shape() { glDeleteProgram(_shaderProgram); };

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

/*************************************************************
                          Models
 *************************************************************/

// Represents a small portion of a model
// Should never be instantiated outside of Model class
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


// Container that loads data using assimp & turns it into a collection of mesh objects
class Model : public Object {
    std::vector<Mesh*> _meshes;
    std::string _pathRoot;

    // Processing helpers
    void processNode(aiNode*, const aiScene*, Camera*, GLuint, LightSource*);
    Mesh* processMesh(aiMesh*, const aiScene*, Camera*, GLuint, LightSource*);
    std::vector<Mesh::Texture> getTextures(aiMaterial*, aiTextureType, std::string, std::string);

public:
    Model(std::string, Camera*, GLuint, LightSource*);
    ~Model() final;

    void render() override;

    // Modifiers
    void setBlend(bool);
    void isLit(bool) override;
    void setPosition(glm::vec3) override;
    void setSize(float) override;
    void setRotation(glm::vec3) override;
    void setRotation(glm::vec3, float) override;
};

#endif
