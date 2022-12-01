#include <iostream>
#include <windows.h>

#include "setup.h"

extern std::string g_source;
extern Time g_time;
extern Window g_window;
extern InputManager g_keyboard, g_mouse;

void Screen::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Screen::initialize(const DirectionalLight& dirLight__, const Shader& screenShader__)
{
    dirLight = dirLight__;
    gamma = 1.5f;

    frameBuffer.initialize();
    subBuffer.initialize();
    depthBuffer.initialize();

    frameBuffer.addTexture("multiTexture", g_window.SCR_WIDTH, g_window.SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, 0, GL_REPEAT, true);
    subBuffer.addTexture("texture", g_window.SCR_WIDTH, g_window.SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_REPEAT, false);

    frameBuffer.addRenderBuffer("renderBuffer");

    shader = screenShader__;
    shader.use();
    shader.setInt("screenTexture", 0);
}
void Screen::remove()
{
    frameBuffer.remove();
    subBuffer.remove();
    depthBuffer.remove();
    shader.remove();
}
void Screen::refreshResolution()
{
    frameBuffer.refresh(g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    subBuffer.refresh(g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    resolutionUpdated = true;
}
void Screen::store()
{
    glViewport(0, 0, g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    frameBuffer.bind(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);
}
void Screen::draw()
{
    frameBuffer.bind(GL_READ_FRAMEBUFFER);
    subBuffer.bind(GL_DRAW_FRAMEBUFFER);
    glBlitFramebuffer(0, 0, g_window.SCR_WIDTH, g_window.SCR_HEIGHT, 0, 0, g_window.SCR_WIDTH, g_window.SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setFloat("gamma", gamma);
    quad.draw(subBuffer.getTexture("texture").data);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    g_keyboard.parse(GLFW_KEY_SPACE, glfwGetKey(window, GLFW_KEY_SPACE));
    for(GLenum k = GLFW_KEY_0; k <= GLFW_KEY_9; k++)
    {
        g_keyboard.parse(k, glfwGetKey(window, k));
    }
    for(GLenum k = GLFW_KEY_A; k <= GLFW_KEY_Z; k++)
    {
        g_keyboard.parse(k, glfwGetKey(window, k));
    }
    for(GLenum k = GLFW_KEY_ESCAPE; k <= GLFW_KEY_PAGE_DOWN; k++)
    {
        g_keyboard.parse(k, glfwGetKey(window, k));
    }
    for(GLenum k = GLFW_KEY_LEFT_SHIFT; k <= GLFW_KEY_LAST; k++)
    {
        g_keyboard.parse(k, glfwGetKey(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_keyboard.parse(key, false);
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        if(!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
        {
            glfwMaximizeWindow(window);
        }
        else
        {
            glfwRestoreWindow(window);
        }
    }
    else if(key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        if(g_window.screen.fullscreen = !g_window.screen.fullscreen)
        {
            g_window.screen.m_lastResolution = Vector2(g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
            glfwGetWindowPos(window, &g_window.screen.lastPosition.x, &g_window.screen.lastPosition.y);

            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        }
        else
        {
            glfwSetWindowMonitor(window, nullptr, 0, 0, g_window.screen.m_lastResolution.x, g_window.screen.m_lastResolution.y, GLFW_DONT_CARE);
            glfwSetWindowPos(window, g_window.screen.lastPosition.x, g_window.screen.lastPosition.y);
        }
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    for(GLenum k = GLFW_MOUSE_BUTTON_1; k <= GLFW_MOUSE_BUTTON_LAST; k++)
    {
        g_mouse.parse(k, glfwGetMouseButton(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_mouse.parse(button, false);
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    g_window.mousePosition = {(float)xpos, (float)ypos};
}
void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height)
{
    if(width && height)
    {
        g_window.SCR_WIDTH = width;
        g_window.SCR_HEIGHT = height;
        g_window.screen.refreshResolution();
    }

    glViewport(0, 0, width, height);
}

Window::Window(std::string name, uint32_t width, uint32_t height)
{
    if(g_window.data != NULL)
        return;

    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    data = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.c_str(), NULL, NULL);
    if(data == NULL)
    {
        std::cout << "ERROR :: Failed to create GLFW window" << std::endl;
        return;
    }

    glfwMakeContextCurrent(data);
    glfwSetWindowPos(data, 0, 25);
    
    glfwSetFramebufferSizeCallback(data, framebuffer_size_callback);
    glfwSetKeyCallback(data, key_callback);
    glfwSetCursorPosCallback(data, mouse_callback);
    glfwSetMouseButtonCallback(data, mouse_button_callback);

    configureGLAD();
    
    double xPos, yPos;
    glfwGetCursorPos(data, &xPos, &yPos);
    mousePosition = {(float)xPos, (float)yPos};
    g_window = *this;
    g_window.screen.initialize(DirectionalLight{Vector3(0, 0, 1), color::WHITE, 1}, Shader("screen_vertex", "screen_frag"));
}

bool Window::throwError()
{
    uint32_t error = glGetError();
    std::string errorMessage;
    switch(error)
    {
        case 0:
            errorMessage = "SUCCESS";
        break;
        case 1280:
            errorMessage = "ERROR :: Invalid enumeration parameter.";
        break;
        case 1281:
            errorMessage = "ERROR :: Invalid value parameter.";
        break;
        case 1282:
            errorMessage = "ERROR :: Invalid command state for given parameter.";
        break;
        case 1283:
            errorMessage = "ERROR :: Stack overflow.";
        break;
        case 1284:
            errorMessage = "ERROR :: Stack underflow.";
        break;
        case 1285:
            errorMessage = "ERROR :: Memory could not be allocated.";
        break;
        case 1286:
            errorMessage = "ERROR :: Framebuffer incomplete.";
        break;
    }
    std::cout << errorMessage << std::endl;
    return error;
}

void Window::configureGLAD()
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR :: Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        data = NULL;
        return;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glViewport(0, 0, g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    active = true;
}


bool initGL(const char *name, uint32_t width, uint32_t height)
{
    Window(name, width, height);
    if(!g_window.active)
        return false;

    stbi_set_flip_vertically_on_load(true);
    texture::set("", {"default"}, "png", GL_SRGB_ALPHA);
    mesh::set("square", shape::square());
    mesh::set("cube", shape::cube());

    g_keyboard.initialize(GLFW_KEY_LAST);
    g_mouse.initialize(GLFW_MOUSE_BUTTON_LAST);

    return true;
}