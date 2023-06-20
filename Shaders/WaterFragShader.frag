#version 450 core

uniform mat4 proj;
uniform float nearPanel;
uniform float farPanel;
uniform sampler2D depthBuffer;

in vec4 worldCoord;
in vec4 projCoord;

out vec4 FragColor;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPanel * farPanel) / (farPanel + nearPanel - z * (farPanel - nearPanel));
}

void main()
{
    vec2 texCoord = projCoord.xy / projCoord.ww / 2.0 + vec2(0.5, 0.5);
    float tmp = texture(depthBuffer, texCoord.xy).r;
    if (tmp > 0.995) discard;
    float groundDepth = LinearizeDepth(tmp);
    float waterDepth = LinearizeDepth(gl_FragCoord.z);
    if (groundDepth <= waterDepth) discard;
    FragColor = vec4(0.0, 0.4, 0.6, 1 - exp((waterDepth - groundDepth - 0.01) * 100));
//    FragColor = vec4(texture(depthBuffer, texCoord.xy).rgb, 1);
}
