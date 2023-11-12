#include "physics/physics_engine.hpp"

#include <memory>

#include "log.hpp"
#include "engine.hpp"
#include "core/resource_handler.hpp"
#include "scene/scene.hpp"
#include "scene/components.hpp"
#include "rendering/renderer.hpp"
#include "rendering/render_commands.hpp"

namespace YE {
        
    PhysicsEngine* PhysicsEngine::singleton = nullptr;
    
    glm::vec3 PhysicsEngine::HexColorToRGB(uint32_t hex) {
        ENTER_FUNCTION_TRACE();

        return glm::vec3(
            (hex >> 16 & 0xFF) / 255.f ,
            (hex >> 8 & 0xFF) / 255.f ,
            (hex & 0xFF) / 255.f
        );
    }

    void PhysicsEngine::SubmitDebugRendering() {
        ENTER_FUNCTION_TRACE();

        YE_CRITICAL_ASSERTION(physics_world != nullptr , "Attempting to submit debug rendering without a physics world");

        rp3d::DebugRenderer& debug_renderer = physics_world->getDebugRenderer();
        uint32_t num_lines = debug_renderer.getNbLines();
        uint32_t num_triangles = debug_renderer.getNbTriangles();

        if (num_lines == 0 || num_triangles == 0)
            return;

        rp3d::DebugRenderer::DebugLine* lines = const_cast<rp3d::DebugRenderer::DebugLine*>(
            debug_renderer.getLinesArray()
        );
        rp3d::DebugRenderer::DebugTriangle* triangles = const_cast<rp3d::DebugRenderer::DebugTriangle*>(
            debug_renderer.getTrianglesArray()
        );

        std::vector<float> triangle_vertices;
        for (uint32_t i = 0; i < num_triangles; ++i) {
            triangle_vertices.push_back(triangles[0].point1.x);
            triangle_vertices.push_back(triangles[0].point1.y);
            triangle_vertices.push_back(triangles[0].point1.z);

            glm::vec3 color = HexColorToRGB(triangles[0].color1);
            triangle_vertices.push_back(color.r);
            triangle_vertices.push_back(color.g);
            triangle_vertices.push_back(color.b);

            triangle_vertices.push_back(triangles[0].point2.x);
            triangle_vertices.push_back(triangles[0].point2.y);
            triangle_vertices.push_back(triangles[0].point2.z);

            glm::vec3 color2 = HexColorToRGB(triangles[0].color2);
            triangle_vertices.push_back(color2.r);
            triangle_vertices.push_back(color2.g);
            triangle_vertices.push_back(color2.b);

            triangle_vertices.push_back(triangles[0].point3.x);
            triangle_vertices.push_back(triangles[0].point3.y);
            triangle_vertices.push_back(triangles[0].point3.z);

            glm::vec3 color3 = HexColorToRGB(triangles[0].color3);
            triangle_vertices.push_back(color3.r);
            triangle_vertices.push_back(color3.g);
            triangle_vertices.push_back(color3.b);
        }

        std::vector<float> line_vertices;
        for (uint32_t i = 0; i < num_lines; ++i) {
            line_vertices.push_back(lines[i].point1.x);
            line_vertices.push_back(lines[i].point1.y);
            line_vertices.push_back(lines[i].point1.z);
            line_vertices.push_back((lines[i].color1 >> 16 & 0xFF) / 255);
            line_vertices.push_back((lines[i].color1 >> 8 & 0xFF) / 255);
            line_vertices.push_back((lines[i].color1 & 0xFF) / 255);

            line_vertices.push_back(lines[i].point2.x);
            line_vertices.push_back(lines[i].point2.y);
            line_vertices.push_back(lines[i].point2.z);
            line_vertices.push_back((lines[i].color2 >> 16 & 0xFF) / 255);
            line_vertices.push_back((lines[i].color2 >> 8) & 0xFF / 255);
            line_vertices.push_back((lines[i].color2 & 0xFF) / 255);
        }

        // temporary until better rendering system is in place
        // ydelete debug_triangle_renderable.vao;
        // debug_triangle_renderable.vao = ynew VertexArray(triangle_vertices , {} , std::vector<uint32_t> {
        //     3 , 3
        // });

        // ydelete debug_line_renderable.vao;
        // debug_line_renderable.vao = ynew VertexArray(line_vertices , {} , std::vector<uint32_t> {
        //     3 , 3
        // });

        // std::unique_ptr<RenderCommand> debug_triangles = std::make_unique<DrawVao>(
        //     debug_triangle_vao ,
        //     ResourceHandler::Instance()->GetCoreShader("default") ,
        //     glm::mat4(1.f) , DrawMode::TRIANGLES
        // );
        // std::unique_ptr<RenderCommand> debug_lines = std::make_unique<DrawVao>(
        //     debug_line_vao , 
        //     ResourceHandler::Instance()->GetCoreShader("default") ,
        //     glm::mat4(1.f) , DrawMode::LINES
        // );

        // Renderer::Instance()->SubmitDebugRenderCmnd(debug_triangles);
        // Renderer::Instance()->SubmitDebugRenderCmnd(debug_lines);
        // end temporary

        EXIT_FUNCTION_TRACE();
    }


    PhysicsEngine* PhysicsEngine::Instance() {
        if (singleton == nullptr)
            singleton = ynew PhysicsEngine;
        return singleton;
    }

    void PhysicsEngine::ToggleDebugRendering() {
        ENTER_FUNCTION_TRACE();

        // debug_rendering = !debug_rendering;
        // if (physics_world != nullptr)
        //     physics_world->setIsDebugRenderingEnabled(debug_rendering);

        // if (debug_rendering) {
        // } else {
        // }
    }

    void PhysicsEngine::SetSceneContext(Scene* scene) {
        ENTER_FUNCTION_TRACE();

        current_context = scene;

        if (physics_world != nullptr)
            physics_common.destroyPhysicsWorld(physics_world);

        rp3d::PhysicsWorld::WorldSettings settings;
        settings.worldName = scene->SceneName();
        settings.gravity = reactphysics3d::Vector3(0.f, -9.81f, 0.f);
        settings.persistentContactDistanceThreshold = 0.3f;
        settings.defaultFrictionCoefficient = 0.3f;
        settings.defaultBounciness = 0.3f;
        settings.restitutionVelocityThreshold = 0.5f;
        settings.isSleepingEnabled = false;
        settings.defaultVelocitySolverNbIterations = 32;
        settings.defaultPositionSolverNbIterations = 32;
        settings.defaultTimeBeforeSleep = true;
        settings.defaultSleepLinearVelocity = 0.02f;
        settings.defaultSleepAngularVelocity = 0.0523599f;
        settings.cosAngleSimilarContactManifold = 0.95f;

        physics_world = physics_common.createPhysicsWorld(settings);

        alpha = 0.f;

        physics_world->setIsDebugRenderingEnabled(debug_rendering);

        rp3d::DebugRenderer& debug_renderer = physics_world->getDebugRenderer();
        debug_renderer.setContactNormalLength(1.5f);
        debug_renderer.setContactPointSphereRadius(1.f);
        debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB , true);
        debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE , true);
        debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT , true);
        debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL , true);
        debug_renderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB , true);

        EXIT_FUNCTION_TRACE();
    }

    rp3d::RigidBody* PhysicsEngine::CreateRigidBody(components::Transform& transform) {
        ENTER_FUNCTION_TRACE();

        rp3d::Vector3 position = rp3d::Vector3(transform.position.x , transform.position.y , transform.position.z);
        
        glm::quat rotation(transform.rotation);
        rp3d::Quaternion orientation = rp3d::Quaternion(rotation.x , rotation.y , rotation.z , rotation.w);

        rp3d::Transform physics_transform(position , orientation);

        EXIT_FUNCTION_TRACE();
        return physics_world->createRigidBody(physics_transform);
    }

    rp3d::BoxShape* PhysicsEngine::CreateBoxShape(components::Transform& transform) {
        ENTER_FUNCTION_TRACE();

        rp3d::Vector3 half_extents = rp3d::Vector3((transform.scale.x + 0.1f) / 2.f , (transform.scale.y + 0.1f) / 2.f , (transform.scale.z + 0.1f) / 2.f);
        
        EXIT_FUNCTION_TRACE();
        return physics_common.createBoxShape(half_extents);
    }

    rp3d::SphereShape* PhysicsEngine::CreateSphereShape(float radius) {
        ENTER_FUNCTION_TRACE();
        return physics_common.createSphereShape(radius);
    }

    rp3d::CapsuleShape* PhysicsEngine::CreateCapsuleShape(float radius , float height) {
        ENTER_FUNCTION_TRACE();
        return physics_common.createCapsuleShape(radius , height);
    }

    rp3d::PolyhedronMesh* PhysicsEngine::CreatePolygonMesh(
        const std::vector<float>& vertices , const std::vector<uint32_t>& indices , uint32_t num_faces
    ) {
        ENTER_FUNCTION_TRACE();
        rp3d::PolygonVertexArray::PolygonFace* faces = ynew rp3d::PolygonVertexArray::PolygonFace[num_faces];
        rp3d::PolygonVertexArray::PolygonFace* face = faces;

        for (uint32_t i = 0; i < indices.size(); i += 3) {
            face->indexBase = i;
            face->nbVertices = 3;
            ++face;
        }

/*
uint32 nbVertices, const void* verticesStart, uint32 verticesStride ,const void* indexesStart, uint32 indexesStride, uint32 nbFaces, PolygonFace* facesStart,
VertexDataType vertexDataType, IndexDataType indexDataType
*/
        rp3d::PolygonVertexArray* pva = ynew rp3d::PolygonVertexArray(
            vertices.size() / 3 , vertices.data() , sizeof(float) * 3 , indices.data() , sizeof(uint32_t) , num_faces , faces ,
            rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE , rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        ); 

        EXIT_FUNCTION_TRACE();
        return physics_common.createPolyhedronMesh(pva);
    }

    rp3d::ConvexMeshShape* PhysicsEngine::CreateConvexMeshShape(rp3d::PolyhedronMesh* mesh) {
        ENTER_FUNCTION_TRACE();
        return physics_common.createConvexMeshShape(mesh);
    }

    void PhysicsEngine::StepPhysics() {
        if (current_context == nullptr) {
            return;
        }
        
        float time_step = Engine::Instance()->TargetTimeStep();
        accumulator += time_step;

        while (accumulator >= time_step) {
            physics_world->update(time_step);
            accumulator -= time_step;
        }

        alpha = accumulator / time_step;

        // if (debug_rendering)
            // SubmitDebugRendering();
    }

    void PhysicsEngine::DestroyRigidBody(rp3d::RigidBody* body) {
        ENTER_FUNCTION_TRACE();
        physics_world->destroyRigidBody(body);
    }
    
    void PhysicsEngine::DestroyBoxShape(rp3d::BoxShape* shape) {
        ENTER_FUNCTION_TRACE();
        physics_common.destroyBoxShape(shape);
    }

    void PhysicsEngine::DestroySphereShape(rp3d::SphereShape* shape) {
        ENTER_FUNCTION_TRACE();
        physics_common.destroySphereShape(shape);
    }

    void PhysicsEngine::DestroyCapsuleShape(rp3d::CapsuleShape* shape) {
        ENTER_FUNCTION_TRACE();
        physics_common.destroyCapsuleShape(shape);
    }
    
    void PhysicsEngine::DestroyPolygonMesh(rp3d::PolyhedronMesh* shape) {
        ENTER_FUNCTION_TRACE();
        physics_common.destroyPolyhedronMesh(shape);
    }
    
    void PhysicsEngine::DestroyConvexMeshShape(rp3d::ConvexMeshShape* shape) {
        ENTER_FUNCTION_TRACE();
        physics_common.destroyConvexMeshShape(shape);
    }

    void PhysicsEngine::Cleanup() {
        ENTER_FUNCTION_TRACE();

        if (physics_world != nullptr)
            physics_common.destroyPhysicsWorld(physics_world);
        
        if (singleton != nullptr)
            ydelete singleton;

        EXIT_FUNCTION_TRACE();
    }
    
}