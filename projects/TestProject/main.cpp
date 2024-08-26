#include "project.h"

int main()
{
    Application app = Application("Example", 400, 400);
    Texture::load("block", Texture::PNG);
    Texture::load("crate", Texture::PNG);
    Texture::load("floor", Texture::PNG);
    Mesh::load("sphere", shape::sphere(1, 5));

    Window& window = app.window();
    window.lockCursor(true);
    window.enableVSync(true);
    window.screen.dirLight = DirectionalLight(vec3::down, color::WHITE, 0.1f);

    /* 
        This function was created in a separate '.cpp' file and defined in a seperate header file. When new '.cpp' files are added, their file name must be added to the CMake file in this directory;
        Insert 'file_name.cpp' in the empty string at the 'set(PROJECT_SRC, "")' command at the top of the CMake file. The 'main.cpp' file should not be added to that list.
    */
    project::initialize(app);

    Application::beginEventLoop(app);
}