#include "project.h"

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

    struct CameraLook : Script // This is an example of a component script. In both types of scripts, do not access any properties in the 'window' namespace if you are initializing a script's field, unless the window has already been initialized.
    {
        float mouseSensitivity = 0.25f;
        Vector2 lastCursorPosition, angle;

        // Vector2 aspectRatio = window::aspect(); <== EXAMPLE: If this initialization function was run BEFORE the window was initialized, accessing this field would lead to undefined behavior.
    };
    CameraLook& look = object::initializeScript<CameraLook>();
    look.addRequirement<Camera>();
    {
        look.start([]
        (System& script)
        {
            for(auto& entity : script.entities()) // This will iterate through each of the entities added to the system. This list should not be altered in any way, as entities are automatically added and removed to their designated systems.
            {
                /*
                    There are two ways of getting an Entity's component: if the Entity is wrapped in the Object structure, getComponent is a method of that struct. Otherwise, getComponent is a function of the object namespace, and the Entity is passed as a parameter.
                    If the component needs to be modified in any way, it must be stored as reference, as seen below.
                */
                CameraLook& cameraLook = object::getComponent<CameraLook>(entity);
                cameraLook.lastCursorPosition = window::center();
            }
        });
        look.update([]
        (System& script)
        {
            for(auto& entity : script.entities())
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
    }

    struct Movement : Script // This is an example of a generic script. The fields stored here act as static fields across each of the key functions. Component script fields can also be accessed statically, but this is not usually intended behavior.
    {
        float speed = 0.1f;
    };
    Movement& movement = object::initializeScript<Movement>();
    {
        movement.start([]
        (System& script)
        {
            Object camera("camera");
            camera.addComponent<Transform>(Transform{Vector3(0, 0, -20)});
            camera.addComponent<CameraLook>(); // Notice that one of the requirements for this component (Camera) was added after CameraLook was added. Components never need to be added in a specific order.

            Camera& cam = camera.addComponent<Camera>(Camera(Color(0.5f, 0.5f, 5), vec3::forward, vec3::up)); // A camera is required to see created objects.
            window::setCamera(camera.data); // While only one camera can be active at a single time, the camera can be changed at any point in the event loop.
        });
        movement.fixedUpdate([]
        (System& script)
        {
            Movement& data = script.data<Movement>();

            Object camera = object::find("camera");
            Camera& cam = camera.getComponent<Camera>();
            Transform& transform = camera.getComponent<Transform>();

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

    struct Event : Script
    {
        SpotLight *light; // Components stored statically should be stored as pointers; references cannot be statically stored, and simple variables cannot be changed.
        Model *model;
        Transform *transform;
    };
    Event& event = object::initializeScript<Event>();
    {
        event.load([]
        (System& script)
        {
            Event& data = script.data<Event>();

            Shader& uiShader = shader::get("ui_shader");

            Object spotlight("light");
            spotlight.addComponent<Transform>({Vector3(0, 2, 0), 0.25f});
            data.model = &spotlight.addComponent<Model>(Model(color::WHITE, Material(shader::get("simple_shader"), shader::simple), mesh::get("cube"), texture::get("default")));
            data.light = &spotlight.addComponent<SpotLight>(SpotLight(vec3::down, color::WHITE, 1.0f, object::brightness(5), std::cos(math::radians(30.0f)), std::cos(math::radians(20.0f))));

            Object pointlight("area");
            data.transform = &pointlight.addComponent<Transform>({Vector3(0, -0.1f, 2), 0.1f});
            pointlight.addComponent<Model>(Model(color::PRIMROSEPETAL, Material(shader::get("simple_shader"), shader::simple), mesh::get("sphere"), texture::get("default")));
            pointlight.addComponent<PointLight>(PointLight(color::PRIMROSEPETAL, 1.0f, object::brightness(3)));
        });
        event.update([]
        (System& script)
        {
            Event& data = script.data<Event>();

            float frequency = 1.0f;
            float index = math::modf(event::time(), 32);
            data.light -> color = data.model -> color = Color(std::sin(frequency*index + 0) * 127 + 128, std::sin(frequency*index + 2) * 127 + 128, std::sin(frequency*index + 4) * 127 + 128); // This simply changes the global directional light's color into a rainbow.
            data.transform -> position = Vector3(std::sin(2*event::time()) * 2, -0.1f, std::cos(2*event::time())) * 2;
        });
        event.destroy([]
        (System& script)
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