#ifndef FAULT_FORMATION_TERRAIN_H
#define FAULT_FORMATION_TERRAIN_H

#include "terrain.h"

class FaultFormationTerrain : public BaseTerrain
{
public:

	FaultFormationTerrain() {};
	FaultFormationTerrain(const GLchar* vShaderPath, const GLchar* fShaderPath)
	: BaseTerrain(vShaderPath, fShaderPath) {}

	void CreateFaultFormation(int terrainSize, int iterations, float minHeight, float maxHeight, float FIRfilter);
private:
	struct TerrainPoint
	{
		int x = 0;
		int z = 0;

		void Print()
		{
			printf("[%d, %d]", x, z);
		}

		bool IsEqual(TerrainPoint& p) const
		{
			return ((x == p.x) && (z == p.z));
		}
	};

	void SetupShaderHeights(float minHeight, float maxHeight);
	void CreateFaultFormationInternal(int iterations, float minHeight, float maxHeight, float filter);
	float FIRFilterSinglePoint(int x, int z, float prevVal, float filter);
	void ApplyFIRFilter(float filter);
	void GenRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2);
};

#endif