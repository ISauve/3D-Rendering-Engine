#include "Object.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb_image.h"

Object::Object(GLuint s, Camera* c, LightSource* l) :_shaderProgram(s), _c(c), _lightSrc(l),
    _lit(true), _position(glm::vec3(0.0)), _size(1.0f), _rotationAxis(glm::vec3(0.0)), _rotationSpeed(0.0f) {
    _start = std::chrono::high_resolution_clock::now();
    _vao = initializeVAO();
};


Object::~Object() {     // Note: Gets called after each child class' destructor is finished
    glDeleteVertexArrays(1, &_vao);

    for (auto it : _bufferIDs)
        glDeleteBuffers(1, &it);
    _bufferIDs.clear();

    for (auto it : _textureIDs)
        glDeleteTextures(1, &it);
    _textureIDs.clear();

    // Don't delete shader program because some subclasses (ie Meshes) share them
}

// Create & bind a vertex array object
GLuint Object::initializeVAO() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    return vao;
}

// Create, bind, and load data into a vertex buffer object
GLuint Object::storeToVBO(GLfloat* vertices, int size) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    return vbo;
}

GLuint Mesh::storeToVBO(Mesh::Vertex* vertices, long size) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    return vbo;
}

// Create, bind, and load data into a vertex buffer object
GLuint Object::storeToVBO(GLfloat* positions, int sizeP, GLfloat* colors, int sizeC) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // We'll use glBufferSubData to load the data in 2 parts
    glBufferData(GL_ARRAY_BUFFER,  // target
                 sizeP + sizeC,    // total size
                 NULL,             // no data (yet)
                 GL_STATIC_DRAW);  // method

    glBufferSubData(GL_ARRAY_BUFFER,  // target
                    0,                // no offset
                    sizeP,            // size
                    positions);       // data

    glBufferSubData(GL_ARRAY_BUFFER,  // target
                    sizeP,            // offset = sizeof previous data entered
                    sizeC,            // size
                    colors);          // data
    return vbo;
}

// Create, bind, and load data into an element buffer object
GLuint Object::storeToEBO(GLuint* indices, int size) {
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    return ebo;
}

// Texture cache
std::unordered_map<std::string, GLuint> textureCache;

// Create, bind, and load data into a 2D texture object
GLuint Object::storeTex(std::string path, GLenum wrapping) {

    // Check if this particular texture has already been loaded & return its ID if so
    if (textureCache[path]) {
        if (DEBUG) std::cout << "Skipping loading of " << path << ": returning cached version" << std::endl;
        return textureCache[path];
    }

    GLuint tex;
    glGenTextures(1, &tex);
    textureCache[path] = tex;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)  {
        GLenum format = GL_RGB;
        if (nrChannels == 1)        format = GL_RED;
        else if (nrChannels == 4)   format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (DEBUG) std::cout << path << " loaded" << std::endl;

    } else std::cerr << path << " failed to load" << std::endl;

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

    if (wrapping == GL_CLAMP_TO_BORDER) {
        // Specify a border color
        float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };   // black
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    return tex;
}

// Create, bind, and load data into a texture cube map
GLuint Object::storeCubeMap(std::vector<std::string>& faces) {
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)  {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << faces[i] << " failed to load." << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return tex;
}

