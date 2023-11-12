#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texcoord;

out vec2 frag_texcoord;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    frag_texcoord = in_texcoord;
    gl_Position = proj * view * model * vec4(in_pos , 1.0);
}