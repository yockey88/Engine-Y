#include "rendering/model.hpp"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "core/defines.hpp"
#include "core/log.hpp"

namespace EngineY {
    
    void Model::ProcessNode(aiNode* node , const aiScene* scene) {
        ENGINE_ASSERT(node != nullptr , "Error: Invalid mesh");
        ENGINE_ASSERT(scene != nullptr , "Error: Invalid scene");

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh , scene);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i] , scene);

    }

    void Model::ProcessMesh(aiMesh* mesh , const aiScene* scene) {
        std::vector<Vertex> yvertices{};
        std::vector<uint32_t> indices{};

        num_vertices += mesh->mNumVertices;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex{};
            vertex.position = glm::vec3(mesh->mVertices[i].x , mesh->mVertices[i].y , mesh->mVertices[i].z);

            if (mesh->HasNormals()) {
                vertex.normal = glm::vec3(mesh->mNormals[i].x , mesh->mNormals[i].y , mesh->mNormals[i].z);
            } else {
                vertex.normal = glm::vec3(1.0f , 1.0f , 1.0f);
            }

            if (mesh->HasVertexColors(i)) {
                vertex.color = glm::vec3(mesh->mColors[0][i].r , mesh->mColors[0][i].g , mesh->mColors[0][i].b);
            }

            if (mesh->mTextureCoords[0]) {
                vertex.texcoord = glm::vec2(mesh->mTextureCoords[0][i].x , mesh->mTextureCoords[0][i].y);
            } else {
                vertex.texcoord = glm::vec2(0.0f , 0.0f);
            }

            if (mesh->HasTangentsAndBitangents()) {
                vertex.tangent = glm::vec3(mesh->mTangents[i].x , mesh->mTangents[i].y , mesh->mTangents[i].z);
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x , mesh->mBitangents[i].y , mesh->mBitangents[i].z);
            } else {
                vertex.tangent = glm::vec3(1.0f , 1.0f , 1.0f);
                vertex.bitangent = glm::vec3(1.0f , 1.0f , 1.0f);
            }
            yvertices.push_back(vertex);
        }

        for (auto& v : yvertices) {
            vertices.push_back(v.position.x);
            vertices.push_back(v.position.y);
            vertices.push_back(v.position.z);
        }

        num_faces += mesh->mNumFaces;

        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }


        VertexArray* vertex_array = ynew(VertexArray ,yvertices , indices);
        vaos.push_back(vertex_array);

        this->yvertices.insert(this->yvertices.end() , yvertices.begin() , yvertices.end()); // = yvertices;
        this->indices.insert(this->indices.end() , indices.begin() , indices.end()); // = indices;

        if (!scene->HasMaterials())
            return;

        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

        {
            aiColor3D ambient , color , shininess; // emission , roughness , metallic ,normal
            if (mat->Get(AI_MATKEY_COLOR_DIFFUSE , color) == AI_SUCCESS) 
                diffuse = glm::vec3(color.r , color.g , color.b); 
            
            if (mat->Get(AI_MATKEY_COLOR_AMBIENT , ambient) == AI_SUCCESS)
                this->ambient = glm::vec3(ambient.r , ambient.g , ambient.b);

            if (mat->Get(AI_MATKEY_SHININESS , shininess) == AI_SUCCESS)
                this->shininess = shininess.r;
        } 

        for (uint32_t i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++) {
            aiString str;
            mat->GetTexture(aiTextureType_DIFFUSE , i , &str);

            if (std::find(texture_paths.begin() , texture_paths.end() , directory + "/" + str.C_Str()) == texture_paths.end()) {
                texture_paths.push_back(directory + "/" + str.C_Str());
                Texture* texture = ynew(Texture , directory + "/" + str.C_Str());
                texture->SetTextureType(TextureType::diffuse);
                textures.push_back(texture);
            } else {
                continue;
            }
        }

        for (uint32_t i = 0; i < mat->GetTextureCount(aiTextureType_SPECULAR); i++) {
            aiString str;
            mat->GetTexture(aiTextureType_SPECULAR , i , &str);

            if (std::find(texture_paths.begin() , texture_paths.end() , directory + "/" + str.C_Str()) == texture_paths.end()) {
                texture_paths.push_back(directory + "/" + str.C_Str());
                Texture* texture = ynew(Texture , directory + "/" + str.C_Str());
                texture->SetTextureType(TextureType::specular);
                textures.push_back(texture);
            } else {
                continue;
            }
        }

        for (uint32_t i = 0; i < mat->GetTextureCount(aiTextureType_HEIGHT); i++) {
            aiString str;
            mat->GetTexture(aiTextureType_HEIGHT , i , &str);

            if (std::find(texture_paths.begin() , texture_paths.end() , directory + "/" + str.C_Str()) == texture_paths.end()) {
                texture_paths.push_back(directory + "/" + str.C_Str());
                Texture* texture = ynew(Texture , directory + "/" + str.C_Str());
                texture->SetTextureType(TextureType::height);
                textures.push_back(texture);
            } else {
                continue;
            }
        }

        for (uint32_t i = 0; i < mat->GetTextureCount(aiTextureType_NORMALS); i++) {
            aiString str;
            mat->GetTexture(aiTextureType_NORMALS , i , &str);

            if (std::find(texture_paths.begin() , texture_paths.end() , directory + "/" + str.C_Str()) == texture_paths.end()) {
                texture_paths.push_back(directory + "/" + str.C_Str());
                Texture* texture = ynew(Texture , directory + "/" + str.C_Str());
                texture->SetTextureType(TextureType::normal);
                textures.push_back(texture);
            } else {
                continue;
            }
        }

        if (textures.size() > 0) 
            textured = true;
    }
    
    void Model::Load(){

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path , aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                                                        aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);

        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            ENGINE_ERROR("Unable to Load File -> {} | Assimp Error -> {}" , path , importer.GetErrorString());
            return;
        }

        directory = path.substr(0 , path.find_last_of('/'));
        ProcessNode(scene->mRootNode , scene);

        for (auto& texture : textures)
            texture->Load();
        
        valid = true;
    }

    void Model::Draw(Shader* shader , DrawMode draw_mode) {
        if (textured) {
            for (uint32_t i = 0; i < textures.size(); i++) {
                std::string name = textures[i]->GetTypeName();
                shader->SetUniformInt("material." + name + "_tex" , i);
                textures[i]->Bind(i);
            }
        }

        shader->SetUniformVec3("material.ambient" , ambient);
        shader->SetUniformFloat("material.shininess" , shininess);

        for (auto& vao : vaos)
            vao->Draw(draw_mode);

        for (uint32_t i = 0; i < textures.size(); i++)
            textures[i]->Unbind(i);
    }

    void Model::Cleanup() {
        for (auto& vao : vaos) {
            ydelete(vao);
        }
        
        for (auto& texture : textures) {
            ydelete(texture);
        }
    }
    
}
