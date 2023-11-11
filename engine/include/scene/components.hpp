#ifndef YE_ENTITY_COMPONENTS_HPP
#define YE_ENTITY_COMPONENTS_HPP

#include <string>
#include <vector>
#include <limits>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <mono/metadata/object.h>

#include "native_script_entity.hpp"
#include "core/UUID.hpp"
#include "core/RNG.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/model.hpp"
#include "physics/physics_engine.hpp"
#include "scripting/script_engine.hpp"

namespace YE {
    
    static constexpr uint32_t kSizeOfTransformMat = 16;

namespace components {

    struct ID {
        UUID id{ 0 };
        std::string name = "<NAMELESS ENTITY>";

        ID() {}
        ID(const ID& other) 
            : id(other.id) , name(other.name) {}
        ID(const std::string& name) 
            : id(GetNewUUID()) , name(name) {}
    };

    struct Grouping {
        UUID parent{ 0 };
        std::vector<UUID> children;

        Grouping() {}
        Grouping(const Grouping& other) 
            : parent(other.parent) , children(other.children) {}
        Grouping(const UUID& parent)
            : parent(parent) {}
    };

    struct Transform {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        glm::vec3 rotation = glm::vec3(1.0f);

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4& Model();
        glm::quat RotationQuat() const { return glm::quat(rotation); }

        Transform() {}
        Transform(const Transform& other) 
            : position(other.position) , scale(other.scale) ,
            rotation(other.rotation) ,
            model(other.model) {} 
        Transform(const glm::vec3& pos , const glm::vec3& scale ,
                  const glm::vec3& rotation);
    };

    struct Renderable {
        VertexArray* vao = nullptr;
        Material material;
        Shader* shader = nullptr;
        std::string shader_name;

        bool corrupted = false;

        Renderable() {}
        Renderable(const Renderable& other) 
            : vao(other.vao) , shader_name(other.shader_name) , 
            material(other.material) , corrupted(other.corrupted) {}
        Renderable(VertexArray* vao , Material material , const std::string& shader_name)
            : vao(vao) , material(material) , shader_name(shader_name) {}
    };

    struct TexturedRenderable {
        VertexArray* vao = nullptr;
        Shader* shader = nullptr;
        Material material;
        std::string shader_name;
        std::vector<Texture*> textures{};

        bool corrupted = false;

        TexturedRenderable() {}
        TexturedRenderable(const TexturedRenderable& other) 
            : vao(other.vao) , shader_name(other.shader_name) , textures(other.textures) ,
            corrupted(other.corrupted) {}
        TexturedRenderable(VertexArray* vao , Material material , const std::string& shader_name , 
                           const std::vector<Texture*>& textures) 
            : vao(vao) , material(material) , 
            shader_name(shader_name) , textures(textures) {}
    };

    struct CubeMapRenderable {
        // VertexArray* vao = nullptr;
        // Shader* shader = nullptr;
        // std::string shader_name;
        // CubeMap* cube_map = nullptr;
    };

    struct RenderableModel {
        Model* model = nullptr;
        Shader* shader = nullptr;
        Material material;
        std::string model_name = "";
        std::string shader_name = "";

        bool corrupted = false;

        RenderableModel() {}
        RenderableModel(const RenderableModel& other) 
            : model(other.model) , shader_name(other.shader_name) , corrupted(other.corrupted) {}
        RenderableModel(Material material , const std::string& model_name , const std::string& shader_name) 
            : material(material) , model_name(model_name) , shader_name(shader_name) {}
    };

    struct DirectionalLight {
        glm::vec3 direction = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);

        DirectionalLight() {}
        DirectionalLight(const DirectionalLight& other) 
            : direction(other.direction) , ambient(other.ambient) , diffuse(other.diffuse) ,
            specular(other.specular) {}
        DirectionalLight(const glm::vec3& direction , const glm::vec3& ambient , const glm::vec3& diffuse ,
                         const glm::vec3& specular) 
                         : direction(direction) , ambient(ambient) , diffuse(diffuse) , specular(specular) {}
    };

    struct PointLight {
        glm::vec3 position = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        
        float constant_attenuation = 1.0f;
        float linear_attenuation = 0.09f;
        float quadratic_attenuation = 0.032f;

        PointLight() {}
        PointLight(const PointLight& other) 
            : position(other.position) , ambient(other.ambient) , diffuse(other.diffuse) ,
            specular(other.specular) , constant_attenuation(other.constant_attenuation) ,
            linear_attenuation(other.linear_attenuation) , quadratic_attenuation(other.quadratic_attenuation) {}
        PointLight(const glm::vec3& position , const glm::vec3& ambient , const glm::vec3& diffuse ,
                   const glm::vec3& specular , float constant_attenuation , float linear_attenuation ,
                   float quadratic_attenuation) 
                   : position(position) , ambient(ambient) , diffuse(diffuse) , specular(specular) ,
                   constant_attenuation(constant_attenuation) , linear_attenuation(linear_attenuation) ,
                   quadratic_attenuation(quadratic_attenuation) {}
    };

    struct SpotLight {
        glm::vec3 position = glm::vec3(1.0f);
        glm::vec3 direction = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(1.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);

        float constant_attenuation = 1.0f;
        float linear_attenuation = 0.09f;
        float quadratic_attenuation = 0.032f;

        float cutoff = 0.0f;
        float outer_cutoff = 0.0f;

        SpotLight() {}
        SpotLight(const SpotLight& other) 
            : position(other.position) , direction(other.direction) , ambient(other.ambient) ,
            diffuse(other.diffuse) , specular(other.specular) , constant_attenuation(other.constant_attenuation) ,
            linear_attenuation(other.linear_attenuation) , quadratic_attenuation(other.quadratic_attenuation) ,
            cutoff(other.cutoff) , outer_cutoff(other.outer_cutoff) {}
        SpotLight(const glm::vec3& position , const glm::vec3& direction , const glm::vec3& ambient ,
                  const glm::vec3& diffuse , const glm::vec3& specular , float constant_attenuation ,
                  float linear_attenuation , float quadratic_attenuation , float cutoff , float outer_cutoff) 
                  : position(position) , direction(direction) , ambient(ambient) , diffuse(diffuse) ,
                  specular(specular) , constant_attenuation(constant_attenuation) , linear_attenuation(linear_attenuation) ,
                  quadratic_attenuation(quadratic_attenuation) , cutoff(cutoff) , outer_cutoff(outer_cutoff) {}
    };

    /// \note this will override any rotation by the transform component
    ///         as its rotation/position will be decided by the physics engine
    struct PhysicsBody {
        PhysicsBodyType type = PhysicsBodyType::STATIC;
        rp3d::RigidBody* body = nullptr;

        /// \note this is private because eventually there will be physics
        ///     calculations happening when these are called because this should 
        ///     manage its own interpolation (hopefully)
        private:
            rp3d::Transform interpolation_transform;
        
        public:
            PhysicsBody() {}
            PhysicsBody(const PhysicsBody& other) 
                : type(other.type) , body(other.body) , interpolation_transform(other.interpolation_transform) {}
            PhysicsBody(PhysicsBodyType type) 
                : type(type) {}

            void SetInterpolationTransform(const rp3d::Transform& transform) { interpolation_transform = transform; }
            rp3d::Transform& GetInterpolationTransform() { return interpolation_transform; }
    };

    struct BoxCollider {
        rp3d::BoxShape* shape = nullptr;
        rp3d::Collider* collider = nullptr;

        BoxCollider() {}
        BoxCollider(const BoxCollider& other) 
            : shape(other.shape) , collider(other.collider) {}
    };

    struct SphereCollider {
        rp3d::SphereShape* shape = nullptr;
        rp3d::Collider* collider = nullptr;

        float radius = 1.0f;

        SphereCollider() {}
        SphereCollider(const SphereCollider& other) 
            : shape(other.shape) , collider(other.collider) {}
        SphereCollider(float radius)
            : radius(radius) {}
    };

    struct CapsuleCollider {
        rp3d::CapsuleShape* shape = nullptr;
        rp3d::Collider* collider = nullptr;

        float radius = 1.0f;
        float height = 1.0f;

        CapsuleCollider() {}
        CapsuleCollider(const CapsuleCollider& other) 
            : shape(other.shape) , collider(other.collider) {}
        CapsuleCollider(float radius , float height)
            : radius(radius) , height(height) {}
    };

    struct MeshCollider {
        rp3d::PolyhedronMesh* mesh = nullptr;
        rp3d::ConvexMeshShape* shape = nullptr;
        rp3d::Collider* collider = nullptr;

        uint32_t num_faces = 0;
        std::vector<float> vertices{};
        std::vector<uint32_t> indices{};

        MeshCollider() {}
        MeshCollider(const MeshCollider& other) 
            : mesh(other.mesh) , shape(other.shape) , collider(other.collider) {}
        MeshCollider(const std::vector<float>& vertices , const std::vector<uint32_t>& indices , uint32_t num_faces)
            : vertices(vertices) , indices(indices) {}
    };

    struct NativeScript {
        NativeScriptEntity* instance = nullptr;
        NativeScriptEntity*(*bind_script)() = nullptr;
        void(*unbind_script)(NativeScript*) = nullptr;

        bool bound = false;

        NativeScript() {}
        ~NativeScript() {
            if (bound) {
                Unbind();
            }
        }

        template<typename T>
        void Bind() {
            bind_script = []() { return static_cast<NativeScriptEntity*>(new T()); };
            instance = bind_script();

            unbind_script = [](NativeScript* instance) {
                if (instance->instance != nullptr) {
                    ydelete instance->instance;
                    instance->instance = nullptr;
                } else {
                    ENGINE_WARN("Attempted to destroy an unbound native entity script");
                }
            };

            instance->Create();

            bound = true;
        }

        void Update(float dt) {
            if (instance != nullptr) {
                instance->Update(dt);
            } else {
                ENGINE_WARN("Attempted to update an unbound native entity script");
            }
        }

        void Unbind() {
            if (instance != nullptr) {
                instance->Destroy();
                unbind_script(this);
                bind_script = nullptr;
                unbind_script = nullptr;
                bound = false;
            } else {
                ENGINE_WARN("Attempted to destroy an unbound native entity script");
            }
        }
    };
    
    struct Script {
        ScriptObject* object = nullptr;
        MonoObject* instance = nullptr;
        GCHandle handle = nullptr;

        std::vector<ParamHandle> constructor_args{};

        bool bound = false;
        bool active = false;
        std::string class_name;

        Script() {}
        Script(const Script& other) 
            : object(other.object) , instance(other.instance) , handle(other.handle) , 
            constructor_args(other.constructor_args) , bound(other.bound) , active(other.active) {}
        Script(const std::string& class_name , const std::vector<ParamHandle>& constructor_args = {}) 
            : class_name(class_name) , constructor_args(constructor_args) {}

        void Bind(const std::string& class_name) {
            this->class_name = class_name;
            object = ScriptEngine::Instance()->CreateObject(instance , class_name);
            bound = (object != nullptr);
            if (!bound) {
                ENGINE_WARN("Failed to bind entity to script");
                return;
            }
        }

        void Update(float dt) {
            if (bound) {
                ScriptEngine::Instance()->InvokeUpdate(object , instance , handle , dt);
            }
        }

        template<typename T>
        void Set(const std::string& field , T value) {
            if (bound) {
                Field f(&value , sizeof(value));
                ScriptEngine::Instance()->SetObjField(object , instance , handle , field , &f);
            }
        }

        template<>
        void Set<std::string>(const std::string& field , std::string value) {
            if (bound) {
                Field f(static_cast<FieldHandle>(value.data()) , value.size());
                ScriptEngine::Instance()->SetObjField(object , instance , handle , field , &f);
            }
        }

        template<>
        void Set<Entity>(const std::string& field , Entity ent) {
            if (bound) {
                auto& id = ent.GetComponent<ID>();
                Field f(&(id.id) , sizeof(id.id));
                ScriptEngine::Instance()->SetObjField(object , instance , handle , field , &f);
            }
        }

        template<typename T>
        T Get(const std::string& field) {
            if (bound) {
                Field f;
                ScriptEngine::Instance()->GetObjField(object , instance , field , &f);
                return *static_cast<T*>(f.handle);
            }
            return T();
        }

        template<>
        std::string Get<std::string>(const std::string& field) {
            if (bound) {
                Field f;
                ScriptEngine::Instance()->GetObjField(object , instance , field , &f);
                std::string str = ScriptUtils::MonoStringToStr(static_cast<MonoString*>(f.handle));
                return str;
            }
            return std::string();
        }

        template<>
        Entity Get<Entity>(const std::string& field) {
            if (bound) {
                Field f;
                ScriptEngine::Instance()->GetObjField(object , instance , field , &f);
                UUID id = *static_cast<UUID*>(f.handle);

                Entity* ent = ScriptEngine::Instance()->GetSceneContext()->GetEntity(id);
                if (ent == nullptr)
                    ent = ScriptEngine::Instance()->GetSceneContext()->CreateEntity("<blank entity>");

                return *ent;
            }
            return Entity();
        }

        void Unbind() {
            if (bound) {
                ScriptEngine::Instance()->DestroyObject(object , instance , handle);
                bound = false;
            }
        }
    };

}

}

#endif // !YE_ENTITY_COMPONENTS_HPP