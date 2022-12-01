#include <iostream>
#include <math.h>

#include "setup.h"
#include "file_util.h"

extern Window g_window;
extern Time g_time;
extern InputManager g_keyboard, g_mouse;

void beginEventLoop()
{
    Shader& objectShader = shader::set("obj_shader", Shader("object_vertex", "object_frag"));
    objectShader.use();
    objectShader.setInt("material.diffuse", 0);
    objectShader.setInt("material.specular", 1);
    objectShader.setVec3("dirLight.direction", g_window.screen.dirLight.direction);
    objectShader.setVec4("dirLight.color", g_window.screen.dirLight.color);
    objectShader.setFloat("dirLight.strength", g_window.screen.dirLight.strength);

    Shader& uiShader = shader::set("ui_shader", Shader("ui_vertex", "ui_frag"));
    uiShader.use();
    uiShader.setInt("text", 0);

    g_window.screen.quad = mesh::get("square");

    float timeScale = 0.02f;
    object::load();
    object::start();
    g_time.beginTimer(glfwGetTime());
    while (!glfwWindowShouldClose(g_window.data))
    {
        g_time.update(glfwGetTime());

        if(g_keyboard.inputs[GLFW_KEY_ESCAPE].pressed)
            glfwSetWindowShouldClose(g_window.data, true);

        while(g_time.timer > timeScale)
        {
            object::fixedUpdate();
            g_time.resetTimer(timeScale);
        }

        object::update();
        object::lateUpdate();
        object::render();

        g_keyboard.refresh();
        g_mouse.refresh();
        
        glfwSwapBuffers(g_window.data);
        glfwPollEvents();
    }

    g_window.remove();
    mesh::remove();
    shader::remove();
    texture::remove();

    std::cout << g_time.averageFrameRate << " FPS : " << g_time.deltaTime*1000 << " ms" << std::endl;
    g_window.throwError();
    glfwTerminate();
}