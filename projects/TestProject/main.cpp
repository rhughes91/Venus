#include "project.h"

int main()
{
    /* 
        This function was created in a separate '.cpp' file and defined in a seperate header file. When new '.cpp' files are added, their file name must be added to the CMake file in this directory;
        Insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. The 'main.cpp' file should not be added to that list.
    */
    project::initialize();
    
    /* Optional window settings can be set using the 'window' namespace. */
    window::lockCursor(true);
    window::setSize(400, 400);

    // window::enableVSync(true);
    window::setLighting(DirectionalLight(vec3::down, color::WHITE, 0.15f));

    texture::load("block", texture::PNG);
    texture::load("crate", texture::PNG);
    texture::load("floor", texture::PNG);
    mesh::load("sphere", shape::sphere(1, 5));

    /*
        Entities can be initialized at any point before the event loop begins.
        Scripts can be defined and initialized at any point before the event loop begins.

        The Entities created below are meant to be an example that a script is not required to create an Object. However, a script will be necessary if the Object needs to be updated during runtime.
    */
    entity sphere = object::createEntity();
    object::addComponent<Transform>(sphere, Transform(Vector3(0, -0.5f, 0), 2));
    object::addComponent<Model>(sphere, Model(texture::get("default"), mesh::get("sphere")));
    object::addComponent<AdvancedShader>(sphere, {color::SOFTPEACH, 0.15f, 0.6f, 0.25f, 256});

    entity floor = object::createEntity();
    object::addComponent<Transform>(floor, Transform(Vector3(0, -2, -30), 20, Quaternion(math::radians(90), vec3::left)));
    object::addComponent<Model>(floor, Model(texture::get("floor.png"), mesh::get("square")));
    object::addComponent<AdvancedShader>(floor, {color::WHITE, 0.2f, 0.5f, 0.1f, 64});
    object::addComponent<MeshAddon>(floor, // MeshAddon attaches copies of the original mesh with different Transforms so that they are rendered in the same draw call. If this component is added to an Object, changes to this Object's Transform will likely cause unwanted behavior.
    MeshAddon
    ({
        {{Vector3(0, 0, 20)}}, {{Vector3(0, 0, 40)}}, {{Vector3(0, 0, 60)}}, {{Vector3(20, 0, 0)}}, {{Vector3(20, 0, 20)}}, {{Vector3(20, 0, 40)}}, {{Vector3(20, 0, 60)}}, {{Vector3(-20, 0, 0)}}, {{Vector3(-20, 0, 20)}}, {{Vector3(-20, 0, 40)}}, {{Vector3(-20, 0, 60)}}
    }));

    beginEventLoop();
}