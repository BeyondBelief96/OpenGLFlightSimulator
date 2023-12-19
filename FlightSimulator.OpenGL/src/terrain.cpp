#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <glm/glm.hpp>

#include "utils.h"
#include "constants.h"
#include "terrain.h"
#include "texture_config.h"
#include "utils.h"

// Loads terrain data from a file
void BaseTerrain::LoadFromFile(const char* pFilename)
{
    // Load the heightmap from the specified file
    LoadHeightMapFile(pFilename);

    // Create a terrain grid using the terrain size and this terrain instance.
    // The grid is used for rendering the terrain.
    m_terrainGrid.CreateTerrainGrid(m_terrainSize, m_terrainSize, this);
}

// Initializes the terrain with world and texture scales and multiple textures
void BaseTerrain::InitTerrain(float WorldScale, float TextureScale, float minHeight, float maxHeight,
    const std::vector<string>& textureFilenames)
{
    // Ensure the number of provided textures matches the size of the texture array.
    // If not, print an error message and exit.
    if (textureFilenames.size() != ARRAY_SIZE_IN_ELEMENTS(m_textures)) {
        printf("%s:%d - number of provided textures (%lud) is not equal to the size of the texture array (%lud)\n",
            __FILE__, __LINE__, textureFilenames.size(), ARRAY_SIZE_IN_ELEMENTS(m_textures));
        exit(0);
    }

    // Set the world and texture scales, and min/max heights for the terrain.
    m_worldScale = WorldScale;
    m_textureScale = TextureScale;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_isSingleTexTerrain = false;

    // Load each texture from the provided filenames into the texture array.
    for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        m_textures[i] = new TextureLoader(GL_TEXTURE_2D);
        m_textures[i]->Load(textureFilenames[i]);
    }

    // Calculate the height factor for texture blending and set shader parameters.
    float heightFactor = (maxHeight - minHeight) / textureFilenames.size();
    terrainShader.Use();
    terrainShader.setInt("gTextureHeight0", 0);
    terrainShader.setInt("gTextureHeight1", 1);
    terrainShader.setInt("gTextureHeight2", 2);
    terrainShader.setInt("gTextureHeight3", 3);

    // Set texture heights for blending in the shader.
    SetTextureHeights(heightFactor, heightFactor * 2, heightFactor * 3, heightFactor * 4);
    SetMinMaxHeight(minHeight, maxHeight);
}

// Initializes the terrain with world and texture scales, assuming a single texture
void BaseTerrain::InitTerrain(float WorldScale, float TextureScale)
{
    // Set the world scale, texture scale, and flag indicating a single texture.
    m_worldScale = WorldScale;
    m_textureScale = TextureScale;
    m_isSingleTexTerrain = true;
}

// Retrieves the interpolated height at a given (x, z) position on the terrain
float BaseTerrain::GetHeightInterpolated(float x, float z) const
{
    // Get the base height at the nearest integer position.
    float BaseHeight = GetHeight((int)x, (int)z);

    // Check for boundary conditions to avoid accessing out-of-bounds data.
    if (((int)x + 1 >= m_terrainSize) || ((int)z + 1 >= m_terrainSize)) {
        return BaseHeight;
    }

    // Interpolate height along the X and Z axes for smoother terrain.
    float NextXHeight = GetHeight((int)x + 1, (int)z);
    float RatioX = x - floorf(x);
    float InterpolatedHeightX = (NextXHeight - BaseHeight) * RatioX + BaseHeight;

    float NextZHeight = GetHeight((int)x, (int)z + 1);
    float RatioZ = z - floorf(z);
    float InterpolatedHeightZ = (NextZHeight - BaseHeight) * RatioZ + BaseHeight;

    // Average the interpolated heights for a smoother transition.
    float FinalHeight = (InterpolatedHeightX + InterpolatedHeightZ) / 2.0f;

    return FinalHeight;
}

// Loads heightmap data from a file and initializes the terrain
void BaseTerrain::LoadHeightMapFile(const char* pFilename)
{
    int FileSize = 0;
    // Read the file as binary and get a pointer to its contents.
    unsigned char* p = reinterpret_cast<unsigned char*>(ReadBinaryFile(pFilename, FileSize));

    // Assert to ensure the file size is divisible by the size of a float.
    assert(FileSize % sizeof(float) == 0);

    // Calculate the size of the terrain based on the file size.
    m_terrainSize = sqrtf(FileSize / sizeof(float));

    // Initialize the heightmap array with data read from the file.
    m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, p);
}

// Renders the terrain using the provided camera
void BaseTerrain::Render(Camera& camera)
{
    // Get the combined view and projection matrix from the camera.
    glm::mat4 viewProjMatrix = camera.GetViewProjMatrix();

    // Use the terrain shader for rendering and set shader parameters.
    terrainShader.Use();
    terrainShader.setMat4(shaderViewProjMatUniformName, viewProjMatrix);
    terrainShader.setInt("gTextureHeight0", 0);
    terrainShader.setInt("gTextureHeight1", 1);
    terrainShader.setInt("gTextureHeight2", 2);
    terrainShader.setInt("gTextureHeight3", 3);

    // Set texture heights and min/max heights for the shader.
    SetTextureHeights((m_maxHeight - m_minHeight) / MAX_TEXTURES, (m_maxHeight - m_minHeight) / 2,
        3 * (m_maxHeight - m_minHeight) / 4, m_maxHeight - m_minHeight);
    SetMinMaxHeight(m_minHeight, m_maxHeight);

    // Bind the terrain textures before rendering.
    for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++)
    {
        if (m_textures[i])
        {
            m_textures[i]->Bind(COLOR_TEXTURE_UNIT_0 + i);
        }
    }

    // Render the terrain triangle list.
    m_terrainGrid.Render();

    glBindVertexArray(0);
}

void BaseTerrain::SetMinMaxHeight(float minHeight, float maxHeight)
{
    // Set the minimum and maximum height uniforms in the terrain shader.
    terrainShader.setFloat("gMinHeight", minHeight);
    terrainShader.setFloat("gMaxHeight", maxHeight);
}

void BaseTerrain::SetTextureHeights(float Tex0Height, float Tex1Height, float Tex2Height, float Tex3Height)
{
    // Set the texture height uniforms in the terrain shader for texture blending.
    terrainShader.setFloat("gHeight0", Tex0Height);
    terrainShader.setFloat("gHeight1", Tex1Height);
    terrainShader.setFloat("gHeight2", Tex2Height);
    terrainShader.setFloat("gHeight3", Tex3Height);
}
