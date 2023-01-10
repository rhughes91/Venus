#include "project.h"


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

        /* objects can be initialized at any point before 'getEventLoop()' is run, although it may be cleaner to initialize them in the 'load' function of a script. */
        Object camera("camera");
        camera.addComponent<Transform>(Transform{Vector3(0, 0, -20)});
        camera.addComponent<Camera>(Camera(color::SKY_BLUE, vec3::forward, vec3::up)); // camera is required to see created objects
        window::setCamera(camera.data);

        Object cube("cube");
        cube.addComponent<Transform>(Transform(Vector3(0, 0, 0), 1, Quaternion(math::radians(45), Vector3(1, 1, 0))));
        cube.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("cube"), texture::get("default.png")));

        /* 
            this function was created in a separate '.cpp' file and defined in a seperate header file. when new '.cpp' files are added, their file name must be added to the CMake file in this directory;
            insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. the 'main.cpp' file should not be added to that list.
        */
        project::initialize();

        beginEventLoop();
    }
    else
        return 1;
    return 0;
}