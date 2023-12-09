#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec2 vert_tex;

out vec2 frag_tex;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * view * model * vec4(vert_pos , 1.0);
}
