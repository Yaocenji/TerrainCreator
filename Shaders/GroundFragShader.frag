#version 450 core

uniform sampler2D heightMap;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture(heightMap, texCoord);
}
