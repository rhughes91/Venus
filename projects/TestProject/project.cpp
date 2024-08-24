#include "project.h"
#include <iostream>

void project::initialize(Application& app)
{
    object::ecs& container = app.getScene().container;

    /*
        Both generic scripts and component scripts are created in the same way; the differences lie in how their key functions are created:
        -----------------------------------------------------------------------------------------------------------------------------------
        Generic scripts do not need to iterate over its internal list of entities, and it does not need to have any requirements (they may be added, but they are ineffective).

        Component scripts are scripts that are meant to be added to entities, like 'Transform' or 'Model'. The user can iterate over the 'container.entities<T>()' field.
        Requirements can be added so that an entity must contain both the custom script and the required components in order to be added to the entity list.

        Provided script functions are: 
            load (runs once before the event loop),
            start (runs once after 'load' is completed),
            update (runs every frame during the event loop), 
            lateUpdate (runs after 'update' every frame),
            fixedUpdate (runs once every 200 milliseconds),
            render (runs after 'lateUpdate' every frame; sends render calls to screen texture)
            destroy (runs once after the event loop)
        
        More functions can be added to the Application's container, but they must be explicitly called by the user.
    */

    struct CameraLook // This is an example of a component script.
    {
        float mouseSensitivity = 0.25f;
        Vector2 lastCursorPosition, angle;
    };
    auto& look = container.createSystem<CameraLook, CameraLook, Camera>();
    look.setFunction(object::fn::START, []
    (object::ecs& container, object::ecs::system& script, void *data)
    {
        Window& win = Application::data(data).window();
        for(entity e : container.entities<CameraLook>()) // This will iterate through each of the entities added to the system. This list should not be altered in any way, as entities are automatically added and removed to their designated systems.
        {
            /*
                'getComponent' is a function of the object::ecs struct, and the entity is passed as a parameter.
                If the component needs to be modified in any way, it must be stored as reference, as seen below.
                If the component requires an explicit serialization function, it cannot be stored as a reference; 'setComponent' can be used in this case.
            */
            CameraLook& cameraLook = container.getComponent<CameraLook>(e);
            cameraLook.lastCursorPosition = win.center();
            cameraLook.angle = {4.7f, 16.3f};
        }
    });
    look.setFunction(object::fn::UPDATE, []
    (object::ecs& container, object::ecs::system& script, void *data)
    {
        Window& win = Application::data(data).window();
        for(auto& entity : container.entities<CameraLook>())
        {
            CameraLook& cameraLook = container.getComponent<CameraLook>(entity);

            cameraLook.angle = vec2::modf(cameraLook.angle + (cameraLook.lastCursorPosition - win.cursorScreenPosition()) * cameraLook.mouseSensitivity, 2*M_PI);
            if(math::abs(cameraLook.angle.y) > M_PI/2)
            {
                cameraLook.angle.y = M_PI/2 * math::sign(cameraLook.angle.y);
            }
            container.getComponent<Camera>(win.screen.camera).front = Vector3(std::sin(cameraLook.angle.x), -std::sin(cameraLook.angle.y), std::cos(cameraLook.angle.x)).normalized();

            cameraLook.lastCursorPosition = win.cursorScreenPosition();
        }
    });

    struct Movement // This is an example of a generic script. The fields stored here act as static fields across each of the key functions. Component script fields can also be accessed statically, but this is not usually intended behavior.
    {
        float speed = 0.1f;
    };
    auto& movement = container.createSystem<Movement>();
    {
        movement.setFunction(object::fn::START, []
        (object::ecs& container, object::ecs::system& script, void *data)
        {
            Window& win = Application::data(data).window();

            entity camera = container.createEntity();
            container.addComponent<Transform>(camera, Transform{Vector3(0, 0, -20)});
            container.addComponent<CameraLook>(camera); // Notice that one of the requirements for this component (Camera) was added after CameraLook was added. Components never need to be added in a specific order.
            container.addComponent<Camera>(camera, Camera(Color(0.5f, 0.5f, 5), win.aspectRatioInv(), vec3::forward, vec3::up, 0.01f, 400)); // A camera is required to see renderable objects.

            win.setCamera(camera); // While only one camera can be active at a single time, the camera can be changed at any point in the event loop.
        });
        movement.setFunction(object::fn::FIXED_UPDATE, []
        (object::ecs& container, object::ecs::system& script, void *data)
        {
            Window& win = Application::data(data).window();
            Movement& movement = script.getInstance<Movement>();

            entity camera = win.screen.camera;
            Camera& cam = container.getComponent<Camera>(camera);
            Transform& transform = container.getComponent<Transform>(camera);

            /* Standard format for detecting an input: key::pressed, key::held, key::released. */
            if(key::held(key::W) || key::held(key::UP))
            {
                transform.position += cam.front * movement.speed;
            }
            /* The KeyCode values used previously are equal to their respective ASCII values (if the key has a valid ASCII value). */
            if(key::held('A') || key::held(key::LEFT))
            {
                transform.position += cam.up.cross(cam.front).normalized() * movement.speed;
            }
            /* Input detection also allows for an array of key types as a parameter; 'key::held' returns true if any of the provided keys are held, acting as an OR gate. This is the recommended way of detecting multiple key inputs. */
            if(key::held({key::S, key::DOWN}))
            {
                transform.position += -cam.front * movement.speed;
            }
            /* ASCII values and KeyCode values are interchangeable in a key array as long as the ASCII value is initialized as a KeyCode */
            if(key::held({key::KeyCode('D'), key::RIGHT}))
            {
                transform.position += cam.front.cross(cam.up).normalized() * movement.speed;
            }
            if(key::held(key::SPACE))
            {
                transform.position.y += movement.speed;
            }
            if(key::held(key::LEFT_SHIFT))
            {
                transform.position.y -= movement.speed;
            }

            if(key::held(key::MINUS))
            {
                cam.fov -= 0.1f;
            }
            if(key::held(key::EQUAL))
            {
                 cam.fov += 0.1f;
            }
        });
    }

    struct Event
    {
        entity spot, point;
        entity floor;
    };
    auto& event = container.createSystem<Event>();
    {
        event.setFunction(object::fn::LOAD, []
        (object::ecs& container, object::ecs::system& script, void *data)
        {
            Event& event = script.getInstance<Event>();

            entity sphere = container.createEntity();
            container.addComponent<Transform>(sphere, Transform(Vector3(0, -0.5f, 0), 2));
            container.addComponent<Model>(sphere, Model(Texture::get("default"), Mesh::get("sphere")));
            container.addComponent<AdvancedShader>(sphere, {color::SOFTPEACH, 0.15f, 0.6f, 0.25f, 256});
            container.addComponent<Audio>(sphere, Audio(Audio::get("breeze.wav")));

            event.floor = container.createEntity();
            container.addComponent<Transform>(event.floor, Transform(Vector3(0, -2, -30), 20, Quaternion(math::radians(90), vec3::left)));
            container.addComponent<Model>(event.floor, Model(Texture::get("crate.png"), Mesh::get("square")));
            container.addComponent<AdvancedShader>(event.floor, {color::WHITE, 0.2f, 0.5f, 0.1f, 64});
            container.addComponent<MeshAddon>(event.floor, // MeshAddon attaches copies of the original mesh with different Transforms so that they are rendered in the same draw call. If this component is added to an object, changes to this object's Transform will likely cause unwanted behavior.
            MeshAddon
            ({
                {{Vector3(0, 0, 20)}}, {{Vector3(0, 0, 40)}}, {{Vector3(0, 0, 60)}}, {{Vector3(20, 0, 0)}}, {{Vector3(20, 0, 20)}}, {{Vector3(20, 0, 40)}}, {{Vector3(20, 0, 60)}}, {{Vector3(-20, 0, 0)}}, {{Vector3(-20, 0, 20)}}, {{Vector3(-20, 0, 40)}}, {{Vector3(-20, 0, 60)}}
            }));

            entity spotlight = container.createEntity();
            event.spot = spotlight;
            container.addComponent<Transform>(spotlight, {Vector3(0, 2, 0), 0.25f});
            container.addComponent<Model>(spotlight, Model(Texture::get("default"), Mesh::get("cube")));
            container.addComponent<SimpleShader>(spotlight, color::WHITE);
            container.addComponent<SpotLight>(spotlight, SpotLight(vec3::down, color::WHITE, 1.0f, object::brightness(5), std::cos(math::radians(30.0f)), std::cos(math::radians(20.0f))));

            entity pointlight = container.createEntity();
            event.point = pointlight;
            container.addComponent<Transform>(pointlight, {Vector3(0, -0.1f, 2), 0.1f});
            container.addComponent<Model>(pointlight, Model(Texture::get("default"), Mesh::get("sphere")));
            container.addComponent<SimpleShader>(pointlight, color::PRIMROSEPETAL);
            container.addComponent<PointLight>(pointlight, PointLight(color::PRIMROSEPETAL, 1.0f, object::brightness(3)));
        });
        event.setFunction(object::fn::UPDATE, []
        (object::ecs& container, object::ecs::system& script, void *data)
        {
            Time& time = Application::data(data).time;
            Event& event = script.getInstance<Event>();

            float frequency = 1.0f;
            float index = math::modf(time.runtime, 32);
            container.getComponent<SpotLight>(event.spot).color = 
            container.getComponent<SimpleShader>(event.spot).color = 
                Color(std::sin(frequency*index + 0) * 127 + 128, std::sin(frequency*index + 2) * 127 + 128, std::sin(frequency*index + 4) * 127 + 128); // This simply changes the spotlight's color into a rainbow.
            container.getComponent<Transform>(event.point).position = Vector3(std::sin(2*time.runtime) * 2, -0.1f, std::cos(2*time.runtime)) * 2;

            if(key::pressed(key::P))
            {
                container.getComponent<Audio>(0).play();
            }
        });
        event.setFunction(object::fn::DESTROY, []
        (object::ecs& container, object::ecs::system& script, void *data)
        {
            Time& time = Application::data(data).time;
            std::cout << "Your application ran for " << time.runtime << " seconds." << std::endl;
        });
    }
}