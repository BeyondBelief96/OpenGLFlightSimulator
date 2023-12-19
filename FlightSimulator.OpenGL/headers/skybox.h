#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <stb_image/stb_image.h>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include "constants.h"
#include "data.h"

class Skybox
{
public:

    // Default Constructor
    Skybox() = default;

    // Destructor
    ~Skybox() {
        Cleanup();
    }

    // Copy Constructor (Rule of Three)
    Skybox(const Skybox& other) {
        // Perform deep copy or resource duplication
        VAO = other.VAO;
        VBO = other.VBO;
        textureID = other.textureID;
    }

    // Copy Assignment Operator (Rule of Three)
    Skybox& operator=(const Skybox& other) {
        if (this != &other) {
            Cleanup();
            VAO = other.VAO;
            VBO = other.VBO;
            textureID = other.textureID;
        }
        return *this;
    }

    // Move Constructor (Rule of Five)
    Skybox(Skybox&& other) noexcept : VAO(other.VAO), VBO(other.VBO), textureID(other.textureID) {
        other.VAO = 0;
        other.VBO = 0;
        other.textureID = 0;
    }

    // Move Assignment Operator (Rule of Five)
    Skybox& operator=(Skybox&& other) noexcept {
        if (this != &other) {
            Cleanup();
            VAO = other.VAO;
            VBO = other.VBO;
            textureID = other.textureID;
            other.VAO = 0;
            other.VBO = 0;
            other.textureID = 0;
        }
        return *this;
    }

    void Initialize(const std::vector<std::string>& faces) 
    {
        // Initialize the skybox vertex and fragment shaders.
            LoadShaders(skyboxVertexShaderPath, skyboxFragmentShaderPath);

        // Generate and bind the VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Load the cubemap textures
        loadCubemap(faces);

        //Set skybox sampler uniform for texture mapping.
        skyboxShader.Use();
        skyboxShader.setInt("skybox", 0);
    }

    void Cleanup() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (textureID) glDeleteTextures(1, &textureID);
        VAO = 0;
        VBO = 0;
        textureID = 0;
    }


    // Render: Renders the skybox.
    void Render(Camera& camera)
    {
        // Change depth function so depth test passes when values are equal to depth buffer's 
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Use();
        // Removing translation portion of the view matrix
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4(shaderViewMatUniformName, view);
        skyboxShader.setMat4(shaderProjMatUniformName, camera.GetProjectionMatrix());
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // reset OpenGL settings.
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default
    }

private:
    GLuint textureID;   // Texture ID for the cubemap.
    GLuint VAO, VBO;    // VAO and VBO for the skybox vertices.
    Shader skyboxShader; // Enapsulates logic for loading skybox shaders.

    // loadCubemap: Loads the cubemap textures from the provided file paths.
    void loadCubemap(const std::vector<std::string>& faces)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(false); // Flip the textures on load
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum format = 3;
                if (nrChannels == 1)
                    format = GL_RED;
                else if (nrChannels == 3)
                    format = GL_RGB;
                else if (nrChannels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void LoadShaders(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath)
    {
        skyboxShader = Shader(vertexShaderPath, fragmentShaderPath);
    }
};

#endif // SKYBOX_H
