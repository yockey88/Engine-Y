static constexpr std::string_view ystd_vert_layout = 
R"(
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
)";

static constexpr std::string_view ystd_camera_uniforms =
R"(
    uniform mat4 view;
    uniform mat4 projection;
    uniform mat4 model;
    uniform vec3 view_pos;
)";