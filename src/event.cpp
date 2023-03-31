#include "input.h"
#include "setup.h"
#include "structure.h"
#include "component.h"

extern Time g_time;
extern InputManager g_keyboard, g_mouse;

void beginEventLoop()
{
    if(!window::active())
    {
        std::cout << "ERROR :: Window has not been initialized" << std::endl;
        return;
    }
    
    float timeScale = 0.02f;
    object::load();
    object::start();
    g_time.beginTimer();
    while (!window::closing())
    {
        g_time.update();
        
        if(g_keyboard.inputs[key::ESCAPE].pressed)
            window::close();

        while(g_time.timer > timeScale)
        {
            object::fixedUpdate();
            g_time.resetTimer(timeScale);
        }

        if(!g_time.frozen || key::pressed(g_time.advanceKey))
        {
            object::update();
            object::lateUpdate();
        }
        object::render();

        g_keyboard.refresh();
        g_mouse.refresh();
        window::refresh();
    }

    object::destroy();
    
    window::remove();
    mesh::remove();
    shader::remove();
    texture::remove();

    std::cout << event::framerate() << " FPS : " << g_time.deltaTime*1000 << " ms" << std::endl;
    window::throwError();
    window::terminate();
}