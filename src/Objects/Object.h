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

static bool DEBUG = false;

/*************************************************************
                     Abstract Base Class
 *************************************************************/
class Object {
protected:
    GLuint _shaderProgram;
    GLuint _vao;

    // Buffer ID storage for clean-up purposes: individually references may also be kept
    std::vector <GLuint> _bufferIDs;
    std::vector <GLuint> _textureIDs;

    Camera* _c;

    // Helpers
    GLuint initializeVAO();
    GLuint storeToVBO(GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int);
    GLuint storeToVBO(GLfloat*, int, GLfloat*, int, GLfloat*, int);
    GLuint storeToEBO(GLuint*, int);
    GLuint storeTex(std::string, GLenum = GL_REPEAT);
    GLuint storeCubeMap(std::vector<std::string>&);

public:
    Object(GLuint s, Camera* c) : _shaderProgram(s), _c(c) {};
    ~Object();

    virtual void render() {};
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
    int _texture;

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

class Mesh : public Object {
public:     // Forward declarations
    struct Vertex;
    struct Texture;
private:

    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    glm::vec3 _position;
    float _size;

    // Some models dont have RGBA textures so blending messes with them
    bool _blend;

    // Overridden version for this class only (defn in Object.cpp)
    GLuint storeToVBO(Mesh::Vertex*, long);

public:
    Mesh(GLuint, Camera*);

    void addData(std::vector<Vertex>, std::vector<unsigned int>, std::vector<Texture>);

    void render() override;

    void setBlend(bool b) { _blend = b; };
    void setPosition(glm::vec3 p) { _position = p; };
    void setSize(float s) { _size = s; };

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

    void processNode(aiNode*, const aiScene*, Camera*, GLuint);
    Mesh* processMesh(aiMesh*, const aiScene*, Camera*, GLuint);
    std::vector<Mesh::Texture> getTextures(aiMaterial*, aiTextureType, std::string, std::string);

public:
    Model(std::string, Camera*, GLuint);

    void render();
    void setBlend(bool b);
    void setPosition(glm::vec3);
    void setSize(float);
};

#endif
