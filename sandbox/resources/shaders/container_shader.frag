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

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
    sampler2D diffuse_tex;
    sampler2D specular_tex;
    sampler2D normal_tex;
    sampler2D height_tex;
};

struct DirectionalLight {
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;

    float cutoff;
    float outer_cutoff;

    float constant;
    float linear;
    float quadratic;
};

vec3 CalculateDirLight(DirectionalLight light , vec3 normal , vec3 view_dir);
vec3 CalculatePointLight(PointLight light , vec3 normal , vec3 frag_pos , vec3 view_dir);
vec3 CalculateSpotLight(SpotLight slight , vec3 normal , vec3 frag_pos , vec3 view_dir);

// textures
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform Material material;

#define MAX_POINT_LIGHTS 128

// lights
uniform DirectionalLight dlight;
uniform int point_light_count;
uniform PointLight plights[MAX_POINT_LIGHTS];
// uniform PointLight plight;
uniform SpotLight slight;

void main() {
    vec3 norm = normalize(frag_normal);
    vec3 view_dir = normalize(view_pos - frag_pos);

    vec3 result = vec3(0.0);
    result += CalculateDirLight(dlight , norm , view_dir);

    for (int i = 0; i < point_light_count; i++) 
        result += CalculatePointLight(plights[i] , norm , frag_pos , view_dir);

    // // spotlight
    // result += CalculateSpotLight(slight , norm , frag_pos , view_dir);

    float gamma = 2.2;
    // vec3 color = pow(result , vec3(1.0 / gamma));
    FragColor = vec4(result , 1.0);
}

vec3 CalculateDirLight(DirectionalLight light , vec3 normal , vec3 view_dir) {
    vec3 light_dir = normalize(-light.direction);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float diff = max(dot(normal , light_dir) , 0.0);
    float spec = pow(max(dot(view_dir , reflect_dir) , 0.0) , material.shininess);

    vec3 ambient = light.ambient * texture(tex0 , frag_texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(tex0 , frag_texcoord).rgb;
    vec3 specular = light.specular * spec * texture(tex1 , frag_texcoord).rgb;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light , vec3 normal , vec3 frag_pos , vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float diff = max(dot(normal , light_dir) , 0.0);
    float spec = pow(max(dot(view_dir , reflect_dir) , 0.0) , material.shininess);

    float dist = length(light.position - frag_pos);
    float attentuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.ambient * texture(tex0 , frag_texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(tex0 , frag_texcoord).rgb;
    vec3 specular = light.specular * spec * texture(tex1 , frag_texcoord).rgb;
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight slight , vec3 normal , vec3 frag_pos , vec3 view_dir) {
    // ambient
    vec3 ambient = slight.ambient * texture(tex0 , frag_texcoord).rgb;

    // diffuse
    vec3 light_dir = normalize(slight.position - frag_pos);
    float diff = max(dot(normal , light_dir), 0.0);
    vec3 diffuse = slight.diffuse * diff * texture(tex0 , frag_texcoord).rgb;

    // specular 
    vec3 sreflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir , sreflect_dir) , 0.0) , material.shininess);
    vec3 specular = slight.specular * spec * texture(tex1 , frag_texcoord).rgb;

    // spotlight
    float theta = dot(light_dir, normalize(-slight.direction));
    float epsilon = slight.cutoff - slight.outer_cutoff;
    float intensity = clamp((theta - slight.outer_cutoff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    // attentuation
    float dist = length(slight.position - frag_pos);
    float attentuation = 1.0 / (slight.constant + slight.linear * dist + slight.quadratic * (dist * dist));

    ambient *= attentuation;    
    diffuse *= attentuation;
    specular *= attentuation;

    return vec3(0.0);
}