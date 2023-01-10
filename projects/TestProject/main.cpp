#include "venus.h"


void createScript()
{
    struct Movement : Script
    {
        float speed = 0.1f;
    };
    Movement& movement = object::initializeScript<Movement>();
    {
        /* 
            provided script functions are: 
                load (runs once before the event loop),
                start (runs once after 'load' is completed),
                update (runs every frame during the event loop), 
                lateUpdate (runs after 'update' every frame),
                fixedUpdate (runs once every 200 milliseconds),
                destroy (runs once after the event loop)
        */
        movement.fixedUpdate([]
        (System& script)
        {
            Movement& data = script.data<Movement>();

            Object camera = object::find("camera");
            Transform& transform = camera.getComponent<Transform>(); // if the variable needs to be altered, it must be stored as a reference

            /* standard format for detecting an input: key::pressed, key::held, key::released */
            if(key::held(key::W) || key::held(key::UP))
            {
                transform.position += vec3::forward * data.speed;
            }
            /* The KeyCode values used previously are equal to their respective ASCII values (if the key has a valid ASCII value) */
            if(key::held('A') || key::held(key::LEFT))
            {
                transform.position += vec3::right * data.speed;
            }
            /* 'key::held' also allows for an array of key types as a parameter; 'key::held' returns true if any of the provieded keys are held, acting as an OR gate */
            if(key::held({key::S, key::DOWN}))
            {
                transform.position += vec3::back * data.speed;
            }
            /* ASCII values and KeyCode values are interchangeable in a key array as long as the ASCII value is initialized as a KeyCode */
            if(key::held({key::KeyCode('D'), key::RIGHT}))
            {
                transform.position += vec3::left * data.speed;
            }
        });
        movement.destroy([]
        (System& script)
        {
            /*
                the 'event' namespace holds some data about how the window is running, including:
                    delta (time between each frame in seconds),
                    framerate (time between each frame in frames),
                    time (the total runtime of the application in seconds)
            */
            std::cout << "Your application ran for " << event::time() << " seconds." << std::endl;
        });
    }
}

int main()
{
    if(createWindow("Test Project", 800, 600))
    {
        /* optional window settings can be set using the 'window' namespace */
        // window::hideCursor(true);
        window::enableVSync(true);
        window::setLighting(DirectionalLight(vec3::down, color::WHITE));

        Material defaultMaterial = Material(shader::get("obj_shader"),
        [] 
        (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
        {
            shader::advanced(entity, model, camera, cameraTransform, 0.25f, 0.25f, 0.1f, 64);
        });

        Object camera("camera");
        camera.addComponent<Transform>(Transform{Vector3(0, 0, -20)});
        camera.addComponent<Camera>(Camera(color::SKY_BLUE, vec3::forward, vec3::up));
        window::setCamera(camera.data);

        Object cube("cube");
        cube.addComponent<Transform>(Transform(Vector3(0, 0, 0), 1, Quaternion(math::radians(45), Vector3(1, 1, 0))));
        cube.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("cube"), texture::get("default.png")));

        createScript();
        beginEventLoop();
    }
    else
        return 1;
    return 0;
}