#ifndef CONSTANTS_H
#define CONSTANTS_H

constexpr char terrainVertexShaderPath[] = "./Shaders/terrain.vs";
constexpr char terrainFragmentShaderPath[] = "./Shaders/terrain.fs";
constexpr char planeModelVertexShaderPath[] = "./Shaders/modelLoading.vs";
constexpr char planeModelFragmentShaderPath[] = "./Shaders/modelLoading.frag";
constexpr char skyboxVertexShaderPath[] = "./Shaders/skybox.vs";
constexpr char skyboxFragmentShaderPath[] = "./Shaders/skybox.frag";

constexpr char shaderViewProjMatUniformName[] = "viewProjMat";
constexpr char shaderModelMatUniformName[] = "model";
constexpr char shaderViewMatUniformName[] = "view";
constexpr char shaderProjMatUniformName[] = "projection";
constexpr char terrainShaderMinHeightUniformName[] = "gMinHeight";
constexpr char terrainShaderMaxHeightUniformName[] = "gMaxHeight";


constexpr char heightMapFilePath[] = "data\\heightmap.save";
constexpr char planeModelPath[] = "./Models/plane/Aereo O.obj";

constexpr char terrainTexture1Path[] = "./terrain/textures/rock1.jpg";
constexpr char terrainTexture2Path[] = "./terrain/textures/rock2.jpg";
constexpr char terrainTexture4Path[] = "./terrain/textures/grass1.png";
constexpr char terrainTexture3Path[] = "./terrain/textures/ice1.png";

#endif