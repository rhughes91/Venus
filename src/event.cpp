#include "input.h"
#include "setup.h"
#include "structure.h"
#include "component.h"

#include <iostream>
#include <thread>

extern Time g_time;
extern InputManager g_keyboard, g_mouse;

extern uint32_t currentWindow;
extern std::vector<Window> g_windows;

void beginEventLoop()
{
    if(!window::active())
    {
        std::cout << "ERROR :: Window has not been initialized" << std::endl;
        return;
    }
    
    object::run(object::fn::LOAD);
    object::run(object::fn::START);
    
    g_time.beginTimer();
    while(!window::closing())
    {        
        if(g_keyboard.inputs[key::ESCAPE].pressed)
            window::close();
        
        g_time.update();
        while(g_time.timer > 0.02f)
        {
            object::run(object::fn::FIXED_UPDATE);
            g_time.resetTimer(0.02f);
        }
        
    if((!g_time.frozen || key::pressed(g_time.advanceKey)))
    {
        object::run(object::fn::UPDATE);
        object::run(object::fn::LATE_UPDATE);
    }

    g_windows[currentWindow].screen.store();

    window::clearScreen(object::getComponent<Camera>(window::camera()).backgroundColor);
    object::run(object::fn::RENDER);
    g_windows[currentWindow].screen.draw();

    bool fullscreen = window::fullscreened();
    bool maximized = window::maximized();
    Vector2 resolution = window::resolution();
        
        g_keyboard.refresh();
        g_mouse.refresh();

        window::refresh();
        window::poll();
        
        window::updatedResolution(fullscreen != window::fullscreened() || maximized != window::maximized() || resolution != window::resolution());
    }

    object::run(object::fn::DESTROY);
    
    window::remove();
    mesh::remove();
    shader::remove();
    texture::remove();

    std::cout << event::framerate() << " FPS : " << g_time.deltaTime*1000 << " ms" << std::endl;
    window::throwError();
    window::throwAudioError();
    window::terminate();
}