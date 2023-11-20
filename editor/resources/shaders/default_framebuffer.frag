#version 460 core

in vec2 fs_texcoords;

out vec4 FragColor;

uniform sampler2D framebuffer_tex;

void main() {
    FragColor = texture(framebuffer_tex , fs_texcoords);
}
