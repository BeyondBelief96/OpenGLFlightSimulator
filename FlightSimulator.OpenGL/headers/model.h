#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "constants.h"
#include "camera.h"

using namespace std;

class Model
{
public:
    // Default constructor
    Model() {};

    // Constructor that loads a model from a file
    Model(const GLchar* path, const GLchar* vShaderPath, const GLchar* fShaderPath)
        : modelShader(vShaderPath, fShaderPath)
    {
        this->loadModel(path);  // Load the model when an instance of Model is created
    }

    // Renders the model using the given model matrix and camera.
    void Render(glm::mat4 modelMatrix, Camera& camera)
    {
        modelShader.Use();  // Activate the shader before rendering
        // Set the model and view-projection matrices in the shader
        modelShader.setMat4(shaderModelMatUniformName, modelMatrix);
        modelShader.setMat4(shaderViewProjMatUniformName, camera.GetViewProjMatrix());

        // Draw each mesh in the model
        for (GLuint i = 0; i < this->meshes.size(); i++)
        {
            this->meshes[i].Draw(modelShader);
        }

        glBindVertexArray(0);
    }

    void DeleteBuffers()
    {
        for (auto mesh : meshes)
        {
            mesh.DeleteBuffers();
        }
    }

private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;
    Shader modelShader;

    // Loads a model with supported ASSIMP extensions from file
    void loadModel(string path)
    {
        Assimp::Importer importer;
        // Read the file and apply post-processing steps like triangulation
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

        // Check for errors in loading
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
            return;
        }

        // Extract the directory part of the file path
        directory = path.substr(0, path.find_last_of('/'));

        // Start processing the nodes recursively
        processNode(scene->mRootNode, scene);
    }

    // Processes a node in a recursive fashion
    void processNode(aiNode* node, const aiScene* scene)
    {
        // Process all the meshes in the current node
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene)); // Process and store each mesh
        }

        // Recursively process each of the child nodes
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    // Converts an ASSIMP mesh to a Mesh object
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        // Process vertices
        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // Placeholder vector for transferring data

            // Assign vertex position from ASSIMP's data structure
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Process normals if available
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // Process texture coordinates if available
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // Process indices
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // Retrieve all indices of the face and store them
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // Process material textures
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    // Checks and loads textures of a given type if they haven't been loaded yet
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            // Check if texture was already loaded
            for (GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(this->textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // Skip if already loaded
                    break;
                }
            }
            if (!skip)
            {
                // Load texture if not loaded
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // Add to loaded textures
            }
        }
        return textures;
    }

    // Loads a texture file and creates a texture object in OpenGL
    GLint TextureFromFile(const char* path, string directory)
    {
        string filename = string(path);
        filename = directory + '/' + filename; // Construct full file path

        GLuint textureID;
        glGenTextures(1, &textureID); // Generate texture ID

        int width, height, nrComponents;
        // Load the texture file data
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            // Determine the texture format
            GLenum format = GL_RGB;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            // Bind the texture and set its data and parameters
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps

            // Set texture wrapping and filtering options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

            stbi_image_free(data); // Free the image data
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data); // Free the image data in case of failure
        }

        return textureID;
    }
};

#endif
