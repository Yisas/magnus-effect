#pragma once

#include "shader.hpp"
#include "mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glad/glad.h>

#include <string>
#include <vector>

/**
 * Texture from file.
 */
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model 
{
public:
    // model data
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    bool gammaCorrection;

    /**
     * Constructor, expects a filepath to a 3D model.
     */
    Model(std::string const &path, bool gamma = false);

    /**
     * Draws the model, and thus all its meshes.
     */
    void draw(Shader shader);
    
private:
    /**
     * Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
     */
    void loadModel(std::string const &path);

    /**
     * Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
     */
    void processNode(aiNode *node, const aiScene *scene);

    /**
     * Process the mesh.
     */
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    /**
     * Checks all material textures of a given type and loads the textures if they're not loaded yet.
     * The required info is returned as a Texture struct.
     */
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};