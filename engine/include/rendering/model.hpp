#ifndef ENGINEY_MODEL_HPP
#define ENGINEY_MODEL_HPP

#include <string>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"

namespace EngineY {

    class Model;

    struct ModelResource {
        std::string name;
        std::string filename;
        std::string path;
        Model* model = nullptr;

        ModelResource() {}
    };

    class Model {        
        std::string name;
        std::string path;
        std::string directory;
        std::vector<std::string> texture_paths{};
        
        std::vector<float> vertices{};
        std::vector<uint32_t> indices{};
        std::vector<Vertex> yvertices{};
        std::vector<VertexArray*> vaos;
        std::vector<Texture*> textures;

        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 emission = glm::vec3(1.0f);
        float roughness = 0.0f;
        float shininess = 0.0f;
        float metallic = 0.0f;

        bool valid = false;
        bool textured = false;
        
        uint32_t num_vertices = 0;
        uint32_t num_faces = 0;
        uint32_t num_meshes = 0;

        void ProcessNode(aiNode* mesh , const aiScene* scene);
        void ProcessMesh(aiMesh* mesh , const aiScene* scene); 

        public:
            Model(const std::string& name , const std::string& path) 
                : name(name) , path(path) {}
            ~Model() {}

            void Load();

            void Draw(Shader* shader , DrawMode mode = DrawMode::TRIANGLES);

            void Cleanup();

            inline glm::vec3 Diffuse() const { return diffuse; }
            inline glm::vec3 Specular() const { return specular; }
            inline glm::vec3 Ambient() const { return ambient; }
            inline glm::vec3 Emission() const { return emission; }
            inline float Roughness() const { return roughness; }
            inline float Metallic() const { return metallic; }

            inline void SetDiffuse(const glm::vec3& diffuse) { this->diffuse = diffuse; }
            inline void SetSpecular(const glm::vec3& specular) { this->specular = specular; }
            inline void SetAmbient(const glm::vec3& ambient) { this->ambient = ambient; }
            inline void SetEmission(const glm::vec3& emission) { this->emission = emission; }
            inline void SetRoughness(float roughness) { this->roughness = roughness; }
            inline void SetMetallic(float metallic) { this->metallic = metallic; }

            inline bool Valid() const { return valid; }
            
            inline const std::string& Name() const { return name; }
            inline const std::string& Path() const { return path; }

            inline const uint32_t NumVertices() const { return num_vertices; }
            inline const uint32_t NumFaces() const { return num_faces; }
            inline const std::vector<Vertex>& YVertices() const { return yvertices; }
            inline const std::vector<float>& Vertices() const { return vertices; }
            inline const std::vector<uint32_t>& Indices() const { return indices; }
            inline const std::vector<VertexArray*>& VertexArrays() const { return vaos; }
            inline const std::vector<Texture*> Textures() const { return textures; }
    };
    
}

#endif // !ENGINEY_MODEL_HPP