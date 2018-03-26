#include "Object.h"
#include "../Shaders.h"

Model::Model(std::string path, Camera* c, GLuint shader, LightSource* l) : Object(shader, c, l) {
    // Load the model into an assimp scene object
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
                                             aiProcess_Triangulate // We want all primitives to be triangles
                                             | aiProcess_FlipUVs); // Flip the texture y-coordinate where necessary
    /*
     * Note: Other useful options are
     *  aiProcess_GenNormals - creates normals for each vtx if the model didn't have them
     *  aiProcess_SplitLargeMeshes - splits larges meshes into smaller sub-meshes
     *  aiProcess_OptimizeMeshes - joins smaller meshes into a larger meshes
     */

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error loading model at " << path << ": " << importer.GetErrorString() << std::endl;
        return;
    }

    _pathRoot = path.substr(0, path.find_last_of('/'));

    // Recursively processes the nodes & saves the generated meshes in the _meshes vector
    processNode(scene->mRootNode, scene, c, shader, l);
    if (DEBUG) std::cout << "Succesfully loaded data for model: " << _pathRoot << std::endl;
}

Model::~Model() {
    glDeleteProgram(_shaderProgram);

    for (auto it : _meshes)
        delete it;  // Just calls the Object destructor
    _meshes.clear();
};

void Model::processNode(aiNode* node, const aiScene* scene, Camera* c, GLuint s, LightSource* l) {
    // Process all the meshes
    for (int i=0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene, c, s, l));
    }

    // Recurse on the node's children
    for (int i=0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene, c, s, l);
}

// List of texture types: http://assimp.sourceforge.net/lib_html/material_8h.html#a7dd415ff703a2cc53d1c22ddbbd7dde0
std::vector<Mesh::Texture> Model::getTextures(aiMaterial* mat, aiTextureType type, std::string name_prefix, std::string path) {
    std::vector<Mesh::Texture> textures;
    for (int i = 0; i < mat->GetTextureCount(type); i++) {
        // Get the texture name
        aiString str;
        mat->GetTexture(type, i, &str);

        Mesh::Texture tex;
        // Because we may have multiple textures for a single drawing operation, we
        // give each a unique name which we will bind to a unique texture unit
        tex.name = name_prefix + std::to_string(i);
        tex.path = path + "/" + str.C_Str();
        textures.push_back(tex);
    }
    return textures;
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, Camera* c, GLuint s, LightSource* l) {
    Mesh* newMesh = new Mesh(s, c, l);

    std::vector<Mesh::Vertex> vertices;
    std::vector<unsigned int> indices;

    // Retrieve vertex data
    for(int i=0; i < mesh->mNumVertices; i++) {
        Mesh::Vertex vtx;

        // Position
        glm::vec3 tempVec;
        tempVec.x = mesh->mVertices[i].x;
        tempVec.y = mesh->mVertices[i].y;
        tempVec.z = mesh->mVertices[i].z;
        vtx.position = tempVec;

        // Normal
        tempVec.x = mesh->mNormals[i].x;
        tempVec.y = mesh->mNormals[i].y;
        tempVec.z = mesh->mNormals[i].z;
        vtx.normal = tempVec;

        // Texture coordinates (if they exist)
        glm::vec2 tempVec2 = glm::vec2(0.0f);
        if (mesh->mTextureCoords[0]) {
            tempVec2.x = mesh->mTextureCoords[0][i].x;
            tempVec2.y = mesh->mTextureCoords[0][i].y;
        }
        vtx.texCoords = tempVec2;

        vertices.push_back(vtx);
    }

    // Retrieve index data
    for (int i=0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j=0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Retrieve material (texture) data
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    auto ambient = getTextures(mat, aiTextureType_AMBIENT, "ambient_texture_", _pathRoot);
    auto diffuse = getTextures(mat, aiTextureType_DIFFUSE, "diffuse_texture_", _pathRoot);
    auto specular = getTextures(mat, aiTextureType_SPECULAR, "specular_texture_", _pathRoot);

    std::vector<Mesh::Texture> textures;
    textures.reserve( ambient.size() + diffuse.size() + specular.size() );
    textures.insert( textures.end(), ambient.begin(), ambient.end() );
    textures.insert( textures.end(), diffuse.begin(), diffuse.end() );
    textures.insert( textures.end(), specular.begin(), specular.end() );

    newMesh->addData(vertices, indices, textures);
    return newMesh;
}

void Model::render() {
    for (auto it : _meshes)
        it->render();
}

void Model::setBlend(bool b) {
    for (auto it : _meshes)
        it->setBlend(b);
}

void Model::isLit(bool b) {
    for (auto it: _meshes)
        it->isLit(b);
}

void Model::setPosition(glm::vec3 p) {
    for (auto it : _meshes)
        it->setPosition(p);
};

void Model::setSize(float s) {
    for (auto it : _meshes)
        it->setSize(s);
};

void Model::setRotation(glm::vec3 direction) {
    for (auto it : _meshes)
        it->setRotation(direction, 0);
}


void Model::setRotation(glm::vec3 direction, float speed) {
    for (auto it : _meshes)
        it->setRotation(direction, speed);
}
