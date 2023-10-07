#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform samplerCube skybox;

void main( )
{
    color = texture(skybox, TexCoords);
}
