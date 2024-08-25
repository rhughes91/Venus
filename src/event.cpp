#include "input.h"
#include "setup.h"
#include "structure.h"
#include "component.h"

#include <iostream>
#include <thread>

void Application::beginEventLoop(Application& app)
{    
    Window& win = app.window();
    app.time.beginTimer();
    app.setScene();
    app.lastScene = -1;
    app.updateScene();

    Application::updateCursor();

    while(!win.closing())
    {        
        object::ecs& ecs = app.getScene().container;

        Window& win = app.window();
        if(app.keyboard.inputs[key::ESCAPE].pressed)
            win.close();

        if(win.screen.resolutionUpdated)
        {
            win.screen.refreshResolution(win.resolution());
        }

        entity camera = win.screen.camera;
        bool fovChanged = false;
        float fov = 0;
        if(camera != -1)
        {
            fov = ecs.getComponent<Camera>(camera).fov;
        }

        
        app.time.update();
        while(app.time.timer > 0.02f)
        {
            ecs.run(object::fn::FIXED_UPDATE, &app);
            app.time.resetTimer(0.02f);
        }
        
        ecs.run(object::fn::UPDATE, &app);
        ecs.run(object::fn::LATE_UPDATE, &app);

        win.screen.store();
        if(camera != -1)
        {
            Camera& cam = ecs.getComponent<Camera>(camera);
            fovChanged = (fov != cam.fov);
            win.screen.clear(cam.backgroundColor);
        }
        else
        {
            win.screen.clear(color::MAGENTA);
        }
        
        ecs.run(object::fn::RENDER, &app);
        win.screen.draw();

        bool fullscreen = win.fullscreened();
        bool maximized = win.maximized();
        
        app.keyboard.refresh();
        app.mouse.refresh();
        for(JoystickManager& controller : app.controllers)
        {
            controller.refresh();
            controller.joystick_button_callback();
        }

        Application::updateCursor();
        win.refresh();
        win.poll();

        if(key::pressed(key::F11))
        {
            win.isFullscreen = !win.isFullscreen;
            win.fullscreen(win.isFullscreen, win.vsyncEnabled);
        }
        
        win.screen.resolutionUpdated = (fullscreen != win.fullscreened() || maximized != win.maximized() || win.screen.resolution != win.resolution() || fovChanged);
        
        app.updateScene();
    }

    object::ecs& ecs = app.getScene().container;
    ecs.run(object::fn::DESTROY, &app);
    
    Mesh::clear();
    Shader::clear();
    Texture::clear();
    // Audio::clear();

    std::cout << app.time.framerate() << " FPS : " << app.time.deltaTime*1000 << " ms\n";
    win.throwError();
    // win.throwAudioError();
    std::cout << ecs.parseError() << " (ECS)" << std::endl;
    // win.terminate(win.audioDevice, win.audioContext);
}
