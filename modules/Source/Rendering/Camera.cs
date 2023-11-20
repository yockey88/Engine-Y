
namespace EngineY {
    
    public enum CameraType {
        INVALID ,
        Perspective ,
        Orthographic
    }
    
    public class Camera {

        private Scene context;
        
        public Camera() { Id = 0; }
        public Camera(int id) { Id = id; }
        
        public readonly int Id;

        public CameraType type {
            get => Engine.GetCameraType(Id);
            set => Engine.SetCameraType(Id , value);
        }

        public Scene scene {
            get => context;
        }
        
        public void Recalculate() {
            Engine.RecalculateCameraViewProjectionMatrix(Id);
        }
        
        public Mat4 view {
            get {
                Engine.GetCameraView(Id , out var result);
                return result;
            }
        }
        
        public Mat4 projection {
            get {
                Engine.GetCameraProjection(Id , out var result);
                return result;
            }
        }
        
        public Mat4 view_projection {
            get {
                Engine.GetCameraViewProjection(Id , out var result);
                return result;
            }
        }
        
        public Vec3 position {
            get {
                Engine.GetCameraPosition(Id , out var result);
                return result;
            }
            set => Engine.SetCameraPosition(Id , ref value);
        }
        
        public Vec3 front {
            get {
                Engine.GetCameraFront(Id , out var result);
                return result;
            }
            set => Engine.SetCameraFront(Id , ref value);
        }
        
        public Vec3 up {
            get {
                Engine.GetCameraUp(Id , out var result);
                return result;
            }
            set => Engine.SetCameraUp(Id , ref value);
        }
        
        public Vec3 right {
            get {
                Engine.GetCameraRight(Id , out var result);
                return result;
            }
        }
        
        public Vec3 world_up {
            get {
                Engine.GetCameraWorldUp(Id , out var result);
                return result;
            }
            set => Engine.SetCameraWorldUp(Id , ref value);
        }
        
        public Vec3 euler_angles {
            get {
                Engine.GetCameraEulerAngles(Id , out var result);
                return result;
            }
            set => Engine.SetCameraEulerAngles(Id , ref value);
        }
        
        public Vec2 viewport_size {
            get {
                Engine.GetCameraViewportSize(Id , out var result);
                return result;
            }
            set => Engine.SetCameraViewportSize(Id , ref value);
        }

        public Vec2 clip {
            get {
                Engine.GetCameraClip(Id , out var result);
                return result;
            }
            set => Engine.SetCameraClip(Id , ref value);
        }

        public float near_clip {
            get {
                Engine.GetCameraClip(Id , out var result);
                return result.x;
            }
            set => Engine.SetCameraNearClip(Id , value);
        }

        public float far_clip {
            get {
                Engine.GetCameraClip(Id , out var result);
                return result.y;
            }
            set => Engine.SetCameraFarClip(Id , value);
        }
        
        public Vec2 mouse {
            get {
                Engine.GetCameraMousePos(Id , out var result);
                return result;
            }
        }
        
        public Vec2 last_mouse {
            get {
                Engine.GetCameraLastMousePos(Id , out var result);
                return result;
            }
        }
        
        public Vec2 delta_mouse {
            get {
                Engine.GetCameraDeltaMouse(Id , out var result);
                return result;
            }
        }

        public float speed {
            get => Engine.GetCameraSpeed(Id);
            set => Engine.SetCameraSpeed(Id , value);
        }
        
        public float sensitivity {
            get => Engine.GetCameraSensitivity(Id);
            set => Engine.SetCameraSensitivity(Id , value);
        }
        
        public float fov {
            get => Engine.GetCameraFov(Id);
            set => Engine.SetCameraFov(Id , value);
        }
        
        public float zoom {
            get => Engine.GetCameraZoom(Id);
            set => Engine.SetCameraZoom(Id , value);
        }
        
        public bool constrain_pitch {
            get => Engine.GetCameraConstrainPitch(Id);
            set => Engine.SetCameraConstrainPitch(Id , value);
        }

        public bool primary {
            get => Engine.IsCameraPrimary(Id);
            set => Engine.SetCameraPrimary(Id , value);
        }
    }

    
}