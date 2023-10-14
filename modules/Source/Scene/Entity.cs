using System;
using System.Collections.Generic;

namespace YE {
    public class Entity : IEquatable<Entity> {

        private Entity parent;
        
        private Dictionary<Type , Component> components = new Dictionary<Type , Component>();

        public Entity Parent {
            get {
                ulong id = Engine.GetEntityParent(Id);
                if (parent == null || parent.Id != id)
                    parent = Engine.IsEntityValid(id) ? new Entity(id) : null;
                return parent;
            }
            set => Engine.SetEntityParent(Id, value.Id);
        }

        protected Transform transform;

        protected Entity() {
            Id = 0;
            // using this constructor will cause use to lose access to components
            //  because then the engine will have no connection with the script
            transform = new Transform { Entity = this };
            components.Add(typeof(Transform) , transform);
        }

        public Entity(ulong id) {
            Id = id;
            transform = new Transform { Entity = this };
            components.Add(typeof(Transform) , transform);
        }

        public event Action<Entity> Destroyed;

        public readonly ulong Id;
        public string Name => GetComponent<ID>().Name;

        public virtual void Create() {
        }

        public virtual void Update(float dt) {
        }

        public virtual void Destroy() {
        }

        public T CreateComponent<T>() where T : Component, new() {
            if (HasComponent<T>())
                return GetComponent<T>();
            
            Type comp_type = typeof(T);
            Engine.EntityAddComponent(Id, comp_type);
            T comp = new T { Entity = this };
            components.Add(comp_type , comp);
            
            return comp;
        }

        public bool HasComponent<T>() where T : Component => Engine.EntityHasComponent(Id, typeof(T));
        
        public T GetComponent<T>() where T : Component, new(){
            Type comp_type = typeof(T);
            
            if (!HasComponent<T>()) {
                Engine.LogWarn($"Entity {Name} does not have component {comp_type.Name} !");
                return null;
            }
            
            components.TryGetValue(comp_type , out Component comp);
            if (comp == null)
                return CreateComponent<T>();
            
            return comp as T;
        }

        public bool RemoveComponent<T>() where T : Component {
            Type comp_type = typeof(T);
            bool remvd = Engine.EntityRemoveComponent(Id, comp_type);
            
            if (remvd) {
                components.Remove(comp_type);
            }
            
            return remvd;
        }

        public override bool Equals(object obj) => obj is Entity other && Equals(other);

        public bool Equals(Entity other) {
            if (other is null)
                return false;
            
            if (ReferenceEquals(this , other))
                return true;

            return Id == other.Id;
        }

        public override int GetHashCode() => (int)Id; 

        public static bool operator ==(Entity left , Entity right) => left is null ? right is null : left.Equals(right);
        public static bool operator !=(Entity left , Entity right) => !(left == right);
    }
}