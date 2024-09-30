#include "input.h"
#include "application.h"
#include "structure.h"
#include "component.h"

#include <iostream>
#include <thread>

void Application::runEventLoop(Application& app)
{    
    Window& window = app.window();
    app.time.beginTimer();
    app.setScene();
    app.lastScene = -1;
    app.updateScene();

    Application::updateCursor();

    while(!window.closing())
    {        
        object::ecs& ecs = app.getScene().container;

        Window& window = app.window();
        if(key::pressed(key::ESCAPE))
            window.close();

        window.updateResolution();

        entity camera = window.screen.camera;
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

        window.screen.store();
        if(camera != -1)
        {
            Camera& cam = ecs.getComponent<Camera>(camera);
            fovChanged = (fov != cam.fov);
            window.screen.clear(cam.backgroundColor);
        }
        else
        {
            fovChanged = false;
            window.screen.clear(color::MAGENTA);
        }
        
        ecs.run(object::fn::RENDER, &app);
        window.screen.draw();

        bool fullscreen = window.fullscreened();
        bool maximized = window.maximized();
        
        app.keyboard.refresh();
        app.mouse.refresh();
        for(JoystickManager& controller : app.controllers)
        {
            controller.refresh();
            controller.joystick_button_callback();
        }

        Application::updateCursor();
        window.refresh();
        window.poll();

        if(key::pressed(key::F11))
        {
            window.isFullscreen = !window.isFullscreen;
            window.fullscreen(window.isFullscreen, window.vsyncEnabled);
        }
        
        window.screen.resolutionUpdated = (fullscreen != window.fullscreened() || maximized != window.maximized() || window.screen.resolution != window.resolution() || fovChanged);
        
        app.updateScene();
    }

    object::ecs& ecs = app.getScene().container;
    ecs.run(object::fn::DESTROY, &app);
    
    Mesh::clear();
    Shader::clear();
    Texture::clear();
    // Audio::clear();

    std::cout << app.time.framerate() << " FPS : " << app.time.deltaTime*1000 << " ms\n";
    window.throwError();
    // win.throwAudioError();
    std::cout << ecs.parseError() << " (ECS)" << std::endl;
    // win.terminate(win.audioDevice, win.audioContext);
}
