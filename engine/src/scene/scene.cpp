#include "scene/scene.hpp"

#include <iostream>

#include "core/task_manager.hpp"
#include "core/resource_handler.hpp"
#include "scene/entity.hpp"
#include "scene/components.hpp"
#include "scene/systems.hpp"
#include "rendering/render_commands.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/camera.hpp"

namespace YE {

    void UpdateTest(entt::registry& registry , float dt) {
        std::cout << "UpdateTest: " << dt << std::endl;
    }

    Entity* Scene::CreateEntity(const std::string& name) {
        Systems::entity_created_signal.publish(this , std::ref(name));
        return entities[Hash::FNV(name)];
    }

    Entity* Scene::GetEntity(UUID id) {
        if (entities.find(id.uuid) != entities.end()) {
            return entities[id.uuid];
        }
        return nullptr;
    }
    
    Entity* Scene::GetEntity(const std::string& name) {
        UUID id = Hash::FNV(name);
        return GetEntity(id);
    }

    void Scene::DestroyEntity(Entity* entity) {
        auto& id = entity->GetComponent<components::ID>();
        if (id.id == 0) {
            YE_WARN("Failed to destroy entity | Entity has no ID or does not exist");
            return;
        }

        Systems::entity_destroyed_signal.publish(this , entity);

        entt::entity entt = entity->GetEntity();

        registry.destroy(entt);
        entities.erase(id.id);
    }

    // void Scene::DestroyEntity(UUID id) {

    // }

    // void Scene::DestroyEntity(const std::string& name) {

    // }

    std::unordered_map<UUID , Entity*>* Scene::Entities() {
        return &entities;
    }

    bool Scene::IsEntityValid(UUID id) {
        return (std::find(living_entities.begin() , living_entities.end() , id.uuid) != living_entities.end());
    }

    void Scene::InitializeScene() {
        Systems::SetSceneContext(this);
        ScriptEngine::Instance()->SetSceneContext(this);
        PhysicsEngine::Instance()->SetSceneContext(this);
        Renderer::Instance()->RegisterSceneContext(this);
    }

    void Scene::LoadScene(/* not sure what would be passed here */) {
        Systems::scene_load_signal.publish(this);
    }

    void Scene::UnloadScene(/* not sure what would be passed here */) {
        Systems::scene_unload_signal.publish(this);
    }

    Camera* Scene::AttachCamera(const std::string& name , CameraType type) {
        UUID32 id = Hash::FNV32(name);
        Camera* camera = ynew Camera(type);
        
        cameras[id] = camera;

        if (active_camera == nullptr) {
            active_camera = camera;
            active_camera_id = id;
        }

        return camera;
    }
    
    Camera* Scene::GetCamera(const std::string& name) {
        return GetCamera(Hash::FNV32(name));
    }

    Camera* Scene::GetCamera(UUID32 id) {
        if (cameras.find(id.uuid) != cameras.end())
            return cameras[id];
        return nullptr;
    }

    void Scene::SetActiveCamera(UUID32 id) {
        if (cameras.find(id.uuid) != cameras.end()) {
            active_camera = cameras[id];
        } else {
            active_camera = nullptr;
        }
    }

    void Scene::SetActiveCamera(const std::string& name) {
        SetActiveCamera(Hash::FNV32(name));
    }

    void Scene::SetRenderMode(RenderMode mode) {
        current_render_mode = mode;
        Renderer::Instance()->SetSceneRenderMode(mode);
    }

    void Scene::Start() {
        Systems::scene_start_signal.publish(this);
    }

    void Scene::Update(float dt) {
        TaskManager* task_manager = TaskManager::Instance();

        Systems::update_signal.publish(this , std::ref(dt));

        if (active_camera != nullptr)
            active_camera->Update(dt);

        // must stay in main thread
        registry.view<components::Script>().each([dt](auto& script) {
            script.Update(dt);
        });

        task_manager->DispatchTask([reg = &registry , dt]() {
            reg->view<components::Transform>().each([](auto& transform) {
                Systems::entity_update_transform_signal.publish(transform);
            });

            reg->view<components::PhysicsBody , components::Transform>().each([](auto& body , auto& transform) {
                Systems::physics_body_update_signal.publish(body , transform);
            });
        });
        
        task_manager->DispatchTask([reg = &registry , dt]() {
            reg->view<components::NativeScript>().each([dt](auto& script) {
                script.Update(dt);
            });
        });

        registry.view<components::Transform , components::PointLight>().each([](auto& transform , auto& light) {
            light.position = transform.position;
        });
        
        auto plights = registry.view<components::PointLight>();
        const uint32_t num_lights = plights.size();
        std::vector<components::PointLight> plights_vec;

        for (auto itr = plights.begin(); itr != plights.end(); itr++) {
            const components::PointLight& light = registry.get<components::PointLight>(*itr);
            plights_vec.push_back(light);
        }

        registry.view<components::Renderable>().each([plights_vec](auto& renderable) {
            Systems::renderable_update_signal.publish(renderable , plights_vec);
        });

        registry.view<components::TexturedRenderable>().each([plights_vec](auto& renderable) {
            Systems::textured_renderable_update_signal.publish(renderable , plights_vec);
        });

        registry.view<components::RenderableModel>().each([plights_vec](auto& renderable) {
            Systems::renderable_model_update_signal.publish(renderable , plights_vec);
        });
        
        // task_manager->DispatchTask([reg = &registry]() {
        //     auto plights = reg->view<components::PointLight>();
        //     const uint32_t num_lights = plights.size();
        //     std::vector<components::PointLight> plights_vec;

        //     for (auto itr = plights.begin(); itr != plights.end(); itr++) {
        //         const components::PointLight& light = reg->get<components::PointLight>(*itr);
        //         plights_vec.push_back(light);
        //     }

        //     reg->view<components::Renderable>().each([plights_vec](auto& renderable) {
        //         Systems::renderable_update_signal.publish(renderable , plights_vec);
        //     });
        // }); 

        // task_manager->DispatchTask([reg = &registry]() {
        //     auto plights = reg->view<components::PointLight>();
        //     const uint32_t num_lights = plights.size();
        //     std::vector<components::PointLight> plights_vec;

        //     for (auto itr = plights.begin(); itr != plights.end(); itr++) {
        //         const components::PointLight& light = reg->get<components::PointLight>(*itr);
        //         plights_vec.push_back(light);
        //     }
            
        //     reg->view<components::TexturedRenderable>().each([plights_vec](auto& renderable) {
        //         Systems::textured_renderable_update_signal.publish(renderable , plights_vec);
        //     });
        // }); 

        // task_manager->DispatchTask([reg = &registry]() {
        //     auto plights = reg->view<components::PointLight>();
        //     const uint32_t num_lights = plights.size();
        //     std::vector<components::PointLight> plights_vec;

        //     for (auto itr = plights.begin(); itr != plights.end(); itr++) {
        //         const components::PointLight& light = reg->get<components::PointLight>(*itr);
        //         plights_vec.push_back(light);
        //     }
            
        //     reg->view<components::RenderableModel>().each([plights_vec](auto& renderable) {
        //         Systems::renderable_model_update_signal.publish(renderable , plights_vec);
        //     });
        // }); 
    }

    void Scene::Draw() {
        Renderer* renderer = Renderer::Instance();
        if (active_camera != nullptr)
            renderer->PushCamera(active_camera);

        registry.view<components::Transform , components::Renderable , components::PointLight>().each(
            [reg = &registry , renderer](auto& transform , auto& renderable , auto& light) {
                if (renderable.corrupted) return;

                transform.position = light.position;
                std::unique_ptr<RenderCommand> cmnd = std::make_unique<DrawPointLight>(
                    renderable , &light , transform.Model()
                );
                renderer->SubmitRenderCmnd(cmnd);
            }
        );

        registry.view<components::Transform , components::Renderable>(
            entt::exclude<components::PointLight>
        ).each([reg = &registry , renderer](auto& transform , auto& renderable) {
            if (renderable.corrupted) return;

            std::unique_ptr<RenderCommand> cmnd = std::make_unique<DrawRenderable>(
                renderable , transform.model
            );  
            renderer->SubmitRenderCmnd(cmnd);
        });

        registry.view<components::Transform , components::TexturedRenderable>().each(
            [reg = &registry , renderer](auto& transform , auto& renderable) {
                if (renderable.corrupted) return;

                std::unique_ptr<RenderCommand> cmnd = std::make_unique<DrawTexturedRenderable>(
                    renderable , transform.model
                );
                renderer->SubmitRenderCmnd(cmnd);
            }
        );

        registry.view<components::Transform , components::RenderableModel>().each(
            [reg = &registry , renderer](auto& transform , auto& renderable) {
                if (renderable.corrupted) return;

                std::unique_ptr<RenderCommand> cmnd = std::make_unique<DrawRenderableModel>(
                    renderable , transform.model
                );
                renderer->SubmitRenderCmnd(cmnd);
            }
        );
    }

    void Scene::End() {
        Systems::scene_stop_signal.publish(this);
    }

    void Scene::Shutdown() {
        Systems::CleanupContext(this);

        for (auto& [id , entity] : entities)
            ydelete entity;
        for (auto& [id , camera] : cameras)
            ydelete camera;
        
        auto view = registry.view<components::ID>();
        for (auto& entity : view)
            registry.destroy(entity);
    }

}