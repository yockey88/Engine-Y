#version 460 core

in vec2 frag_texcoord;

out vec4 FragColor;

uniform sampler2D msdf;

uniform float px_range; // set to distance field's pixel range
uniform vec4 bgcolor;
uniform vec4 fgcolor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float ScreenPxRange(vec2 texcoord) {
    vec2 unit_range = vec2(px_range) / vec2(textureSize(msdf , 0));
    vec2 screen_tex_size = vec2(1.0) / fwidth(texcoord);
    return max(0.5 * dot(unit_range , screen_tex_size) , 1.0);
}

void main() {
    vec3 msd = texture(msdf , frag_texcoord).rgb;
    
    float sd = median(msd.r , msd.g , msd.b);
    float screen_px_distance = ScreenPxRange(frag_texcoord) * (sd - 0.5);
    float opacity = clamp(screen_px_distance + 0.5 , 0.0 , 1.0);
   
    FragColor = mix(bgcolor , fgcolor , opacity);
}