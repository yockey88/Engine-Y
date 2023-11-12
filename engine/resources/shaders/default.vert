#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec2 in_tangent;
layout (location = 4) in vec2 in_bitangent;
layout (location = 5) in vec2 in_texcoord;
layout (location = 6) in float in_opacity;

out vec3 frag_color;
out vec2 frag_texcoord;

uniform bool camera_active = false;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform float world_scale = 1.0;

void main() {
    gl_Position = proj * view * model * vec4(in_pos , 1.0);

    frag_texcoord = vec2(1.0 - in_texcoord.x , 1.0- in_texcoord.y);
}