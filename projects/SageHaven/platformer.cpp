#include "platformer.h"

extern Window g_window;
extern InputManager g_keyboard;
extern InputManager g_mouse;

void project::initialize()
{
    
    struct Movement : Script
    {
        uint32_t keyLeft = key::LEFT, keyRight = key::RIGHT, keyJump = key::C;
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

                if(g_keyboard.inputs[key::Z].pressed)
                    movement.gForce *= -1;

                if(g_keyboard.inputs[movement.keyJump].pressed)
                {
                    movement.jumpBuffer.prime();
                }
                else if(!g_keyboard.inputs[movement.keyJump].held)
                {
                    movement.jumpBuffer.reset();
                }

                if((physics.collisions[physics::DOWN] && movement.gForce < 0) || (physics.collisions[physics::UP] && movement.gForce > 0))
                    movement.coyoteBuffer.prime();

                physics.addForce(Vector2((g_keyboard.inputs[movement.keyRight].held-g_keyboard.inputs[movement.keyLeft].held) * 50, 0));
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
        Shader uiShader;
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

            Material defaultMaterial = Material(shader::get("obj_shader"));

            Object player("player");
            data.playerTransform = &player.addComponent<Transform>(Vector3(0, 0, 0));
            auto& collider = player.addComponent<BoxCollider>(BoxCollider(true));
            collider.setTrigger(physics::collisionHandler);
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

            // Object bouncePad = floor.clone();
            // bouncePad.getComponent<Transform>() = Transform(Vector3(-8, 0, 0), Vector3(1, 1, 0));
            // bouncePad.getComponent<BoxCollider>().setTrigger(physics::collisionBounce);

            // data.vertTransform = &(floor.clone().getComponent<Transform>() = Transform(Vector3(-15, 5, 0), Vector3(10, 1, 0)));
            // data.horiTransform = &(floor.clone().getComponent<Transform>() = Transform(Vector3(15, 4.5f, 0), Vector3(1, 10, 0)));

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
            Camera& cam = camera.addComponent<Camera>(Camera(2.0f, color::CLEAR, vec3::back, vec3::up));
            data.cameraTransform = &camera.addComponent<Transform>(Transform{Vector3(0, 0, 20)});
            data.cameraPhysics = &camera.addComponent<Physics2D>(Physics2D(1, 0.1f));
            g_window.screen.camera = camera.data;
        });
    }

    script.lateUpdate([]
    (System& script)
    {
        auto& data = script.data<Event>();

        Vector3 difference = (data.playerTransform -> position - data.cameraTransform -> position).normalized();
        data.cameraPhysics -> velocity = vec3::pow(vec3::abs(difference), 0.75f) * vec3::sign0(difference) * Vector3(75, 125);
    });
}