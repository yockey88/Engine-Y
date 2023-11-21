#version 460 core

in vec3 frag_color;
in vec2 frag_texcoord;

out vec4 FragColor;

uniform int lines;

void main() {
    vec4 color = vec4(frag_color, 1.0);

    if (color.r < 0.0)  {
        color.r = abs(frag_color.r);
    }    

    if (color.g < 0.0) {
        color.g = abs(frag_color.g);
    }

    if (color.b < 0.0) {
        color.b = abs(frag_color.b); 
    }

    if (lines == 1) {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }

    FragColor = color;
}
