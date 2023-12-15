#ifndef TRIANGLE_LIST_H
#define TRIANGLE_LIST_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class BaseTerrain;

// TriangleList class is responsible for managing and rendering a list of triangles, typically for terrain.
class TerrainGrid
{
public:
    // Default constructor initializing OpenGL handles to zero.
    TerrainGrid() : m_vao(0), m_vb(0), m_ib(0) {};

    // Creates a triangle list for terrain rendering based on the specified dimensions and terrain data.
    // @param width: The width of the terrain in number of vertices.
    // @param depth: The depth of the terrain in number of vertices.
    // @param pTerrain: Pointer to the terrain data used for generating the triangle list.
    void CreateTerrainGrid(int width, int depth, const BaseTerrain* pTerrain);

    // Renders the triangle list, typically called every frame.
    void Render();

private:
    // Nested struct representing a vertex in the terrain mesh.
    struct Vertex {
        glm::fvec3 pos; // Position of the vertex in 3D space.
        glm::fvec2 tex; // 2D Position of the texture coordinate.

        // Initializes a vertex with position based on the terrain data and its coordinates.
        // @param pTerrain: Pointer to the terrain data used for vertex position calculations.
        // @param x: X-coordinate on the terrain.
        // @param z: Z-coordinate on the terrain.
        void InitVertex(const BaseTerrain* pTerrain, int x, int z);
    };

    // Initializes OpenGL state necessary for rendering the triangle list.
    void CreateGLState(void);

    // Populates the vertex buffer with data from the terrain.
    // @param pTerrain: Pointer to the terrain data used for populating the vertex buffer.
    void PopulateBuffers(const BaseTerrain* pTerrain);

    // Initializes vertex positions for all vertices in the terrain mesh.
    // @param pTerrain: Pointer to the terrain data used for vertex position calculations.
    // @param vertices: Reference to a vector of vertices to be initialized.
    void InitVertices(const BaseTerrain* pTerrain, std::vector<Vertex>& vertices);

    // Initializes the indices for the triangle list. This function should set up the `m_ib` buffer.
    // @param indices: Reference to a vector of unsigned integers to store the indices.
    void InitIndices(std::vector<unsigned int>& indices);

    int m_width = 0; // Width of the terrain in number of vertices.
    int m_depth = 0; // Depth of the terrain in number of vertices.
    GLuint m_vao; // OpenGL Vertex Array Object handle.
    GLuint m_vb; // OpenGL Vertex Buffer handle.
    GLuint m_ib; // OpenGL Index Buffer handle.
};

#endif

