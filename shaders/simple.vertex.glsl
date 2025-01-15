#version 450

layout(location=25) uniform mat4 mvp = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0
);

layout(location=0) in vec3 vertex_pos;
layout(location=1) in vec4 vertex_color;

layout(location=10) out vec4 frag_pos;
layout(location=11) out vec4 frag_color;

void main() {
    frag_color = vertex_color;
    frag_pos = mvp * vec4(vertex_pos, 1.0);
    gl_Position = frag_pos;
}
