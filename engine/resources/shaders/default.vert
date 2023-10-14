#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texcoord;

out vec3 frag_color;
out vec2 frag_texcoord;

uniform bool camera_active = false;

uniform mat4 view;
uniform mat4 proj;

uniform float world_scale = 1.0;

void main() {
    vec3 pos = vec3(in_pos.x , in_pos.y * world_scale , in_pos.z);

    if (camera_active) {
        gl_Position = proj * view * vec4(pos , 1.0);
    } else {
        gl_Position = vec4(pos , 1.0);
    }

    // if (pos.y < 0.2) {
    //     frag_color = vec3(0.02, 0.04, 0.35); 
    // } else if (pos.y >= 0.2 && pos.y < 0.5) { 
    //     frag_color = vec3(0.05, 0.25, 0.51); 
    // } else if (pos.y >= 0.8) {
    //     frag_color = vec3(0.8, 0.8, 0.8); 
    // } else {
    //     frag_color = vec3(pos.y / 10.0);
    // }

    frag_color = vec3(0.0, 0.8, 7.0);

    frag_texcoord = in_texcoord;
}