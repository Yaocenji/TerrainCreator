#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float maxHeight;
uniform float gridSize;

in vec2 pos;

uniform sampler2D heightMap;

// out vec2 texCoord;
// out vec4 worldCoord;
// out vec4 projCoord;

void main()
{
    vec2 texCoord = pos / gridSize + vec2(0.5, 0.5);
    vec4 modelCoord = vec4(pos.x, texture(heightMap, texCoord).x * maxHeight, pos.y, 1.0);
    // worldCoord = model * vec4(pos.x, texture(heightMap, texCoord).x * maxHeight, pos.y, 1.0);
    // projCoord = proj * view * worldCoord;
    gl_Position = modelCoord;
}
