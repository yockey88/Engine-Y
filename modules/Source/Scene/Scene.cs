using System.Collections.Generic;

namespace EngineY {
    
    public class Scene {

        private Dictionary<ulong , Entity> entities = new Dictionary<ulong , Entity>();
        
        public Scene() { Id = 0; }
        public Scene(ulong id) { Id = id; }
        
        public readonly ulong Id;
        
        // public Entity[] Entities => Engine.GetEntities(Id);

        public Entity CreateEntity(string name = "[Blank Entity]") => new Entity(Engine.CreateEntity(name));
        
        public void DestroyEntity(Entity entity) {
            if (entity == null)
                return;

            if (!Engine.IsEntityValid(entity.Id))
                return;
            
            if (!entities.Remove(entity.Id))
                entities.Remove((ulong)entity.Name.GetHashCode());
            
            Engine.DestroyEntity(entity.Id);
        }

        public Entity EntityFromName(string name) {
            ulong hash = (ulong)name.GetHashCode();
            Entity result = null;

            if (entities.ContainsKey(hash) && entities[hash] != null) {
                result = entities[hash];
                if (!Engine.IsEntityValid(hash)) {
                    entities.Remove(hash);
                    result = null;
                }
            }

            if (result == null) {
                ulong id = Engine.GetEntityFromName(name);
                Entity new_entity = id != 0 ? new Entity(id) : null;
                
                if (new_entity != null)
                    new_entity.Destroyed += EntityDestroyed;

                result = new_entity;
            }
            
            return result;
        }

        public Entity EntityFromId(ulong id) {
            if (entities.ContainsKey(id) && entities[id] != null) {
                var entity = entities[id];

                if (!Engine.IsEntityValid(entity.Id)) {
                    entities.Remove(id);
                    entity = null;
                }
                
                return entity;
            }

            if (!Engine.IsEntityValid(id))
                return null;
            
            var new_entity = new Entity(id);
            entities[id] = new_entity;
            new_entity.Destroyed += EntityDestroyed;
            return new_entity;
        }
        
        private void EntityDestroyed(Entity entity) {
            entity.Destroyed -= EntityDestroyed;

            if (!entities.Remove(entity.Id))
                entities.Remove((ulong)entity.Name.GetHashCode());
        }
        
        public void SwitchOutScene() {
            foreach (var entity in entities) {
                if (entity.Value == null)
                    continue;
                entity.Value.Destroyed -= EntityDestroyed;
            }
            
            entities.Clear();
        }
    }
    
}