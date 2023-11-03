#version 460 core

out vec4 FragColor;

in vec2 frag_texcoord;

uniform sampler2D tex;

const float offset = 1.0 / 300.0;

const vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2(0.0f, offset), // top-center
    vec2(offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2(0.0f, 0.0f), // center-center
    vec2(offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2(0.0f, -offset), // bottom-center
    vec2(offset, -offset) // bottom-right    
);

const float kernel[9] = float[](
    1 ,  1 , 1 , 
    1 , -8 , 1 ,
    1 ,  1 , 1   
);

void main() {
    vec3 sample_tex[9];
    for (int i = 0; i < 9; ++i) {
        sample_tex[i] = vec3(texture(tex, frag_texcoord.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; ++i)
        col += sample_tex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}