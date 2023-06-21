#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float maxHeight;
uniform float gridSize;

in vec2 pos;

uniform sampler2D heightMap;

layout (binding = 2, rgba32f) uniform image2D data;

// out vec2 texCoord;
// out vec4 worldCoord;
// out vec4 projCoord;

void main()
{
    vec2 texCoord = pos / gridSize + vec2(0.5, 0.5);

    ivec2 id = ivec2(texCoord.xy * 1024.0);
    float height = imageLoad(data, ivec2(id.xy)).r;
    vec4 modelCoord = vec4(pos.x, height * maxHeight, pos.y, 1.0);
    // worldCoord = model * vec4(pos.x, texture(heightMap, texCoord).x * maxHeight, pos.y, 1.0);
    // projCoord = proj * view * worldCoord;
    gl_Position = modelCoord;
}
