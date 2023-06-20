#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
out vec3 norm;
out vec4 worldCoord;
out vec4 projCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    norm = cross(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                 gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
    norm = normalize(norm);

    worldCoord = model * gl_in[0].gl_Position;
    projCoord = proj * view * worldCoord;
    gl_Position = projCoord;
    EmitVertex();

    worldCoord = model * gl_in[1].gl_Position;
    projCoord = proj * view * worldCoord;
    gl_Position = projCoord;
    EmitVertex();

    worldCoord = model * gl_in[2].gl_Position;
    projCoord = proj * view * worldCoord;
    gl_Position = projCoord;
    EmitVertex();

    EndPrimitive();
}
