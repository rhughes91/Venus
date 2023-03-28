#include "project.h"


int main()
{
    /* 
        This function was created in a separate '.cpp' file and defined in a seperate header file. When new '.cpp' files are added, their file name must be added to the CMake file in this directory;
        Insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. The 'main.cpp' file should not be added to that list.
    */
    project::initialize();
    
    if(createWindow("Test Project", 800, 600))
    {
        /* Optional window settings can be set using the 'window' namespace. */
        window::lockCursor(true);
        // window::enableVSync(true);
        window::setLighting(DirectionalLight(vec3::down, color::WHITE, 0.4f));
        window::setFontColor(color::WHITE);

        texture::load("block", texture::PNG);
        texture::load("crate", texture::PNG);
        texture::load("floor", texture::PNG);
        file::loadPNG(source::root() + source::texture() + "block.png");

        mesh::load("sphere", shape::sphere(1, 6));

        Material defaultMaterial = Material(shader::get("object_shader"),
        [] 
        (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
        {
            shader::advanced(entity, model, camera, cameraTransform, Vector3(0.2f, 0.5f, 0.1f), 64);
        });

        Material shiny = Material(shader::get("object_shader"),
        [] 
        (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
        {
            shader::advanced(entity, model, camera, cameraTransform, Vector3(0.15f, 0.6f, 0.25f), 256);
        });

        /*
            Objects can be initialized at any point before the event loop begins and after the window is created, although it may be cleaner to initialize them in a script's 'load' and 'start' functions (as seen in project.cpp).
            Scripts can be defined and initialized at any point before the event loop begins, even before the window is created (see 'project::initialize()').

            In this way, Objects can be defined and updated safely at any point before the loop begins; however, Objects defined in this way are technically defined after any Objects created outside of a script, 
            even if that script is initialized before any Objects are created.

            The Objects created below are meant to be an example that a script is not required to create an Object. It will, however, be necessary if one wishes to update the Object's state during the application's runtime.
        */
        Object cube("sphere");
        cube.addComponent<Transform>(Transform(Vector3(0, -0.5f, 0), 2));
        cube.addComponent<Model>(Model(color::SOFTPEACH, shiny, mesh::get("sphere"), texture::get("default")));

        Object floor("floor");
        floor.addComponent<Transform>(Transform(Vector3(0, -2, -30), 20, Quaternion(math::radians(90), vec3::left)));
        floor.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("square"), texture::get("floor.png")));
        floor.addComponent<MeshAddon>( // MeshAddon attaches copies of the original mesh with different Transforms so that they are rendered in the same draw call. If this component is added to an Object, changes to this Object's Transform will likely cause unwanted behavior.
        MeshAddon
        ({
            Vector3(0, 0, 20), Vector3(0, 0, 40), Vector3(0, 0, 60), Vector3(20, 0, 0), Vector3(20, 0, 20), Vector3(20, 0, 40), Vector3(20, 0, 60), Vector3(-20, 0, 0), Vector3(-20, 0, 20), Vector3(-20, 0, 40), Vector3(-20, 0, 60)
        }));

        beginEventLoop();
    }
    else
        return 1;

    return 0;
}