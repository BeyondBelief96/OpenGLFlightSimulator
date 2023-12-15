#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 inTex;

uniform mat4 viewProjMat;
uniform float gMinHeight;
uniform float gMaxHeight;

out vec4 Color;
out vec2 Tex;
out vec3 worldPos;

void main()
{
	gl_Position = viewProjMat * vec4(position, 1.0);

	float deltaHeight = gMaxHeight - gMinHeight;

	float heightRatio = (position.y - gMinHeight) / deltaHeight;

	float c = heightRatio * 0.8 + 0.02;

	Color = vec4(c, c, c, 1.0f);

	Tex = inTex;

	worldPos = position;
}