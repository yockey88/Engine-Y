#version 460 core

in vec3 frag_pos;
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;
in float frag_opacity;

out vec4 FragColor;

uniform vec3 view_pos;

// textures
uniform sampler2D height_map;

void main() {
    FragColor = texture(height_map, frag_texcoord);
}