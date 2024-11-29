#version 450

in vec3 frag_pos;
in vec4 frag_color;

layout(location = 0) out vec4 final_frag_color;


void main() {
    // final_frag_color = frag_color;
    final_frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
