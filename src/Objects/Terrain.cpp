#include "Object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Terrain::Terrain(GLuint s, Camera* c, LightSource* l) : Object(s, c, l) {
    glUseProgram(_shaderProgram);

    // We'll dynamically generate a large square made up of triangles with VTX_COUNT vertices along each edge
    int totalVtcs = VTX_COUNT * VTX_COUNT;

    // Note that the square we will generate has its TOP LEFT CORNER at (0,0,0)
    GLfloat* positions = new GLfloat[totalVtcs * 3];
    GLfloat* normals = new GLfloat[totalVtcs * 3];
    int vtxCount = 0;
    for (int i=0; i<VTX_COUNT; i++) {
        for (int j=0; j<VTX_COUNT; j++) {
            // Create a triangle
            float sideLength = SIZE / ((float)VTX_COUNT - 1);
            GLfloat x = (float) j * sideLength;
            GLfloat z = (float) i * sideLength;
            positions[vtxCount*3] = x;
            positions[vtxCount*3+1] = 0;   // Set Y = 0 to start (-> flat terrain)
            positions[vtxCount*3+2] = z;

            // Since this is a flat terrain, set the normal straight up (0, 1, 0)
            normals[vtxCount*3] = 0;
            normals[vtxCount*3+1] = 1;
            normals[vtxCount*3+2] = 0;

            vtxCount++;
        }
    }
    _bufferIDs.push_back( storeToVBO(positions, sizeof(GLfloat) * totalVtcs * 3, normals, sizeof(GLfloat) * totalVtcs * 3) );

    _numIndices = 6 * (VTX_COUNT-1) * (VTX_COUNT -1);
    GLuint* indices = new GLuint[_numIndices];
    int indexCount = 0;
    for (int i=0; i<VTX_COUNT-1; i++) {
        for (int j=0; j<VTX_COUNT-1; j++) {
            // Make a square out of 2 triangles
            GLuint topLeft = i * VTX_COUNT + j;
            GLuint topRight = topLeft + 1;
            GLuint bottomLeft = (i+1) * VTX_COUNT + j;
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
    GLint uniLoc = glGetUniformLocation(_shaderProgram, "sampleTexture");
    glUniform1i(uniLoc, 0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glActiveTexture(GL_TEXTURE1);
    uniLoc = glGetUniformLocation(_shaderProgram, "sampleHeightMap");
    glUniform1i(uniLoc, 1);
    glBindTexture(GL_TEXTURE_2D, _heightMap);

    // Generate the model matrix (scale -> rotate -> translate)
    mat4 model = translate(mat4(1.0f), _position) * scale(mat4(1.0f), vec3(_size, _size, _size));

    // Pass the MVP matrix into our shader
    mat4 MVP = _c->ProjMatrix() * _c->ViewMatrix() * model;
    GLint uniTransform = glGetUniformLocation(_shaderProgram, "MVP");
    glUniformMatrix4fv(uniTransform, 1, GL_FALSE, value_ptr(MVP));

    // Set the appropriate lighting data
    if (_lit) {
        GLint uniLightCol = glGetUniformLocation(_shaderProgram, "lightColor");
        glUniform3fv(uniLightCol, 1, value_ptr(_lightSrc->Color()));

        GLint uniLightPos = glGetUniformLocation(_shaderProgram, "lightPos");
        glUniform3fv(uniLightPos, 1, value_ptr(_lightSrc->Position()));

        GLint uniModel = glGetUniformLocation(_shaderProgram, "Model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model));

        GLint uniPosn = glGetUniformLocation(_shaderProgram, "viewPos");
        glUniform3fv(uniPosn, 1, value_ptr(_c->Position()));

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
    int totalVertices = VTX_COUNT * VTX_COUNT;
    GLfloat* textureCoords = new GLfloat[totalVertices * 2];
    int count = 0;
    for (int i=0; i<VTX_COUNT; i++) {
        for (int j=0; j<VTX_COUNT; j++) {
            // "Shrink" the displayed texture so that it repeats instead of being 1 large texture
            // and so that it always looks about the same, regardless of how large we make the terrain
            float shrinkFactor = SIZE;

            GLfloat x = (float) j /  ((float)VTX_COUNT - 1);
            GLfloat y = (float) i / ((float)VTX_COUNT - 1);
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

void Terrain::setHeightMap(std::string path) {
    glBindVertexArray(_vao);
    glUseProgram(_shaderProgram);

    _heightMap = storeTex(path, GL_REPEAT);
    _textureIDs.push_back( _heightMap );

    glActiveTexture(GL_TEXTURE1);
    GLint uniTexSample = glGetUniformLocation(_shaderProgram, "sampleHeightMap");
    glUniform1i(uniTexSample, 1);

    unbind();
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
