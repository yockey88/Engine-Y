#include "scene/entity.hpp"

#include "core/UUID.hpp"
#include "scene/components.hpp"

namespace EngineY {

    void Entity::SetParent(Entity* parent) {
        auto& id = this->GetComponent<components::ID>();
        auto& parent_id = parent->GetComponent<components::ID>();
        
        auto& grouping = this->GetComponent<components::Grouping>();
        grouping.parent = parent_id.id;
        grouping.children.push_back(id.id);
        
        auto& parent_grouping = parent->GetComponent<components::Grouping>();
        grouping.parent = parent_id.id;
        parent_grouping.children.push_back(id.id);
    }

    void Entity::RemoveParent(Entity* parent) {
        auto& grouping = GetComponent<components::Grouping>();
        grouping.parent = 0;

        auto& parent_grouping = parent->GetComponent<components::Grouping>();
        for (auto it = parent_grouping.children.begin(); it != parent_grouping.children.end(); ++it) {
            if (*it == parent->GetComponent<components::ID>().id) {
                parent_grouping.children.erase(it);
                break;
            }
        }
    }

    void Entity::GiveChild(Entity* child) {
        auto& id = GetComponent<components::ID>();
        auto& child_id = child->GetComponent<components::ID>();

        auto& grouping = GetComponent<components::Grouping>();
        grouping.parent = id.id;
        grouping.children.push_back(child_id.id);
        
        auto& child_grouping = child->GetComponent<components::Grouping>();
        if (child_grouping.parent != 0) {
            Entity* parent = context->GetEntity(child_grouping.parent);
            if (parent != nullptr)
                parent->RemoveChild(child);
        }
        
        child_grouping.parent = id.id;
        child_grouping.children.push_back(child_id.id);
    }

    void Entity::RemoveChild(Entity* child) {
        auto& grouping = GetComponent<components::Grouping>();
        for (auto it = grouping.children.begin(); it != grouping.children.end(); ++it) {
            if (*it == child->GetComponent<components::ID>().id) {
                grouping.children.erase(it);
                break;
            }
        }

        auto& child_grouping = child->GetComponent<components::Grouping>();
        child_grouping.parent = 0;
    }
    
    Entity* Entity::GetParent() {
        auto& grouping = GetComponent<components::Grouping>();
        
        if (grouping.parent == 0)
            return nullptr;

        return context->GetEntity(grouping.parent);
    }

}