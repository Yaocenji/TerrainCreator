#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float maxHeight;
uniform float gridSize;

in vec2 pos;

uniform sampler2D heightMap;

out vec2 texCoord;
out vec4 worldCoord;

void main()
{
    texCoord = pos / gridSize + vec2(0.5, 0.5);
    worldCoord = view * model * vec4(pos.x, texture(heightMap, texCoord).x * maxHeight, pos.y, 1.0);
    gl_Position = proj * worldCoord;
}
