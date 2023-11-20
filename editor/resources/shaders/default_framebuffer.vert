#version 460 core

layout (location = 0) in vec2 vs_pos;
layout (location = 1) in vec2 vs_texcoords;

out vec2 fs_texcoords;

void main() {
    fs_texcoords = vs_texcoords;
    gl_Position = vec4(vs_pos.x , vs_pos.y , 0.0 , 1.0);
}
