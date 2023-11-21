#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

out vec3 frag_color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    frag_color = in_pos;
    gl_Position = proj * view * model * vec4(in_pos , 1.0);
}
