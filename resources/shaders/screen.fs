#version 330 core

in VertexData
{
   vec2 texCoords;
} i;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(screenTexture, i.texCoords).rgb, 1.0);
}
