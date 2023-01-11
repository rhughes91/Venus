#include "project.h"

void project::initialize()
{
    /*
        Both generic scripts and component scripts are created in the same way; the differences lie in how their key functions are created:
        -----------------------------------------------------------------------------------------------------------------------------------
        Generic scripts do not need to iterate over its internal list of entities, and it does not need to have any requirements (although requirements will not harm these scripts).

        Component scripts are scripts that are meant to be added to Objects, similar to 'Transform' or 'Model'. The user can iterate over the 'script.entities()' that each script will
        automatically contain. Requirements can be added so that an Object must contain both the custom script and the required component in order to be added to the entity list.
    */
    struct CameraLook : Script // This is an example of a component script. In both types of scripts, do not access any properties in the 'window' namespace if you are initializing a script's field, unless the window has already been intialized.
    {
        float mouseSensitivity = 0.5f;
        Vector2 lastCursorPosition, angle;

        // Vector2 aspectRatio = window::aspect(); <== EXAMPLE: If this initialization function was run BEFORE the window was initialized, accessing this field would lead to undefined behavior.
    };
    CameraLook& look = object::initializeScript<CameraLook>();
    look.addRequirement<Camera>();
    {
        /* 
            Provided script functions are: 
                load (runs once before the event loop),
                start (runs once after 'load' is completed),
                update (runs every frame during the event loop), 
                lateUpdate (runs after 'update' every frame),
                fixedUpdate (runs once every 200 milliseconds),
                destroy (runs once after the event loop)
        */
        look.start([]
        (System& script)
        {
            for(auto& entity : script.entities())
            {
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
                object::getComponent<Camera>(window::camera()).front = Vector3(std::sin(cameraLook.angle.x), -std::sin(cameraLook.angle.y), std::cos(cameraLook.angle.x)).normalized();

                
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
            window::setCamera(camera.data);
        });
        movement.update([]
        (System& script)
        {
            float frequency = 1.0f;
            float index = math::modf(event::time(), 32);
            window::lighting().color = Color(std::sin(frequency*index + 0) * 127 + 128, std::sin(frequency*index + 2) * 127 + 128, std::sin(frequency*index + 4) * 127 + 128); // This simply changes the global directional light's color into a rainbow.
        });
        movement.fixedUpdate([]
        (System& script)
        {
            Movement& data = script.data<Movement>();

            Object camera = object::find("camera");
            Camera& cam = camera.getComponent<Camera>();
            Transform& transform = camera.getComponent<Transform>(); // If the variable needs to be altered, it must be stored as a reference

            /* Standard format for detecting an input: key::pressed, key::held, key::released */
            if(key::held(key::W) || key::held(key::UP))
            {
                transform.position += cam.front * data.speed;
            }
            /* The KeyCode values used previously are equal to their respective ASCII values (if the key has a valid ASCII value) */
            if(key::held('A') || key::held(key::LEFT))
            {
                transform.position += cam.up.cross(cam.front).normalized() * data.speed;
            }
            /* 'key::held' also allows for an array of key types as a parameter; 'key::held' returns true if any of the provieded keys are held, acting as an OR gate */
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
        movement.destroy([]
        (System& script)
        {
            /*
                The 'event' namespace holds some data about how the window is running, including:
                    delta (time between each frame in seconds),
                    framerate (time between each frame in frames),
                    time (the total runtime of the application in seconds)
            */
            std::cout << "Your application ran for " << event::time() << " seconds." << std::endl;
        });
    }
}