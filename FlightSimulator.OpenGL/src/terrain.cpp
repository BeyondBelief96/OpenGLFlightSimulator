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

void BaseTerrain::LoadFromFile(const char* pFilename)
{
	LoadHeightMapFile(pFilename);
	m_triangleList.CreateTriangleList(m_terrainSize, m_terrainSize, this);
}

void BaseTerrain::LoadHeightMapFile(const char* pFilename)
{
	int FileSize = 0;
	unsigned char* p = reinterpret_cast<unsigned char*>(ReadBinaryFile(pFilename, FileSize));

	assert(FileSize % sizeof(float) == 0);
	m_terrainSize = sqrtf(FileSize / sizeof(float));
	m_heightMap.InitArray2D(m_terrainSize, m_terrainSize, p);
}

void BaseTerrain::Render(Camera& camera)
{
	glm::mat4 viewProjMatrix = camera.GetViewProjMatrix();
	terrainShader.Use();
	terrainShader.setMat4(shaderViewProjMatUniformName, viewProjMatrix);
	m_triangleList.Render();
}