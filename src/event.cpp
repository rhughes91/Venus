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
    
    DirectionalLight light = window::lighting();

    Shader& objectShader = shader::load("obj_shader", Shader("object_vertex", "object_frag"));
    objectShader.use();
    objectShader.setInt("material.diffuse", 0);
    // objectShader.setInt("material.specular", 1);
    objectShader.setVec3("dirLight.direction", light.direction);
    objectShader.setVec4("dirLight.color", light.color);
    objectShader.setFloat("dirLight.strength", light.strength);
    
    Shader& simpleShader = shader::load("simple_shader", Shader("object_vertex", "simple_frag"));
    simpleShader.use();
    simpleShader.setInt("material.texture", 0);
    
    Shader& uiShader = shader::load("ui_shader", Shader("ui_vertex", "ui_frag"));
    uiShader.use();
    uiShader.setInt("text", 0);

    shader::load("spline_shader", Shader("spline_vertex", "spline_frag"));
    
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
    
    window::remove();
    mesh::remove();
    shader::remove();
    texture::remove();

    std::cout << g_time.averageFrameRate << " FPS : " << g_time.deltaTime*1000 << " ms" << std::endl;
    window::throwError();
    window::terminate();
}