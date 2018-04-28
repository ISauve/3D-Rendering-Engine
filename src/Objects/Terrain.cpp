#include "Object.h"
#include "../Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Terrain::Terrain(GLuint s, Scene* sc, std::string path) : Object(s, sc) {
    glUseProgram(_shaderProgram);

    // Load the height map image
    _heightMap = sf::Image();
    if (!_heightMap.loadFromFile(path)) std::cerr << "Error: error loading heightmap " << path << std::endl;

    int heightMapSize = _heightMap.getSize().x;

    // Calculate the heights and normals for each pixel of this map
    _heights.resize(heightMapSize);
    _normals.resize(heightMapSize);
    for (int i=0; i<heightMapSize; i++) {
        _heights[i].resize(heightMapSize);
        _normals[i].resize(heightMapSize);
    }
    for (int i=0; i<heightMapSize; i++) {
        for (int j=0; j<heightMapSize; j++) {
            float rawHeight = _heightMap.getPixel(i, j).r;  // Gives a # from 0-256
            float height = (rawHeight - 128) / 128;         // Get the range to be (-1)-1
            _heights[i][j] =  height * MAX_HEIGHT;
        }
    }
    for (int i=0; i<heightMapSize; i++) {
        for (int j=0; j<heightMapSize; j++) {
            // Conditionals are to make sure we don't index out of bounds
            float heightL = (i==0) ? _heights[0][j] : _heights[i-1][j];
            float heightR = (i==heightMapSize-1) ? _heights[0][j] : _heights[i+1][j];
            float heightU = (j==0) ? _heights[i][0] : _heights[i][j-1];
            float heightD = (j==heightMapSize-1) ? _heights[i][0] : _heights[i][j+1];
            vec3 normal = vec3(heightL - heightR, 1.0f, heightU - heightD);
            _normals[i][j] = glm::normalize(normal);
        }
    }

    // We'll dynamically generate a large square made up of triangles with _vertexCount vertices along each edge
    _vertexCount = heightMapSize;
    int totalVtcs = _vertexCount * _vertexCount;

    // Note that the square we will generate has its TOP LEFT CORNER at (0,0,0)
    GLfloat* positions = new GLfloat[totalVtcs * 3];
    GLfloat* normals = new GLfloat[totalVtcs * 3];
    int vtxCount = 0;
    for (int i=0; i<_vertexCount; i++) {
        for (int j=0; j<_vertexCount; j++) {
            float sideLength = SIZE / ((float)_vertexCount - 1);

            GLfloat x = (float) i * sideLength;
            GLfloat z = (float) j * sideLength;
            positions[vtxCount*3] = x;
            positions[vtxCount*3+1] = _heights[i][j];
            positions[vtxCount*3+2] = z;

            vec3 norm = _normals[i][j];
            normals[vtxCount*3] = norm.x;
            normals[vtxCount*3+1] = norm.y;
            normals[vtxCount*3+2] = norm.z;

            vtxCount++;
        }
    }
    _bufferIDs.push_back( storeToVBO(positions, sizeof(GLfloat) * totalVtcs * 3, normals, sizeof(GLfloat) * totalVtcs * 3) );

    // Generate the indices for drawing these triangles
    _numIndices = 6 * (_vertexCount-1) * (_vertexCount -1);
    GLuint* indices = new GLuint[_numIndices];
    int indexCount = 0;
    for (int i=0; i<_vertexCount-1; i++) {
        for (int j=0; j<_vertexCount-1; j++) {
            // Make a square out of 2 triangles
            GLuint topLeft = i * _vertexCount + j;  // adding vertexCount skips to "next row"
            GLuint topRight = topLeft + 1;          // adding 1 skips to "next column"
            GLuint bottomLeft = (i+1) * _vertexCount + j;
            GLuint bottomRight = bottomLeft + 1;

            indices[indexCount++] = topLeft;
            indices[indexCount++] = bottomLeft;
            indices[indexCount++] = topRight;

            indices[indexCount++] = topRight;
            indices[indexCount++] = bottomLeft;
            indices[indexCount++] = bottomRight;
        }
    }
    _bufferIDs.push_back( storeToEBO(indices, sizeof(GLuint) * _numIndices) );

    GLint posAttrib = glGetAttribLocation(_shaderProgram, "vPosition");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint normAttrib = glGetAttribLocation(_shaderProgram, "vNormal");
    glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GLfloat) * totalVtcs * 3));
    glEnableVertexAttribArray(normAttrib);

    unbind();
};

void Terrain::render() {
    // Bind the terrain's data
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Bind texture data (no effect if a texture isn't set)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    // The only transformation that applies to terrains is translation
    mat4 model = translate(mat4(1.0f), _position);

    // Pass the MVP matrix into our shader
    mat4 MVP = _scene->camera()->ProjMatrix() * _scene->camera()->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    // Set the appropriate lighting data
    if (_lit) {
        GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
        glUniform3fv(uniLightCol, 1, value_ptr(_scene->lightSource()->Color()));

        GLint uniLightPos = glGetUniformLocation(_shaderProgram, "lightPos");
        glUniform3fv(uniLightPos, 1, value_ptr(_scene->lightSource()->Position()));

        GLint uniModel = glGetUniformLocation(_shaderProgram, "Model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model));

        GLint uniPosn = glGetUniformLocation(_shaderProgram, "viewPos");
        glUniform3fv(uniPosn, 1, value_ptr(_scene->camera()->Position()));

    } else {
        // Set lightColor = 0 so the fragment shader knows not to try to light the shape
        GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
        glUniform3fv(uniLightCol, 1, value_ptr(vec3(0.0f)));
    }

    // Draw the terrain
    glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, nullptr);

    unbind();
}


void Terrain::set2DTexture(std::string path) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    // Dynamically determine the texture coordinates
    int totalVertices = _vertexCount * _vertexCount;
    GLfloat* textureCoords = new GLfloat[totalVertices * 2];
    int count = 0;
    for (int i=0; i<_vertexCount; i++) {
        for (int j=0; j<_vertexCount; j++) {
            // "Shrink" the displayed texture so that it repeats instead of being 1 large texture
            // and so that it always looks about the same, regardless of how large we make the terrain
            float shrinkFactor = SIZE / 2.0f;

            GLfloat x = (float) j /  ((float)_vertexCount - 1);
            GLfloat y = (float) i / ((float)_vertexCount - 1);
            textureCoords[count*2] = x * shrinkFactor;
            textureCoords[count*2+1] = y * shrinkFactor;
            count++;
        }
    }
    _bufferIDs.push_back( storeToVBO(textureCoords, sizeof(GLfloat) * totalVertices * 2) );

    _texture = storeTex(path, GL_REPEAT);
    _textureIDs.push_back( _texture );

    GLint colAttrib = glGetAttribLocation(_shaderProgram, "vTexture");
    glVertexAttribPointer(colAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colAttrib);

    glActiveTexture(GL_TEXTURE0);
    GLint uniTexSample = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniTexSample, 0);

    unbind();
}

float Terrain::barryCentric(vec3 p1, vec3 p2, vec3 p3, vec2 pos) {
    float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
    float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
    float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::getHeightAt(float worldX, float worldZ) {
    // Map these x and z coordinates to coords relative to terrain
    float terrainX = worldX - _position.x;
    float terrainZ = worldZ - _position.z;

    // Terrain is just a grid of squares - find which square this terrain coord is in
    float gridSqSz = SIZE / float(_heights.size());
    int gridX = floor(terrainX / gridSqSz);    // takes the floor
    int gridZ = floor(terrainZ / gridSqSz);

    if (gridX < 0 || gridZ < 0 || gridX >= _heights.size()-1 || gridZ >= _heights.size()-1)
        return 0;

    // Once scaled to "real size", a terrain grid can actually be pretty big - so we'll
    // calculate our relative position within this grid square, use that to figure out which
    // triangle we are standing in, & then use barycentric coordinates to find the height
    // at that exact spot
    float xCoord = std::fmod(terrainX, float(gridSqSz)) / gridSqSz;     // range 0-1
    float zCoord = std::fmod(terrainZ, float(gridSqSz)) / gridSqSz;     // range 0-1
    float preciseHeight;

    if (xCoord <= (1-zCoord)) {   // we're in the "top left" triangle of the grid square
        preciseHeight = barryCentric(vec3(0, _heights[gridX][gridZ], 0),     // top left grid vertex
                                     vec3(1, _heights[gridX+1][gridZ], 0),   // top right grid vertex
                                     vec3(0, _heights[gridX][gridZ+1], 1),   // bottom left grid vertex
                                     vec2(xCoord, zCoord));                  // our position
    }
    else {    // "bottom left" triangle
        preciseHeight = barryCentric(vec3(1, _heights[gridX+1][gridZ], 0),     // top right grid vertex
                                     vec3(1, _heights[gridX+1][gridZ+1], 1),   // bottom right grid vertex
                                     vec3(0, _heights[gridX][gridZ+1], 1),     // bottom left grid vertex
                                     vec2(xCoord, zCoord));                    // our position
    }
    return preciseHeight;
}

glm::vec3 Terrain::getNormalAt(int worldX, int worldZ) {
    float terrainX = worldX - _position.x;
    float terrainZ = worldZ - _position.z;
    float gridSqSz = SIZE / _heightMap.getSize().x;
    int x = terrainX / gridSqSz;
    int z = terrainZ / gridSqSz;
    if (x < 0 || z < 0 || x >= _heightMap.getSize().x || z >= _heightMap.getSize().y)
        return vec3(0.0f, 0.0f, 0.0f);
    return _normals[x][z];
}


// Set the bound data back to defaults
void Terrain::unbind() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}
