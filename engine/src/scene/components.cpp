#include "scene/components.hpp"

namespace EngineY {

namespace components {

    glm::mat4& Transform::Model() {
        model = glm::mat4(1.0f);
        model = glm::translate(model , position);
        model = glm::rotate(model , rotation.x , glm::vec3(1.0f , 0.0f , 0.0f));
        model = glm::rotate(model , rotation.y , glm::vec3(0.0f , 1.0f , 0.0f));
        model = glm::rotate(model , rotation.z , glm::vec3(0.0f , 0.0f , 1.0f));
        model = glm::scale(model , scale);
        return model;
    }

    Transform::Transform(const glm::vec3& pos , const glm::vec3& scale ,
                  const glm::vec3& rotation)
            : position(pos) , rotation(rotation) , scale(scale) {
        model = glm::mat4(1.0f);
        model = glm::translate(model , pos);
        model = glm::rotate(model , rotation.x , glm::vec3(1.0f , 0.0f , 0.0f));
        model = glm::rotate(model , rotation.y , glm::vec3(0.0f , 1.0f , 0.0f));
        model = glm::rotate(model , rotation.z , glm::vec3(0.0f , 0.0f , 1.0f));
        model = glm::scale(model , scale);
    }

}

}
