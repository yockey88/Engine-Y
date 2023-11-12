#version 460 core

in vec3 frag_color;
in vec2 frag_texcoord;

out vec4 FragColor;

const float px_range = 2.0; // set to distance field's pixel range
uniform sampler2D tex0;
uniform vec4 bg_color;
uniform vec4 fg_color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float ScreenPxRange(vec2 texcoord) {
    vec2 unit_range = vec2(px_range) / vec2(textureSize(tex0 , 0));
    vec2 screen_tex_size = vec2(1.0) / fwidth(texcoord);
    return max(0.5 * dot(unit_range , screen_tex_size) , 1.0);
}

void main() {
    vec3 msd = texture(tex0 , frag_texcoord).rgb;
    
    float sd = median(msd.r , msd.g , msd.b);
    float screen_px_distance = ScreenPxRange(frag_texcoord) * (sd - 0.5);
    float opacity = clamp(screen_px_distance + 0.5 , 0.0 , 1.0);
   
    FragColor = mix(bg_color , fg_color , opacity);
}