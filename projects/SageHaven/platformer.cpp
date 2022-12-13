#include "platformer.h"
#include <list>

void project::initialize()
{
    
    struct Movement : Script
    {
        key::KeyCode keyLeft = key::LEFT, keyRight = key::RIGHT, keyJump = key::C;
        float height = 3.5f, gravity = 50;
        int gForce = -1;

        Buffer jumpBuffer = Buffer(6), coyoteBuffer = Buffer(4);
    };
    Movement& movement = object::initializeScript<Movement>();
    {
        movement.addRequirement<Transform>();
        movement.addRequirement<Physics2D>();
        movement.update([]
        (System& script)
        {
            auto& movement = script.data<Movement>();
            for(const auto& entity : script.entities())
            {
                auto& physics = object::getComponent<Physics2D>(entity);

                if(key::pressed(key::Z))
                    movement.gForce *= -1;

                if(key::pressed(movement.keyJump))
                {
                    movement.jumpBuffer.prime();
                }
                else if(!key::held(movement.keyJump))
                {
                    movement.jumpBuffer.reset();
                }

                if((physics.collisions[physics::DOWN] && movement.gForce < 0) || (physics.collisions[physics::UP] && movement.gForce > 0))
                    movement.coyoteBuffer.prime();

                physics.addForce(Vector2((key::held(movement.keyRight)-key::held(movement.keyLeft)) * 50, 0));
                physics.addForce(Vector2(0, movement.gravity*movement.gForce*physics.mass));
                
                if(movement.jumpBuffer.primed && movement.coyoteBuffer.primed)
                {
                    physics.velocity.y = 0;
                    physics.addImpulse(vec3::up * -movement.gForce * std::sqrt(movement.height*2*movement.gravity));

                    movement.jumpBuffer.reset();
                    movement.coyoteBuffer.reset();
                }
            }
        });

        movement.fixedUpdate([]
        (System& script)
        {
            auto& movement = script.data<Movement>();
            for(const auto& entity : script.entities())
            {
                movement.jumpBuffer.update();
                movement.coyoteBuffer.update();
            }
        });
    }
    
    struct Event : Script
    {
        Transform *playerTransform, *cameraTransform;
        Physics2D *cameraPhysics;
    };
    Event& script = object::initializeScript<Event>();
    {
        // START
        script.load([]
        (System& script)
        {
            auto& data = script.data<Event>();

            texture::load("spacebar.png", texture::PNG);

            Material defaultMaterial = Material(shader::get("simple_shader"));
            Object jump("button");
            jump.addComponent<Rect>(Rect(Alignment(alignment::BOTTOM, alignment::CENTER), Vector2(0, 0.025f), Vector2(0.5f, 0.125f)));
            jump.addComponent<Button>(Button([]
                (Entity entity)
                {
                    Physics2D& physics = object::find("player").getComponent<Physics2D>();
                    auto& movement = object::find("player").getComponent<Movement>().data<Movement>();

                    physics.velocity.y = 0;
                    physics.addImpulse(vec3::up * -movement.gForce * std::sqrt(movement.height*2*movement.gravity));
                }, 'J'
            ));
            jump.addComponent<Model>(Model(color::WHITE, Material(shader::get("ui_shader")), mesh::get("square"), texture::get("spacebar.png")));
            
            Object player("player");
            data.playerTransform = &player.addComponent<Transform>(Transform(Vector3(0, 0, 0), 1));
            BoxCollider& collider = player.addComponent<BoxCollider>(true);
            collider.setTrigger(physics::collisionTrigger);
            collider.setMiss(physics::collisionMiss);
            player.addComponent<Model>(Model(color::GREY, defaultMaterial, mesh::get("square"), texture::get("default.png")));
            player.addComponent<Movement>();
            player.addComponent<Physics2D>(Physics2D(1, 50, Vector3(10, 57.5f)));

            Object floor("floor");
            floor.addComponent<Transform>(Transform(Vector3(0, -1, 0), Vector3(120, 1, 0)));
            floor.addComponent<Model>(Model(color::DARK_GREEN, defaultMaterial, mesh::get("square"), texture::get("default.png")));
            floor.addComponent<BoxCollider>();

            floor.clone().getComponent<Transform>() = Transform(Vector3(0, 29, 0), Vector3(100, 1, 0));
            floor.clone().getComponent<Transform>() = Transform(Vector3(0, -29, 0), Vector3(100, 1, 0));
            floor.clone().getComponent<Transform>() = Transform(Vector3(5, 2, 0), Vector3(5, 1, 0));

            Object wall("wall");
            wall.addComponent<Transform>(Transform(Vector3(-59.5f, 4.5f, 0), Vector3(1, 10, 0)));
            wall.addComponent<Model>(Model(color::DARK_GREEN, defaultMaterial, mesh::get("square"), texture::get("default.png")));
            wall.addComponent<BoxCollider>();

            wall.clone().getComponent<Transform>() = Transform(Vector3(59.5, 4.5f, 0), Vector3(1, 10, 0));

            for(int i=-1; i<20; i++)
            {
                Object block("block");
                block.addComponent<Transform>(Transform(Vector3(8, 0.25f+i*0.5f, 0), Vector3(1, 1, 0)*0.5f));
                block.addComponent<Model>(Model(i%2 ? color::BLACK:color::GREY, defaultMaterial, mesh::get("square"), texture::get("default.png")));
            }

            Object camera("camera");
            camera.addComponent<Camera>(Camera(2.0f, color::SKY_BLUE, vec3::back, vec3::up));
            data.cameraTransform = &camera.addComponent<Transform>(Transform{Vector3(0, 0, 20)});
            data.cameraPhysics = &camera.addComponent<Physics2D>(Physics2D(1, 0.1f));
            window::setCamera(camera.data);
        });
        
        script.lateUpdate([]
        (System& script)
        {
            auto& data = script.data<Event>();

            Vector3 difference = (vec3::roundTo(data.playerTransform -> position - data.cameraTransform -> position, 4)).normalized();
            data.cameraPhysics -> velocity = vec3::pow(vec3::abs(difference), 0.75f) * vec3::sign0(difference) * Vector3(75, 125);

            // object::find("button").getComponent<Rect>().rotation *= Quaternion(event::delta(), vec3::forward);
        });
    }
}