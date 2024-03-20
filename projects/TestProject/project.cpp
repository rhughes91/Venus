#include "project.h"
#include <iostream>

void project::initialize()
{
    /*
        Both generic scripts and component scripts are created in the same way; the differences lie in how their key functions are created:
        -----------------------------------------------------------------------------------------------------------------------------------
        Generic scripts do not need to iterate over its internal list of entities, and it does not need to have any requirements (although requirements will not harm these scripts).

        Component scripts are scripts that are meant to be added to Objects, similar to 'Transform' or 'Model'. The user can iterate over the 'script.entities()' field that each script
        automatically contains. Requirements can be added so that an Object must contain both the custom script and the required component in order to be added to the entity list.

        Provided script functions are: 
            load (runs once before the event loop),
            start (runs once after 'load' is completed),
            update (runs every frame during the event loop), 
            lateUpdate (runs after 'update' every frame),
            fixedUpdate (runs once every 200 milliseconds),
            destroy (runs once after the event loop)
    */

    struct CameraLook // This is an example of a component script. In both types of scripts, do not access any properties in the 'window' namespace if you are initializing a script's field, unless the window has already been initialized.
    {
        float mouseSensitivity = 0.25f;
        Vector2 lastCursorPosition, angle;

        // Vector2 aspectRatio = window::aspect(); <== EXAMPLE: If this initialization function was run BEFORE the window was initialized, accessing this field would lead to undefined behavior.
    };
    auto& look = object::createSystem<CameraLook, CameraLook, Camera>();
    look.setFunction(object::fn::START, []
    (object::ecs& container, object::ecs::system& script)
    {
        for(entity e : container.entities<CameraLook>()) // This will iterate through each of the entities added to the system. This list should not be altered in any way, as entities are automatically added and removed to their designated systems.
        {
            /*
                'getComponent' is a function of the object namespace, and the Entity is passed as a parameter.
                If the component needs to be modified in any way, it must be stored as reference, as seen below.
            */
            CameraLook& cameraLook = object::getComponent<CameraLook>(e);
            cameraLook.lastCursorPosition = window::center();
            cameraLook.angle = {4.7f, 16.3f};
        }
    });
    look.setFunction(object::fn::UPDATE, []
    (object::ecs& container, object::ecs::system& script)
    {
        for(auto& entity : container.entities<CameraLook>())
        {
            CameraLook& cameraLook = object::getComponent<CameraLook>(entity);

            cameraLook.angle = vec2::modf(cameraLook.angle + (cameraLook.lastCursorPosition - window::cursorScreenPosition()) * cameraLook.mouseSensitivity, 2*M_PI);
            if(math::abs(cameraLook.angle.y) > M_PI/2)
            {
                cameraLook.angle.y = M_PI/2 * math::sign(cameraLook.angle.y);
            }
            object::getComponent<Camera>(window::camera()).front = Vector3(std::sin(cameraLook.angle.x), -std::sin(cameraLook.angle.y), std::cos(cameraLook.angle.x)).normalized(); // Data for the current active camera Object can be requested and set globally from the window namespace.

            cameraLook.lastCursorPosition = window::cursorScreenPosition();
        }
    });

    struct Movement // This is an example of a generic script. The fields stored here act as static fields across each of the key functions. Component script fields can also be accessed statically, but this is not usually intended behavior.
    {
        float speed = 0.1f;
        entity camera;
    };
    auto& movement = object::createSystem<Movement>();
    {
        movement.setFunction(object::fn::START, []
        (object::ecs& container, object::ecs::system& script)
        {
            entity camera = object::createEntity();
            object::addComponent<Transform>(camera, Transform{Vector3(0, 0, -20)});
            object::addComponent<CameraLook>(camera); // Notice that one of the requirements for this component (Camera) was added after CameraLook was added. Components never need to be added in a specific order.
            object::addComponent<Camera>(camera, Camera(Color(0.5f, 0.5f, 5), vec3::forward, vec3::up)); // A camera is required to see created objects.

            window::setCamera(camera); // While only one camera can be active at a single time, the camera can be changed at any point in the event loop.
        });
        movement.setFunction(object::fn::FIXED_UPDATE, []
        (object::ecs& container, object::ecs::system& script)
        {
            Movement& data = script.getInstance<Movement>();

            entity camera = window::camera();
            Camera& cam = object::getComponent<Camera>(camera);
            Transform& transform = object::getComponent<Transform>(camera);

            /* Standard format for detecting an input: key::pressed, key::held, key::released. */
            if(key::held(key::W) || key::held(key::UP))
            {
                transform.position += cam.front * data.speed;
            }
            /* The KeyCode values used previously are equal to their respective ASCII values (if the key has a valid ASCII value). */
            if(key::held('A') || key::held(key::LEFT))
            {
                transform.position += cam.up.cross(cam.front).normalized() * data.speed;
            }
            /* 'key::held' also allows for an array of key types as a parameter; 'key::held' returns true if any of the provieded keys are held, acting as an OR gate. This is the recommended way of detecting multiple key inputs. */
            if(key::held({key::S, key::DOWN}))
            {
                transform.position += -cam.front * data.speed;
            }
            /* ASCII values and KeyCode values are interchangeable in a key array as long as the ASCII value is initialized as a KeyCode */
            if(key::held({key::KeyCode('D'), key::RIGHT}))
            {
                transform.position += cam.front.cross(cam.up).normalized() * data.speed;
            }
            if(key::held(key::SPACE))
            {
                transform.position.y += data.speed;
            }
            if(key::held(key::LEFT_SHIFT))
            {
                transform.position.y -= data.speed;
            }
        });
    }

    struct Event
    {
        entity spot, point;
    };
    auto& event = object::createSystem<Event>();
    {
        event.setFunction(object::fn::LOAD, []
        (object::ecs& container, object::ecs::system& script)
        {
            Event& data = script.getInstance<Event>();

            entity spotlight = object::createEntity();
            data.spot = spotlight;
            object::addComponent<Transform>(spotlight, {Vector3(0, 2, 0), 0.25f});
            object::addComponent<Model>(spotlight, Model(texture::get("default"), mesh::get("cube")));
            object::addComponent<SimpleShader>(spotlight, color::WHITE);
            object::addComponent<SpotLight>(spotlight, SpotLight(vec3::down, color::WHITE, 1.0f, object::brightness(5), std::cos(math::radians(30.0f)), std::cos(math::radians(20.0f))));

            entity pointlight = object::createEntity();
            data.point = pointlight;
            object::addComponent<Transform>(pointlight, {Vector3(0, -0.1f, 2), 0.1f});
            object::addComponent<Model>(pointlight, Model(texture::get("default"), mesh::get("sphere")));
            object::addComponent<SimpleShader>(pointlight, color::PRIMROSEPETAL);
            object::addComponent<PointLight>(pointlight, PointLight(color::PRIMROSEPETAL, 1.0f, object::brightness(3)));
        });
        event.setFunction(object::fn::UPDATE, []
        (object::ecs& container, object::ecs::system& script)
        {
            Event& data = script.getInstance<Event>();

            float frequency = 1.0f;
            float index = math::modf(event::time(), 32);
            object::getComponent<SpotLight>(data.spot).color = 
            object::getComponent<SimpleShader>(data.spot).color = 
                Color(std::sin(frequency*index + 0) * 127 + 128, std::sin(frequency*index + 2) * 127 + 128, std::sin(frequency*index + 4) * 127 + 128); // This simply changes the spotlight's color into a rainbow.
            object::getComponent<Transform>(data.point).position = Vector3(std::sin(2*event::time()) * 2, -0.1f, std::cos(2*event::time())) * 2;
        });
        event.setFunction(object::fn::DESTROY, []
        (object::ecs& container, object::ecs::system& script)
        {
            /*
                The 'event' namespace holds some data about how the window's event loop is running, including:
                    delta (time between each frame in seconds),
                    framerate (time between each frame in frames),
                    time (the total runtime of the application in seconds)
            */
            std::cout << "Your application ran for " << event::time() << " seconds." << std::endl;
        });
    }
}