#version 330

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 Tex;
in vec3 worldPos;

uniform sampler2D gTextureHeight0;
uniform sampler2D gTextureHeight1;
uniform sampler2D gTextureHeight2;
uniform sampler2D gTextureHeight3;

uniform float gHeight0;
uniform float gHeight1;
uniform float gHeight2;
uniform float gHeight3;

vec4 CalcTexColorRealistic() {
    vec4 TexColor;
    float height = worldPos.y;

    // Define colors for different terrain types
    vec4 grassColor = vec4(0.25, 0.5, 0.17, 1.0);  // Green for grass
    vec4 dirtColor = vec4(0.545, 0.271, 0.075, 1.0);  // Brown for dirt
    vec4 rockColor = vec4(0.5, 0.5, 0.5, 1.0);  // Grey for rock
    vec4 snowColor = vec4(1.0, 1.0, 1.0, 1.0);  // White for snow

    float delta;
    float factor;

    if (height < gHeight0) {
        // Lower altitudes have grass
        TexColor = grassColor;
    } else if (height < gHeight1) {
        // Blend from grass to dirt
        delta = gHeight1 - gHeight0;
        factor = (height - gHeight0) / delta;
        TexColor = mix(grassColor, dirtColor, factor);
    } else if (height < gHeight2) {
        // Blend from dirt to rock
        delta = gHeight2 - gHeight1;
        factor = (height - gHeight1) / delta;
        TexColor = mix(dirtColor, rockColor, factor);
    } else if (height < gHeight3) {
        // Blend from rock to snow
        delta = gHeight3 - gHeight2;
        factor = (height - gHeight2) / delta;
        TexColor = mix(rockColor, snowColor, factor);
    } else {
        // Higher altitudes have snow
        TexColor = snowColor;
    }

    return TexColor;
}

vec4 CalcTexColor()
{
    vec4 TexColor;

    float height = worldPos.y;

    if(height < gHeight0)
    {
        TexColor = texture(gTextureHeight0, Tex);
    }
    else if(height < gHeight1)
    {
        vec4 color0 = texture(gTextureHeight0, Tex);
        vec4 color1 = texture(gTextureHeight1, Tex);
        float delta = gHeight1 - gHeight0;
        float factor = (height - gHeight0) / delta;
        TexColor = mix(color0, color1, factor);
    }
    else if (height < gHeight2) {
       vec4 color0 = texture(gTextureHeight1, Tex);
       vec4 color1 = texture(gTextureHeight2, Tex);
       float delta = gHeight2 - gHeight1;
       float factor = (height - gHeight1) / delta;
       TexColor = mix(color0, color1, factor);
    } else if (height < gHeight3) {
       vec4 color0 = texture(gTextureHeight2, Tex);
       vec4 color1 = texture(gTextureHeight3, Tex);
       float delta = gHeight3 - gHeight2;
       float factor = (height - gHeight2) / delta;
       TexColor = mix(color0, color1, factor);
    } else {
       TexColor = texture(gTextureHeight3, Tex);
    }

    return TexColor;
}

void main()
{
    vec4 TexColor = CalcTexColor();
    FragColor = Color * TexColor;
}