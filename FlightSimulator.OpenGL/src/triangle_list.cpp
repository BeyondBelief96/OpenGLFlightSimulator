#include <stdio.h>
#include <vector>

#include "triangle_list.h"
#include "terrain.h"

void TriangleList::CreateTriangleList(int width, int depth, const BaseTerrain* pTerrain)
{
    // Set the dimensions of the terrain.
    m_width = width;
    m_depth = depth;

    // Initialize OpenGL state for rendering.
    CreateGLState();

    // Populate the buffer with vertex data based on the terrain.
    PopulateBuffers(pTerrain);

    // Unbind the VAO and VBOs to prevent accidental modification.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TriangleList::CreateGLState()
{
    // Generate a Vertex Array Object (VAO) and bind it.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Generate a Vertex Buffer Object (VBO) and bind it.
    glGenBuffers(1, &m_vb);
    glBindBuffer(GL_ARRAY_BUFFER, m_vb);

    glGenBuffers(1, &m_ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);

    // Define the location of the position attribute in the vertex shader.
    int POS_LOC = 0;
    glEnableVertexAttribArray(POS_LOC);

    // Set up the vertex attribute pointer for position.
    size_t NumFloats = 0;
    glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
    NumFloats += 3;
}

void TriangleList::PopulateBuffers(const BaseTerrain* pTerrain)
{
    // Create a vector of vertices sized based on the terrain dimensions.
    std::vector<TriangleList::Vertex> vertices;
    vertices.resize(m_width * m_depth);

    // Initialize the vertices based on the terrain data.
    InitVertices(pTerrain, vertices);

    std::vector<unsigned int> indices;
    int numQuads = (m_width - 1) * (m_depth - 1);
    indices.resize(numQuads * 6);
    InitIndices(indices);

    // Upload the vertex data to the GPU.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void TriangleList::Vertex::InitVertex(const BaseTerrain* pTerrain, int x, int z)
{
    float y = pTerrain->GetHeight(x, z);

    float worldScale = pTerrain->GetWorldScale();
    // Set the position of the vertex in the terrain.
    pos = glm::fvec3(worldScale * x, y, worldScale* z);
}

void TriangleList::InitIndices(std::vector<unsigned int>& indices)
{
    int index = 0;
    for (int z = 0; z < m_depth - 1; z++)
        for (int x = 0; x < m_width - 1; x++)
        {
            unsigned int indexBottomLeft = z * m_width + x;
            unsigned int indexTopLeft = (z + 1) * m_width + x;
            unsigned int indexTopRight = (z + 1) * m_width + x + 1;
            unsigned int indexBottomRight = z * m_width + x + 1;

            // Add top left triangle
            indices[index++] = indexBottomLeft;
            indices[index++] = indexTopLeft;
            indices[index++] = indexTopRight;

            // Add top right triangle
            indices[index++] = indexBottomLeft;
            indices[index++] = indexTopRight;
            indices[index++] = indexBottomRight;
        }
}

void TriangleList::InitVertices(const BaseTerrain* pTerrain, std::vector<Vertex>& vertices)
{
    // Iterate over each point in the terrain grid.
    int index = 0;
    for (int z = 0; z < m_depth; z++)
        for (int x = 0; x < m_width; x++)
        {
            // Ensure the current index is valid.
            assert(index < vertices.size());

            // Initialize each vertex with its position.
            vertices[index].InitVertex(pTerrain, x, z);
            index++;
        }
}

void TriangleList::Render()
{
    // Bind the VAO associated with this object.
    glBindVertexArray(m_vao);

    // Render the vertices as points.
    glDrawElements(GL_TRIANGLES, (m_depth - 1) * (m_width - 1) * 6, GL_UNSIGNED_INT, NULL);

    // Unbind the VAO to prevent accidental modification.
    glBindVertexArray(0);
}