#ifndef TERRAIN_H
#define TERRAIN_H

#include <ogldev_array_2d.h>
#include "triangle_list.h"

#include "camera.h"
#include "shader.h"

// BaseTerrain class represents a terrain in a 3D environment.
// It encapsulates the logic for rendering terrain and managing its data.
class BaseTerrain
{
public:
    // Constructor for BaseTerrain.
    // Initializes the terrain with vertex and fragment shader paths and a scaling factor for the world.
    // @param vShaderPath: Path to the vertex shader file.
    // @param fShaderPath: Path to the fragment shader file.
    // @param worldScale: A scaling factor for the terrain in the world space.
    BaseTerrain(const GLchar* vShaderPath, const GLchar* fShaderPath, float worldScale) 
        : terrainShader(vShaderPath, fShaderPath)
    {
        m_worldScale = worldScale;
    }

    // Renders the terrain using the provided camera.
    // @param camera: The camera through which the terrain is viewed.
    void Render(Camera& camera);

    // Loads terrain data from a file.
    // @param pFilename: Path to the file containing terrain data.
    void LoadFromFile(const char* pFilename);

    // Retrieves the height at a specific (x, z) coordinate.
    // @param x: X-coordinate on the terrain.
    // @param z: Z-coordinate on the terrain.
    // @return: Height at the given coordinate.
    float GetHeight(int x, int z) const { return m_heightMap.Get(x, z); }

    // Returns the scale of the terrain in the world space.
    // @return: World scale factor of the terrain.
    float GetWorldScale() const { return m_worldScale; }

protected:
    // Loads heightmap data from a file.
    // @param pFilename: Path to the heightmap file.
    void LoadHeightMapFile(const char* pFilename);

    // Size of the terrain (assuming square terrain for simplicity).
    int m_terrainSize = 0;

    // Scaling factor for the terrain in world space.
    float m_worldScale = 1.0f;

    // 2D array representing the heightmap of the terrain.
    Array2D<float> m_heightMap;

    // TriangleList used for managing and rendering the geometry of the terrain.
    TriangleList m_triangleList;

    // Shader program used for rendering the terrain.
    Shader terrainShader;
};

#endif
