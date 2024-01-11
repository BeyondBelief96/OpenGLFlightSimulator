#ifndef TERRAIN_H
#define TERRAIN_H

#include <ogldev_array_2d.h>
#include "terrain_grid.h"
#include "camera.h"
#include "shader.h"
#include "3rdParty/ogldev_texture.h"

#define MAX_TEXTURES 4

// BaseTerrain class encapsulates the logic for rendering and managing terrain data in a 3D environment.
class BaseTerrain
{
public:
    // Default constructor.
    BaseTerrain() = default;

    // Constructor that initializes the terrain with shader paths and a world scaling factor.
    // @param vShaderPath: Path to the vertex shader file.
    // @param fShaderPath: Path to the fragment shader file.
    // @param worldScale: Scale factor to adjust the size of the terrain in the world space.
    BaseTerrain(const GLchar* vShaderPath, const GLchar* fShaderPath)
        : terrainShader(vShaderPath, fShaderPath) {}

    // Initializes the terrain with specific world and texture scales and a list of texture filenames.
    // @param WorldScale: Scale factor for the world.
    // @param TextureScale: Scale factor for the texture.
    // @param minHeight: Minimum height for the terrain.
    // @param maxHeight: Maximum height for the terrain.
    // @param TextureFilenames: Vector of texture filenames to be used for the terrain.
    void InitTerrain(float WorldScale, float TextureScale, float minHeight, float maxHeight,
        const std::vector<string>& TextureFilenames);

    // Overloaded initialization function for terrain without specifying texture filenames.
    // Only sets world and texture scales.
    // @param WorldScale: Scale factor for the world.
    // @param TextureScale: Scale factor for the texture.
    void InitTerrain(float WorldScale, float TextureScale);

    // Renders the terrain from the perspective of the given camera.
    // @param camera: Camera object that provides the view and projection matrices.
    void Render(Camera& camera);

    // Loads the terrain data from a specified file.
    // @param pFilename: File path to the terrain data file.
    void LoadFromFile(const char* pFilename);

    // Gets the height at a specific (x, z) coordinate on the terrain.
    // @param x: X-coordinate on the terrain.
    // @param z: Z-coordinate on the terrain.
    // @return: Height at the specified coordinates.
    float GetHeight(int x, int z) const { return m_heightMap.Get(x, z); }

    // Gets the interpolated height at a non-integer (x, z) position on the terrain.
    // Provides smoother height transitions between grid points.
    // @param x: X-coordinate on the terrain.
    // @param z: Z-coordinate on the terrain.
    // @return: Interpolated height at the specified position.
    float GetHeightInterpolated(float x, float z) const;

    // Gets the size of the terrain grid. Assumes the terrain is a square for simplicity.
    // @return: Size of one side of the terrain grid.
    float GetSize() const { return m_terrainSize; }

    // Gets the scale of the terrain textures in world space.
    // @return: The scaling factor applied to the terrain textures.
    float GetTextureScale() const { return m_textureScale; }

    // Sets the texture to be used by the terrain.
    // @param pTexture: Pointer to the texture to be set.
    void SetTexture(TextureLoader* pTexture) { m_textures[0] = pTexture; }

    // Gets the scale of the terrain in world space.
    // @return: The scaling factor applied to the terrain.
    float GetWorldScale() const { return m_worldScale; }

    // Sets the terrains shader uniform variables for the heights to be used in the blending process
    // in the fragment shader.
    // @param tex0Height: Height for the first texture transition.
    // @param tex1Height: Height for the second texture transition.
    // @param tex2Height: Height for the third texture transition.
    // @param tex3Height: Height for the fourth texture transition.
    void SetTextureHeights(float tex0Height, float tex1Height, float tex2Height, float tex3Height);

protected:
    // Loads heightmap data from a specified file.
    // The heightmap defines the elevation at different points on the terrain.
    // @param pFilename: Path to the heightmap file.
    void LoadHeightMapFile(const char* pFilename);

    // Sets the minimum and maximum height uniforms in the terrains fragment shader.
    // @param MinHeight: The minimum height of the terrain.
    // @param MaxHeight: The maximum height of the terrain.
    void SetMinMaxHeight(float MinHeight, float MaxHeight);

    // Size of the terrain grid.
    int m_terrainSize = 0;

    // Scaling factor for the terrain in world space.
    float m_worldScale = 1.0f;

    // 2D array representing the heightmap of the terrain.
    Array2D<float> m_heightMap;

    // TerrainGrid object for managing and rendering the terrain geometry.
    TerrainGrid m_terrainGrid;

    // Shader program used for rendering the terrain.
    Shader terrainShader;

    // Array of texture loaders for loading terrain textures.
    TextureLoader* m_textures[MAX_TEXTURES] = { nullptr };

    // Texture scaling factor.
    float m_textureScale = 1.0f;

    // Flag indicating whether the terrain uses a single texture.
    bool m_isSingleTexTerrain = false;

    // Minimum height of the terrain.
    float m_minHeight = 0.0f;

    // Maximum height of the terrain.
    float m_maxHeight = 0.0f;
};

#endif // TERRAIN_H
