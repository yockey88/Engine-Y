using System;
using System.Runtime.InteropServices;

namespace EngineY {

    public abstract class Component {
        public Entity Entity { get; internal set; }
    }

    public class ID : Component {
        public string Name {
            get => Engine.EntityNameFromId(Entity.Id);
            set => Engine.SetEntityName(Entity.Id , value);
        }
    }
    
    /// <summary>
    ///   Transform component
    /// </summary>
    /// <note>
    ///     A physics body component will override any changes to the position and rotation of the entity
    ///        that are made through the transform component, but only if the physics body is kinematic
    ///         or dynamic. If the physics body is static, the transform component will override the physics body.
    /// </note>
    public class Transform : Component {
        public Vec3 position {
            get {
                Engine.GetEntityPosition(Entity.Id , out var result);
                return result;
            }

            set => Engine.SetEntityPosition(Entity.Id, ref value);
        }

        public Vec3 scale {
            get {
                Engine.GetEntityScale(Entity.Id , out var result);
                return result;
            }
            set => Engine.SetEntityScale(Entity.Id , ref value);
        }

        public Vec3 rotation {
            get {
                Engine.GetEntityRotation(Entity.Id , out var result);
                return result;
            }
            
            set => Engine.SetEntityRotation(Entity.Id , ref value);
        }
    }
    
    public class Renderable : Component {}

    public class TexturedRenderable : Component {}

    public class RenderableModel : Component {}

    public class DirectionalLight : Component {}

    public class PointLight : Component {}

    public class SpotLight : Component {}

    public enum PhysicsBodyType {
        Static ,
        Kinematic ,
        Dynamic
    }
    
    public class PhysicsBody : Component {
        public PhysicsBodyType type {
            get => Engine.GetPhysicsBodyType(Entity.Id);
            set => Engine.SetPhysicsBodyType(Entity.Id , value);
        }

        public Vec3 position {
            get {
                Engine.GetPhysicsBodyPosition(Entity.Id , out var result);
                return result;
            }
            set => Engine.SetPhysicsBodyPosition(Entity.Id , ref value);
        }
        
        public Vec3 rotation {
            get {
                Engine.GetPhysicsBodyRotation(Entity.Id , out var result);
                return result;
            }
            set => Engine.SetPhysicsBodyRotation(Entity.Id , ref value);
        }

        public float mass {
            get => Engine.GetPhysicsBodyMass(Entity.Id);
            set => Engine.SetPhysicsBodyMass(Entity.Id , value);
        }
        
        public void ApplyForce(Vec3 force) => Engine.ApplyForceCenterOfMass(Entity.Id , ref force);
        public void ApplyForce(Vec3 force , Vec3 point) => Engine.ApplyForce(Entity.Id , ref force , ref point);
        
        public void ApplyTorque(Vec3 torque) => Engine.ApplyTorque(Entity.Id , ref torque);
    }

    public class Script : Component {
        // public object instance => Engine.GetScriptInstance(Entity.ID);
    }

    public class NativeScript : Component {
        // public object instance => Engine.GetScriptInstance(Entity.ID);
    }



}