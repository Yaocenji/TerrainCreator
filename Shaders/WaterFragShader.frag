#version 450 core

uniform mat4 proj;
uniform float nearPanel;
uniform float farPanel;

in vec4 worldCoord;

out vec4 FragColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPanel * farPanel) / (farPanel + nearPanel - z * (farPanel - nearPanel));
}

void main()
{
    float groundDepth = LinearizeDepth(gl_FragCoord.z);
    vec4 tmp = proj * worldCoord;
    float waterDepth = tmp.z;
    FragColor = vec4(0.0, 0.5, 1.0, 1 - exp(waterDepth - groundDepth));
}
