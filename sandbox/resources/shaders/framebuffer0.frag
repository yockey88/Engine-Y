#version 460 core

out vec4 FragColor;

in vec2 frag_texcoord;

uniform sampler2D screen_tex;

void main() {
    vec3 col = texture(screen_tex, frag_texcoord).rgb;
    FragColor = vec4(col , 1.0);
}