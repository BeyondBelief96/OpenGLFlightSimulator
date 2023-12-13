#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr char terrainVertexShaderPath[] = "./Shaders/terrain.vs";
constexpr char terrainFragmentShaderPath[] = "./Shaders/terrain.fs";
constexpr char planeModelVertexShaderPath[] = "./Shaders/modelLoading.vs";
constexpr char planeModelFragmentShaderPath[] = "./Shaders/modelLoading.frag";

constexpr char shaderViewProjMatUniformName[] = "viewProjMat";
constexpr char shaderModelMatUniformName[] = "model";
constexpr char shaderViewMatUniformName[] = "view";
constexpr char shaderProjMatUniformName[] = "projection";


constexpr char heightMapFilePath[] = "data\\heightmap.save";
constexpr char planeModelPath[] = "./Models/plane/Aereo O.obj";

#endif