#version 450 core

uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(colorBuffer, texCoord).rgb, 1);
}
