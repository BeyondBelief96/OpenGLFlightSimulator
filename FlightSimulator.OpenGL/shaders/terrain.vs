#version 330

layout (location = 0) in vec3 position;

uniform mat4 viewProjMat;

out vec4 Color;

void main()
{
	gl_Position = viewProjMat * vec4(position, 1.0);

	Color = vec4(position.y / 200.0);
}