#include "project.h"
#include <algorithm>

void test()
{
    int length = 20;
    int n = 100;
    std::vector<Vector2> points = std::vector<Vector2>();
    for(int i=0; i<length; i++)
    {
        points.push_back(Vector2(rand()%n, rand()%n));
    }

    Vector2 average = 0;
    for(int i=0; i<length; i++)
    {
        average += points[i];
    }
    average /= length;

    for(int i=0; i<length; i++)
    {
        points[i] -= average;
    }

    std::sort(points.begin(), points.end(), [](const Vector2& one, const Vector2& two)
    {
        float angleOne = vec3::angle(Vector3(one, 0), vec3::up), angleTwo = vec3::angle(Vector3(two, 0), vec3::up);
        return angleOne * math::sign(one.x) > angleTwo * math::sign(two.x);
    });

    for(int i=0; i<length; i++)
    {
        std::cout << (i ? ", ":"") << points[i];
        // std::cout << points[i].dot(vec3::up) << " : " << points[i] << '\n';
    }
    std::cout << '\n';
}

int main()
{
    // test();

    /* 
        This function was created in a separate '.cpp' file and defined in a seperate header file. When new '.cpp' files are added, their file name must be added to the CMake file in this directory;
        Insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. The 'main.cpp' file should not be added to that list.
    */
    project::initialize();
    
    /* Optional window settings can be set using the 'window' namespace. */
    window::lockCursor(true);
    window::setSize(400, 400);

    window::enableVSync(true);
    window::setLighting(DirectionalLight(vec3::down, color::WHITE, 0.15f));

    // audio::load("breeze.wav");

    texture::load("block", texture::PNG);
    texture::load("crate", texture::PNG);
    texture::load("floor", texture::PNG);
    Mesh::load("sphere", shape::sphere(1, 5));

    /*
        Entities can be initialized at any point before the event loop begins.
        Scripts can be defined and initialized at any point before the event loop begins.

        The entities created below are meant to be an example that a script is not required to create an Object. However, a script will be necessary if the Object needs to be updated during runtime.
    */
    entity sphere = object::createEntity();
    object::addComponent<Transform>(sphere, Transform(Vector3(0, -0.5f, 0), 2));
    object::addComponent<Model>(sphere, Model(texture::get("default"), Mesh::get("sphere")));
    object::addComponent<AdvancedShader>(sphere, {color::SOFTPEACH, 0.15f, 0.6f, 0.25f, 256});
    object::addComponent<Audio>(sphere, Audio(audio::get("breeze.wav")));

    beginEventLoop();
}