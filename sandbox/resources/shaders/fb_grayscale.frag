#version 460 core

out vec4 FragColor;

in vec2 frag_texcoord;

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, frag_texcoord);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);
}