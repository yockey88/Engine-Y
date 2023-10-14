#ifndef YE_CAMERA_HPP
#define YE_CAMERA_HPP

#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace YE {

    enum class CameraType {
        INVALID = 0 ,
        PERSPECTIVE = 1 ,
        ORTHOGRAPHIC = 2
    };

    class Camera {
        CameraType type;

        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view_projection = glm::mat4(1.0f);

        glm::vec3 position = glm::vec3(0.0f , 0.0f , 3.0f);
        glm::vec3 front = glm::vec3(0.0f , 0.0f , -1.0f);
        glm::vec3 up = glm::vec3(0.0f , 1.0f , 0.0f);
        glm::vec3 right = glm::vec3(1.0f , 0.0f , 0.0f);
        glm::vec3 world_up = glm::vec3(0.0f , 1.0f , 0.0f);

        // roll , pitch , yaw 
        glm::vec3 euler_angles = glm::vec3(-90.0f , 0.0f , 0.0f);

        glm::ivec2 viewport = glm::ivec2(800 , 600);
        // near clip , far clip
        glm::vec2 clip = glm::vec2(0.1f , 100.0f);
        glm::vec2 mouse = glm::vec2(0.0f , 0.0f);
        glm::vec2 last_mouse = glm::vec2(0.0f , 0.0f);
        glm::vec2 delta_mouse = glm::vec2(0.0f , 0.0f);

        float speed = 0.1f;
        float sensitivity = 0.2f;
        float fov = 75.0f;
        float zoom = 45.0f;

        bool constrain_pitch = true;

        bool is_primary = false;

        std::function<void(Camera* camera , float dt)> mouse_callback = nullptr;
        std::function<void(Camera* camera , float dt)> keyboard_callback = nullptr;

        void CalculateOrthographicProjection(const glm::ivec2& win_size);
        void CalculatePerspectiveProjection(const glm::ivec2& win_size);

        void CalculateViewMatrix();
        void CalculateProjectionMatrix();

        Camera(Camera&&) = delete;
        Camera(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;
        Camera& operator=(const Camera&) = delete;

        public:
            Camera(CameraType type = CameraType::PERSPECTIVE)
                : type(type) {}
            ~Camera() {}

            void Recalculate();

            void Rotate(const glm::vec3& rotation);

            void Update(float dt);

            inline void SetType(CameraType type) { this->type = type; }

            inline void MoveForward(float dt) { position += front * speed * dt; }
            inline void MoveBackward(float dt) { position -= front * speed * dt; }
            inline void MoveLeft(float dt) { position -= right * speed * dt; }
            inline void MoveRight(float dt) { position += right * speed * dt; }
            inline void MoveUp(float dt) { position += up * speed * dt; }
            inline void MoveDown(float dt) { position -= up * speed * dt; }
            inline void Move(const glm::vec3& direction , float dt) { position += direction * speed * dt; }

            inline void SetPosition(const glm::vec3& position) { this->position = position; }
            inline void SetFront(const glm::vec3& front) { this->front = front; }
            inline void SetUp(const glm::vec3& up) { this->up = up; }
            inline void SetRight(const glm::vec3& right) { this->right = right; }
            inline void SetWorldUp(const glm::vec3& world_up) { this->world_up = world_up; }

            inline void SetOrientation(const glm::vec3& orientation) { this->euler_angles = orientation; }
            inline void SetYaw(float yaw) { this->euler_angles.x = yaw; }
            inline void SetPitch(float pitch) { this->euler_angles.y = pitch; }
            inline void SetRoll(float roll) { this->euler_angles.z = roll; }

            inline void SetViewport(const glm::ivec2& viewport) { this->viewport = viewport; }
            inline void SetClip(const glm::vec2& clip) { this->clip = clip; }
            inline void SetMousePos(const glm::vec2& mouse) { this->mouse = mouse; }
            inline void SetLastMouse(const glm::vec2& last_mouse) { this->last_mouse = last_mouse; }
            inline void SetDeltaMouse(const glm::vec2& delta_mouse) { this->delta_mouse = delta_mouse; }

            inline void SetSpeed(float speed) { this->speed = speed; }
            inline void SetSensitivity(float sensitivity) { this->sensitivity = sensitivity; }
            inline void SetFOV(float fov) { this->fov = fov; }
            inline void SetZoom(float zoom) { this->zoom = zoom; }

            inline void SetConstrainPitch(bool constrain_pitch) { this->constrain_pitch = constrain_pitch; }

            inline void SetPrimary(bool primary) { this->is_primary = primary; }

            inline void RegisterMouseCallback(std::function<void(Camera* camera , float dt)> callback) { mouse_callback = callback; }
            inline void RegisterKeyboardCallback(std::function<void(Camera* camera , float dt)> callback) { keyboard_callback = callback; }

            void UnregisterMouseCallback();
            void UnregisterKeyboardCallback();

            inline const glm::mat4& View() { Recalculate(); return view; }
            inline const glm::mat4& Projection() { Recalculate(); return projection; }
            inline const glm::mat4& ViewProjection() { Recalculate(); return view_projection; }
            inline void View(glm::mat4& view) { Recalculate(); view = this->view; }
            inline void Projection(glm::mat4& projection) { Recalculate(); projection = this->projection; }
            inline void ViewProjection(glm::mat4& view_projection) { Recalculate(); view_projection = this->view_projection; }

            inline CameraType Type() const { return type; }

            inline const glm::vec3& Position() const { return position; }
            inline const glm::vec3& Front() const { return front; }
            inline const glm::vec3& Up() const { return up; }
            inline const glm::vec3& Right() const { return right; }
            inline const glm::vec3& WorldUp() const { return world_up; }

            inline const glm::vec3& Orientation() const { return euler_angles; }
            inline float Yaw() const { return euler_angles.x; }
            inline float Pitch() const { return euler_angles.y; }
            inline float Roll() const { return euler_angles.z; }

            inline const glm::ivec2& Viewport() const { return viewport; }
            inline const glm::vec2& Clip() const { return clip; }
            inline const glm::vec2& Mouse() const { return mouse; }
            inline const glm::vec2& LastMouse() const { return last_mouse; }
            inline const glm::vec2& DeltaMouse() const { return delta_mouse; }

            inline float Speed() const { return speed; }
            inline float Sensitivity() const { return sensitivity; }
            inline float FOV() const { return fov; }
            inline float Zoom() const { return zoom; }

            inline bool ConstrainPitch() const { return constrain_pitch; }

            inline bool IsPrimary() const { return is_primary; }
    };

}

#endif