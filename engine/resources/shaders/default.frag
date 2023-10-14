#version 460 core

in vec3 frag_color;
in vec2 frag_texcoord;

out vec4 FragColor;

uniform vec3 view_pos;
uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
    FragColor = vec4(frag_color , 1.0); 
}