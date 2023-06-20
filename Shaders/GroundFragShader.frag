#version 450 core

uniform sampler2D heightMap;
uniform sampler2D depthBuffer;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.8, 0.8, 0.8, 1.0);
//    FragColor.r = texture(heightMap, texCoord).r;
    FragColor.rgb = gl_FragCoord.zzz;
}
