#version 450

layout(location=0) in vec3 vertex_pos;
layout(location=1) in vec4 vertex_color;

layout(location=10) out vec4 frag_color;
layout(location=11) out vec4 frag_pos;

void main() {
    frag_color = vertex_color;
    frag_pos = vec4(vertex_pos, 1.0);
}
