
#version 460 core

layout (location = 0) in vec3 in_pos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * view * model * vec4(in_pos , 1.0);
}