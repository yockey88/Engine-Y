
using System;
using YE;

public class UserEntity : Entity {

    private Random rand = new Random();

    public UserEntity() {}

    public UserEntity(ulong id) 
        : base(id) {}

    public override void Create() {}

    public override void Update(float dt) {
        if (Keyboard.KeyPressed(KeyCode.YE_SPACE)) {
            PhysicsBody body = GetComponent<PhysicsBody>();
            body.ApplyForce(new Vec3(0 , 0.5f , 0));
            
            int xyz = rand.Next(0 , 2);
            int sign = rand.Next(0 , 2) == 0 ? -1 : 1;
            int amount = rand.Next(0, 100) * sign;
            if (xyz == 0) {
                body.ApplyTorque(new Vec3(amount, 0, 0));
            } else if (xyz == 1) {
                body.ApplyTorque(new Vec3(0 , amount , 0));
            } else {
                body.ApplyTorque(new Vec3(0 , 0 , amount));
            }

            transform.scale -= 1;
        }
    }

    public override void Destroy() {}
    
}