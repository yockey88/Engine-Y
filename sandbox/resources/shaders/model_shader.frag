#version 460 core

in vec3 frag_pos;
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;
in float frag_opacity;

out vec4 FragColor;

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

uniform vec3 view_pos;
uniform Material material;

// lights
uniform DirectionalLight dlight;
uniform PointLight plight;
uniform SpotLight slight;

vec3 CalculateDirLight(DirectionalLight light , vec3 normal , vec3 view_dir);
vec3 CalculatePointLight(PointLight light , vec3 normal , vec3 frag_pos , vec3 view_dir);
vec3 CalculateSpotLight(SpotLight light , vec3 normal , vec3 frag_pos , vec3 view_dir);

void main() {
    vec3 normal = normalize(frag_normal);
    vec3 view_dir = normalize(view_pos - frag_pos);

    vec3 result = vec3(0.0);
    result += CalculateDirLight(dlight , normal , view_dir);

    result += CalculatePointLight(plight , normal , frag_pos , view_dir);

    // result += CalculateSpotLight(slight , normal , frag_pos , view_dir);

    float gamma = 2.2;
    // vec3 color = pow(result , vec3(1.0 / gamma));
    FragColor = vec4(result , 1.0);
}

vec3 CalculateDirLight(DirectionalLight light , vec3 normal , vec3 view_dir) {
    vec3 light_dir = normalize(-light.direction);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float diff = max(dot(normal , light_dir) , 0.0);
    float spec = pow(max(dot(view_dir , reflect_dir) , 0.0) , material.shininess);

    vec3 ambient = light.ambient * texture(material.diffuse_tex , frag_texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse_tex , frag_texcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular_tex , frag_texcoord).rgb;

    return ambient + diffuse + specular;
}

vec3 CalculatePointLight(PointLight light , vec3 normal , vec3 frag_pos , vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float diff = max(dot(normal , light_dir) , 0.0);
    float spec = pow(max(dot(view_dir , reflect_dir) , 0.0) , material.shininess);

    float dist = length(light.position - frag_pos);
    float attentuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.ambient * texture(material.diffuse_tex , frag_texcoord).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse_tex , frag_texcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular_tex , frag_texcoord).rgb;
    ambient *= attentuation;
    diffuse *= attentuation;
    specular *= attentuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light , vec3 normal , vec3 frag_pos , vec3 view_dir) {
    // ambient
    vec3 ambient = slight.ambient * texture(material.diffuse_tex , frag_texcoord).rgb;

    // diffuse
    vec3 light_dir = normalize(slight.position - frag_pos);
    float diff = max(dot(normal , light_dir), 0.0);
    vec3 diffuse = slight.diffuse * diff * texture(material.diffuse_tex , frag_texcoord).rgb;

    // specular 
    vec3 sreflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir , sreflect_dir) , 0.0) , material.shininess);
    vec3 specular = slight.specular * spec * texture(material.specular_tex , frag_texcoord).rgb;

    // spotlight
    float theta = dot(light_dir, normalize(-slight.direction));
    float epsilon = slight.cutoff - slight.outer_cutoff;
    float intensity = clamp((theta - slight.outer_cutoff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    // attentuation
    float dist = length(slight.position - frag_pos);
    float attentuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    ambient *= attentuation;    
    diffuse *= attentuation;
    specular *= attentuation;

    return vec3(0.0);
}