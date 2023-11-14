#ifndef YE_PHYSICS_ENGINE_HPP
#define YE_PHYSICS_ENGINE_HPP

#include <reactphysics3d/reactphysics3d.h>

#include "rendering/vertex_array.hpp"

namespace YE {

    using ContactListener = rp3d::EventListener;

namespace components {

    struct Transform;
    struct Renderable;

}

    class Scene;

    enum class PhysicsBodyType {
        STATIC = 0 ,
        KINEMATIC ,
        DYNAMIC 
    };

    class PhysicsEngine {
        static PhysicsEngine* singleton;

        rp3d::PhysicsCommon physics_common;
        rp3d::PhysicsWorld* physics_world = nullptr;
        
        rp3d::DebugRenderer::DebugLine* debug_lines = nullptr;
        rp3d::DebugRenderer::DebugTriangle* debug_triangles = nullptr;

        // components::Renderable debug_line_renderable;
        // components::Renderable debug_triangle_renderable;

        uint32_t debug_line_count = 0;
        uint32_t debug_triangle_count = 0;

        rp3d::decimal accumulator = 0.f;
        rp3d::decimal alpha = 0.f;

        Scene* current_context = nullptr;

        bool debug_rendering = true;

        glm::vec3 HexColorToRGB(uint32_t hex);
        void SubmitDebugRendering();

        PhysicsEngine() {}
        ~PhysicsEngine() {}

        PhysicsEngine(PhysicsEngine&&) = delete;
        PhysicsEngine(const PhysicsEngine&) = delete;
        PhysicsEngine& operator=(PhysicsEngine&&) = delete;
        PhysicsEngine& operator=(const PhysicsEngine&) = delete;

        public:

            static PhysicsEngine* Instance(); 

            void ToggleDebugRendering();
            void SetSceneContext(Scene* scene);
            void UnsetSceneContext();

            rp3d::RigidBody* CreateRigidBody(components::Transform& transform);
            rp3d::BoxShape* CreateBoxShape(components::Transform& transform);
            rp3d::SphereShape* CreateSphereShape(float radius);
            rp3d::CapsuleShape* CreateCapsuleShape(float radius , float height);
            rp3d::PolyhedronMesh* CreatePolygonMesh(const std::vector<float>& vertices , const std::vector<uint32_t>& indices , uint32_t num_faces);
            rp3d::ConvexMeshShape* CreateConvexMeshShape(rp3d::PolyhedronMesh* mesh);

            void StepPhysics();

            void DestroyRigidBody(rp3d::RigidBody* body);
            void DestroyBoxShape(rp3d::BoxShape* shape);
            void DestroySphereShape(rp3d::SphereShape* shape);
            void DestroyCapsuleShape(rp3d::CapsuleShape* shape);
            void DestroyPolygonMesh(rp3d::PolyhedronMesh* shape);
            void DestroyConvexMeshShape(rp3d::ConvexMeshShape* shape);
            void Cleanup();

            inline rp3d::PhysicsWorld* World() { return physics_world; }
            inline rp3d::decimal Alpha() const { return alpha; }
    };

}

#endif // !YE_PHYSICS_ENGINE_HPP