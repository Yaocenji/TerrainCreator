#version 450 core

uniform sampler2D heightMap;
uniform sampler2D depthBuffer;

// in vec2 texCoord;
in vec3 norm;

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(vec3(0.0, 0.2, 0.1));
    vec3 lightCol = vec3(1.0, 0.8667, 0.6784);
    vec3 baseCol;
    if (norm.y >= 0.5)
        baseCol = vec3(0.0941, 0.6314, 0.102);
    else
        baseCol = vec3(0.6157, 0.6706, 0.6275);
    
    vec3 ambient = 0.1 * baseCol;
    vec3 diffuse = baseCol * lightCol * (dot(norm, lightDir) * 0.5 + 0.5);
    vec3 ans = ambient + diffuse;

    vec3 newNorm = norm / 2 + vec3(0.5, 0.5, 0.5);
    FragColor = vec4(ans, 1.0);
//    FragColor.rgb = texture(heightMap, texCoord).rgb;
}
