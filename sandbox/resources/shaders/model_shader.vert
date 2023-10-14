#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;
layout (location = 5) in vec2 in_texcoord;
layout (location = 6) in float in_opacity;

out vec3 frag_pos;
out vec3 frag_color;
out vec3 frag_normal;
out vec3 frag_tangent;
out vec3 frag_bitangent;
out vec2 frag_texcoord;
out float frag_opacity;

uniform bool camera_active = false;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

void main() {
    frag_pos = vec3(model * vec4(in_pos, 1.0));
    frag_normal = mat3(transpose(inverse(model))) * in_normal;

    if (camera_active) {
        gl_Position = proj * view * vec4(frag_pos , 1.0);
    } else {
        gl_Position = model * vec4(in_pos , 1.0);
    }

    frag_color = in_color;
    frag_tangent = in_tangent;
    frag_bitangent = in_bitangent;
    frag_texcoord = in_texcoord;
    frag_opacity = in_opacity;
}