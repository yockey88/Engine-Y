#include "rendering/camera.hpp"

#include <SDL.h>

#include "log.hpp"
#include "core/window.hpp"
#include "rendering/renderer.hpp"

namespace YE {
 
    void Camera::CalculateOrthographicProjection(const glm::ivec2& win_size) {
        projection = glm::ortho(
            static_cast<float>(-win_size.x) / 2.0f ,
            static_cast<float>(win_size.x) / 2.0f ,
            static_cast<float>(-win_size.y) / 2.0f ,
            static_cast<float>(win_size.y) / 2.0f ,
            clip.x , clip.y
        );
    }

    void Camera::CalculatePerspectiveProjection(const glm::ivec2& win_size) {
        projection = glm::perspective(
            glm::radians(fov) , 
            static_cast<float>(win_size.x) / win_size.y , 
            clip.x , clip.y
        ); 
    }

    void Camera::CalculateViewMatrix() { 
        view = glm::lookAt(position , position + front , up); 
    }

    void Camera::CalculateProjectionMatrix() { 
        glm::ivec2 win_size = Renderer::Instance()->ActiveWindow()->GetSize();
        switch (type) {
            case CameraType::ORTHOGRAPHIC: CalculateOrthographicProjection(win_size); break;
            case CameraType::PERSPECTIVE: CalculatePerspectiveProjection(win_size); break;
            default:
                YE_CRITICAL_ASSERTION(false , "CameraType is invalid");
        }
    }

    void Camera::Recalculate() {
        CalculateViewMatrix();
        CalculateProjectionMatrix();
        view_projection = projection * view;
    }

    // Copilot Rotation
    void Camera::Rotate(const glm::vec3& rotation) {
        euler_angles.x += rotation.x;
        euler_angles.y += rotation.y;
        euler_angles.z += rotation.z;

        if (constrain_pitch) {
            if (euler_angles.y > 89.0f)
                euler_angles.y = 89.0f;
            if (euler_angles.y < -89.0f)
                euler_angles.y = -89.0f;
        }

        glm::vec3 front;
        front.x = cos(glm::radians(euler_angles.x)) * cos(glm::radians(euler_angles.y));
        front.y = sin(glm::radians(euler_angles.y));
        front.z = sin(glm::radians(euler_angles.x)) * cos(glm::radians(euler_angles.y));
        this->front = glm::normalize(front);

        right = glm::normalize(glm::cross(this->front , world_up));
        up = glm::normalize(glm::cross(right , this->front));
    }
    
    void Camera::Update(float dt) {
        if (mouse_callback != nullptr)
            mouse_callback(this , dt);
        if (keyboard_callback != nullptr)
            keyboard_callback(this , dt);
    }

    void Camera::UnregisterMouseCallback() {
        mouse_callback = nullptr;
        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
            SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void Camera::UnregisterKeyboardCallback() {
        keyboard_callback = nullptr;
    }

}