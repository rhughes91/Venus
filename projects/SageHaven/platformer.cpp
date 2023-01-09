#include "platformer.h"
#include <list>

void project::initialize()
{
    struct SmoothFollower : Script
    {
        Entity target;
        float speed;
        SmoothFollower(Entity target__ = 0, float speed__ = 0) : target(target__), speed(speed__) {}
    };
    SmoothFollower& smoothFollower = object::initializeScript<SmoothFollower>();
    {
        smoothFollower.addRequirement<Transform>();
        smoothFollower.lateUpdate([]
        (System& script)
        {
            for(auto const& entity : script.entities())
            {
                SmoothFollower& follower = object::getComponent<SmoothFollower>(entity);
                Vector3& position = object::getComponent<Transform>(entity).position;
                position = Vector3(vec2::lerp(object::getComponent<Transform>(entity).position, object::getComponent<Transform>(follower.target).position, 1-std::pow(follower.speed, event::delta())), position.z);
            }
        });
    }

    struct Movement : Script
    {
        key::KeyArray keyLeft = {key::LEFT, key::A}, keyRight = {key::RIGHT, key::D}, keyJump = {key::C, key::W, key::UP, key::SPACE};
        float height = 3.5f, gravity = 50;
        int gForce = -1;

        Buffer jumpBuffer = Buffer(4), coyoteBuffer = Buffer(3);
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

                if(key::pressed({key::Z, key::LEFT_SHIFT}))
                {
                    movement.gForce *= -1;
                }

                if(key::pressed(movement.keyJump))
                {
                    movement.jumpBuffer.prime();
                }
                else if(!key::held(movement.keyJump))
                {
                    movement.jumpBuffer.reset();
                }

                if((physics.collisions[physics::DOWN] && movement.gForce < 0) || (physics.collisions[physics::UP] && movement.gForce > 0))
                {
                    movement.coyoteBuffer.prime();
                }

                physics.addForce(Vector2((key::held(movement.keyRight)-key::held(movement.keyLeft)) * 50, 0));
                physics.addForce(Vector2(0, movement.gravity*movement.gForce*physics.mass));
                
                if(movement.jumpBuffer.primed && movement.coyoteBuffer.primed)
                {
                    physics.velocity.y = 0;
                    physics.addImpulse(vec3::down * movement.gForce * std::sqrt(movement.height*2*movement.gravity));

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
        Transform *playerTransform, *cameraTransform, *moveTransform, *hmoveTransform;
        Physics2D *cameraPhysics;
    };
    Event& script = object::initializeScript<Event>();
    {
        // START
        script.load([]
        (System& script)
        {
            // event::freezeTime(true);
            auto& data = script.data<Event>();

            mesh::load("test", shape::double_square());

            texture::load("spacebar.png", texture::PNG);
            texture::load("block.png", texture::PNG);
            texture::load("Amina/Walk/amina_walk0000.png", texture::PNG);

            Material defaultMaterial = Material(shader::get("simple_shader"));

            Object jump("button", false);
            jump.addComponent<Rect>(Rect(Alignment(alignment::BOTTOM, alignment::CENTER), Vector2(0, 0.075f), Vector2(0.5f, 0.125f)));
            jump.addComponent<Button>(Button([]
                (Entity entity)
                {
                    Physics2D& physics = object::find("player").getComponent<Physics2D>();
                    auto& movement = object::find("player").getComponent<Movement>();

                    physics.velocity.y = 0;
                    physics.addImpulse(vec3::up * -movement.gForce * std::sqrt(movement.height*2*movement.gravity));
                }, 'J'
            ));
            jump.addComponent<Model>(Model(color::RED, Material(shader::get("ui_shader"), shader::ui), mesh::get("square"), texture::get("spacebar.png")));

            Object floor("floor", false);
            floor.addComponent<Transform>(Transform(Vector3(0, -1.5f, 0), Vector3(120, 2, 0)));
            floor.addComponent<Model>(Model(color::DARK_GREEN, defaultMaterial, mesh::get("square"), texture::get("block.png")));
            floor.addComponent<BoxCollider>();

            floor.clone().getComponent<Transform>() = Transform(Vector3(0, 29, 0), Vector3(120, 1, 0));
            floor.clone().getComponent<Transform>() = Transform(Vector3(0, -29, 0), Vector3(100, 1, 0));

            Object tile("tile", false);
            tile.addComponent<Transform>(Transform(Vector3(3, 2, 0), Vector3(1, 1, 0)));
            tile.addComponent<Model>(Model(color::DARK_GREEN, defaultMaterial, mesh::get("square"), texture::get("block.png")));
            tile.addComponent<MeshAddon>(MeshAddon({Vector3(-1, 0), Vector3(-2, 0), Vector3(1, 0), Vector3(2, 0)}));
            tile.addComponent<BoxCollider>(BoxCollider(Vector3(5, 1, 1)));

            Object wall("wall", false);
            wall.addComponent<Transform>(Transform(Vector3(-59.5f, 4.5f, 0), Vector3(1, 10, 0)));
            wall.addComponent<Model>(Model(color::DARK_GREEN, defaultMaterial, mesh::get("square"), texture::get("block.png")));
            wall.addComponent<BoxCollider>();

            wall.clone().getComponent<Transform>() = Transform(Vector3(59.5, 4.5f, 0), Vector3(1, 10, 0));
            wall.clone().getComponent<Transform>() = Transform(Vector3(59.5, 23.5f, 0), Vector3(1, 10, 0));

            for(int i=-1; i<20; i++)
            {
                Object block("block");
                block.addComponent<Transform>(Transform(Vector3(8, 0.25f+i*0.5f, -0.1f), Vector3(1, 1, 0)*0.5f));
                block.addComponent<Model>(Model(i%2 ? color::BLACK:Color(0.25f, 0.25f, 0.25f, 1), defaultMaterial, mesh::get("square"), texture::get("default.png")));
            }

            Object player("player");
            Vector3& playerPosition = player.addComponent<Transform>(Transform(Vector3(0, 0.2125f, 0), 1.5f)).position;
            player.addComponent<BoxCollider>(BoxCollider(Vector3(0.5f, 0.95f, 1), Vector3(-0.075f, 0, 0), physics::collisionTrigger, physics::collisionMiss));
            player.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("square"), texture::get("Amina/Walk/amina_walk0000.png")));
            player.addComponent<Movement>();
            player.addComponent<Physics2D>(Physics2D(1, 50, Vector3(10, 57.5f)));

            Object camera("camera");
            camera.addComponent<Transform>(Transform{Vector3(playerPosition.x, playerPosition.y, 20)});
            camera.addComponent<SmoothFollower>(SmoothFollower(player.data, 0.001f));
            camera.addComponent<Camera>(Camera(color::SKY_BLUE, vec3::back, vec3::up));
            window::setCamera(camera.data);
        });
    }
}