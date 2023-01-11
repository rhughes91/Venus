#include "project.h"


int main()
{
    if(createWindow("Test Project", 800, 600))
    {
        /* Optional window settings can be set using the 'window' namespace. */
        window::lockCursor(true);
        window::enableVSync(true);
        window::setLighting(DirectionalLight(vec3::down, color::WHITE));

        texture::load("block.png", texture::PNG);
        texture::load("crate.png", texture::PNG);
        texture::load("floor.png", texture::PNG);

        Material defaultMaterial = Material(shader::get("obj_shader"),
        [] 
        (Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
        {
            shader::advanced(entity, model, camera, cameraTransform, Vector3(0.1f, 0.5f, 0.1f), 64);
        });

        /* Objects can be initialized at any point before 'getEventLoop()' is run, although it may be cleaner to initialize them in the 'load' function of a script. */
        Object cube("cube");
        cube.addComponent<Transform>(Transform(Vector3(0, 0, 0), 1, Quaternion(math::radians(45), Vector3(-1, 1, 0))));
        cube.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("cube"), texture::get("crate.png")));

        Object floor("floor");
        floor.addComponent<Transform>(Transform(Vector3(0, -2, -30), 20, Quaternion(math::radians(90), vec3::left)));
        floor.addComponent<Model>(Model(color::WHITE, defaultMaterial, mesh::get("square"), texture::get("floor.png")));
        floor.addComponent<MeshAddon>( // MeshAddon attaches copies of the original mesh with different Transforms so that they are rendered in the same draw call.
        MeshAddon
        ({
            Vector3(0, 0, 20), Vector3(0, 0, 40), Vector3(0, 0, 60), Vector3(20, 0, 0), Vector3(20, 0, 20), Vector3(20, 0, 40), Vector3(20, 0, 60), Vector3(-20, 0, 0), Vector3(-20, 0, 20), Vector3(-20, 0, 40), Vector3(-20, 0, 60)
        }));

        /* 
            This function was created in a separate '.cpp' file and defined in a seperate header file. When new '.cpp' files are added, their file name must be added to the CMake file in this directory;
            Insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. The 'main.cpp' file should not be added to that list.
        */
        project::initialize();

        beginEventLoop();
    }
    else
        return 1;

    return 0;
}