#version 460 core

in vec2 frag_tex;

out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
    FragColor = mix(texture(tex0 , frag_tex) , texture(tex1 , frag_tex) , 0.2);
}
