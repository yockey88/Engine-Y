#include "scene/components.hpp"

#include "rendering/vertex_array.hpp"
#include "rendering/shader.hpp"
#include "rendering/texture.hpp"
#include "rendering/font.hpp"
#include "rendering/model.hpp"

namespace YE {

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
            : position(pos) , scale(scale) , rotation(rotation) {
        model = glm::mat4(1.0f);
        model = glm::translate(model , pos);
        model = glm::rotate(model , rotation.x , glm::vec3(1.0f , 0.0f , 0.0f));
        model = glm::rotate(model , rotation.y , glm::vec3(0.0f , 1.0f , 0.0f));
        model = glm::rotate(model , rotation.z , glm::vec3(0.0f , 0.0f , 1.0f));
        model = glm::scale(model , scale);
    }

}

}