#include "glad/glad.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include "image/stb_image.h"

#include <algorithm>
#include <windows.h>

#include "setup.h"
#include "input.h"
#include "structure.h"

Window g_window;

extern std::string g_source;
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
    subBuffer.addTexture("texture", g_window.SCR_WIDTH, g_window.SCR_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_REPEAT, false);
    
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
    frameBuffer.refresh(g_window.SCR_WIDTH, g_window.SCR_HEIGHT, defaultColor.a == 1);
    subBuffer.refresh(g_window.SCR_WIDTH, g_window.SCR_HEIGHT, defaultColor.a == 1);
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
    glClearColor(defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    shader.use();
    shader.setFloat("gamma", gamma);
    quad.draw(subBuffer.getTexture("texture").data);
}

void object::render()
{
    g_window.screen.store();
    g_manager.render();
    g_window.screen.draw();
}
void FrameBuffer::addRenderBuffer(const std::string& name)
{
    uint32_t renderBuffer;
    bind(GL_FRAMEBUFFER);

    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, g_window.SCR_WIDTH, g_window.SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    renderBuffers.insert({name, renderBuffer});
    unbind();
}
void Mesh::draw(const uint32_t texture) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_window.screen.depthBuffer.getTexture("texture").data);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, count);
}

uint32_t glInputToKeyCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_KEY_SPACE: return key::SPACE;
        case GLFW_KEY_APOSTROPHE: return key::APOSTROPHE;
        case GLFW_KEY_COMMA: return key::COMMA;
        case GLFW_KEY_MINUS : return key::MINUS;
        case GLFW_KEY_PERIOD: return key::PERIOD;
        case GLFW_KEY_SLASH: return key::SLASH;
        case GLFW_KEY_0: return key::ZERO;
        case GLFW_KEY_1: return key::ONE;
        case GLFW_KEY_2: return key::TWO;
        case GLFW_KEY_3: return key::THREE;
        case GLFW_KEY_4: return key::FOUR;
        case GLFW_KEY_5: return key::FIVE;
        case GLFW_KEY_6: return key::SIX;
        case GLFW_KEY_7: return key::SEVEN;
        case GLFW_KEY_8: return key::EIGHT;
        case GLFW_KEY_9: return key::NINE;
        case GLFW_KEY_SEMICOLON: return key::SEMICOLON;
        case GLFW_KEY_EQUAL: return key::EQUAL;
        case GLFW_KEY_A: return key::A;
        case GLFW_KEY_B: return key::B;
        case GLFW_KEY_C: return key::C;
        case GLFW_KEY_D: return key::D;
        case GLFW_KEY_E: return key::E;
        case GLFW_KEY_F: return key::F;
        case GLFW_KEY_G: return key::G;
        case GLFW_KEY_H: return key::H;
        case GLFW_KEY_I: return key::I;
        case GLFW_KEY_J: return key::J;
        case GLFW_KEY_K: return key::K;
        case GLFW_KEY_L: return key::L;
        case GLFW_KEY_M: return key::M;
        case GLFW_KEY_N: return key::N;
        case GLFW_KEY_O: return key::O;
        case GLFW_KEY_P: return key::P;
        case GLFW_KEY_Q: return key::Q;
        case GLFW_KEY_R: return key::R;
        case GLFW_KEY_S: return key::S;
        case GLFW_KEY_T: return key::T;
        case GLFW_KEY_U: return key::U;
        case GLFW_KEY_V: return key::V;
        case GLFW_KEY_W: return key::W;
        case GLFW_KEY_X: return key::X;
        case GLFW_KEY_Y: return key::Y;
        case GLFW_KEY_Z: return key::Z;
        case GLFW_KEY_LEFT_BRACKET: return key::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return key::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return key::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return key::GRAVE;
        case GLFW_KEY_ESCAPE: return key::ESCAPE;
        case GLFW_KEY_ENTER: return key::ENTER;
        case GLFW_KEY_TAB: return key::TAB;
        case GLFW_KEY_BACKSPACE: return key::BACKSPACE;
        case GLFW_KEY_INSERT: return key::INSERT;
        case GLFW_KEY_DELETE: return key::DEL;
        case GLFW_KEY_RIGHT: return key::RIGHT;
        case GLFW_KEY_LEFT: return key::LEFT;
        case GLFW_KEY_DOWN: return key::DOWN;
        case GLFW_KEY_UP: return key::UP;
        case GLFW_KEY_PAGE_UP: return key::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return key::PAGE_DOWN;
        case GLFW_KEY_HOME: return key::HOME;
        case GLFW_KEY_END: return key::END;
        case GLFW_KEY_CAPS_LOCK: return key::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return key::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return key::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return key::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return key::PAUSE;
        case GLFW_KEY_F1: return key::F1;
        case GLFW_KEY_F2: return key::F2;
        case GLFW_KEY_F3: return key::F3;
        case GLFW_KEY_F4: return key::F4;
        case GLFW_KEY_F5: return key::F5;
        case GLFW_KEY_F6: return key::F6;
        case GLFW_KEY_F7: return key::F7;
        case GLFW_KEY_F8: return key::F8;
        case GLFW_KEY_F9: return key::F9;
        case GLFW_KEY_F10: return key::F10;
        case GLFW_KEY_F11: return key::F11;
        case GLFW_KEY_F12: return key::F12;
        case GLFW_KEY_F13: return key::F13;
        case GLFW_KEY_F14: return key::F14;
        case GLFW_KEY_F15: return key::F15;
        case GLFW_KEY_F16: return key::F16;
        case GLFW_KEY_F17: return key::F17;
        case GLFW_KEY_F18: return key::F18;
        case GLFW_KEY_F19: return key::F19;
        case GLFW_KEY_F20: return key::F20;
        case GLFW_KEY_F21: return key::F21;
        case GLFW_KEY_F22: return key::F22;
        case GLFW_KEY_F23: return key::F23;
        case GLFW_KEY_F24: return key::F24;
        case GLFW_KEY_F25: return key::F25;
        case GLFW_KEY_KP_0: return key::K_0;
        case GLFW_KEY_KP_1: return key::K_1;
        case GLFW_KEY_KP_2: return key::K_2;
        case GLFW_KEY_KP_3: return key::K_3;
        case GLFW_KEY_KP_4: return key::K_4;
        case GLFW_KEY_KP_5: return key::K_5;
        case GLFW_KEY_KP_6: return key::K_6;
        case GLFW_KEY_KP_7: return key::K_7;
        case GLFW_KEY_KP_8: return key::K_8;
        case GLFW_KEY_KP_9: return key::K_9;
        case GLFW_KEY_KP_DECIMAL: return key::K_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return key::K_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return key::K_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return key::K_SUBTRACT;
        case GLFW_KEY_KP_ADD: return key::K_ADD;
        case GLFW_KEY_KP_ENTER: return key::K_ENTER;
        case GLFW_KEY_KP_EQUAL: return key::K_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return key::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return key::LEFT_CTRL;
        case GLFW_KEY_LEFT_ALT: return key::LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return key::LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return key::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return key::RIGHT_CTRL;
        case GLFW_KEY_RIGHT_ALT: return key::RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return key::RIGHT_SUPER;
        case GLFW_KEY_WORLD_1: return key::GLOBAL_0;
        case GLFW_KEY_WORLD_2: return key::GLOBAL_1;
        case GLFW_KEY_MENU: return key::MENU;
    }
    return key::NIL;
}
uint32_t glInputToButtonCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_MOUSE_BUTTON_1: return button::MOUSE_0;
        case GLFW_MOUSE_BUTTON_2: return button::MOUSE_1;
        case GLFW_MOUSE_BUTTON_3: return button::MOUSE_2;
        case GLFW_MOUSE_BUTTON_4: return button::MOUSE_3;
        case GLFW_MOUSE_BUTTON_5: return button::MOUSE_4;
        case GLFW_MOUSE_BUTTON_6: return button::MOUSE_5;
        case GLFW_MOUSE_BUTTON_7: return button::MOUSE_6;
        case GLFW_MOUSE_BUTTON_8: return button::MOUSE_7;
    }
    return button::NIL;
}

void Time::update()
{
    runtime = glfwGetTime();
    lastDeltaTime = deltaTime;
    deltaTime = deltaTime * 0.75f + (runtime - lastFrame) * 0.25f;
    lastFrame = runtime;
    timer += deltaTime;
    averageFrameRate = (averageFrameRate + (1/deltaTime))/2;
}
void Time::beginTimer()
{
    float currentFrame = glfwGetTime();
    lastFrame = currentFrame;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    for(GLenum k = 0; k<=GLFW_KEY_LAST; k++)
    {
        g_keyboard.parse(glInputToKeyCode(k), glfwGetKey(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_keyboard.parse(glInputToKeyCode(key), false);
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
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, g_window.vsyncEnabled ? mode->refreshRate : GLFW_DONT_CARE);

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

    for(GLenum k = 0; k <= GLFW_MOUSE_BUTTON_LAST; k++)
    {
        g_mouse.parse(glInputToButtonCode(k), glfwGetMouseButton(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_mouse.parse(glInputToButtonCode(button), false);
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    g_window.cursorPosition = {(float)xpos, (float)ypos};
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
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    data = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.c_str(), NULL, NULL);
    if(data == NULL)
    {
        std::cout << "ERROR :: Failed to create GLFW window" << std::endl;
        return;
    }

    glfwMakeContextCurrent((GLFWwindow *)data);
    glfwShowWindow((GLFWwindow *)data);
    glfwSetWindowPos((GLFWwindow *)data, 0, 25);
    
    glfwSetFramebufferSizeCallback((GLFWwindow *)data, framebuffer_size_callback);
    glfwSetKeyCallback((GLFWwindow *)data, key_callback);
    glfwSetCursorPosCallback((GLFWwindow *)data, mouse_callback);
    glfwSetMouseButtonCallback((GLFWwindow *)data, mouse_button_callback);

    configureGLAD();
    
    double xPos, yPos;
    glfwGetCursorPos((GLFWwindow *)data, &xPos, &yPos);
    cursorPosition = {(float)xPos, (float)yPos};
    g_window = *this;
    g_window.screen.initialize(DirectionalLight{Vector3(0, 0, 1), color::WHITE, 1}, Shader("screen_vertex", "screen_frag"));
    g_window.screen.defaultColor = color::RED;
}
bool Window::closing()
{
    return glfwWindowShouldClose((GLFWwindow *)data);
}
bool Window::decorated()
{
    return glfwGetWindowAttrib((GLFWwindow *)data, GLFW_DECORATED);
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
void Window::close()
{
    glfwSetWindowShouldClose((GLFWwindow *)data, true);
}
void Window::enableDecoration(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)data, GLFW_DECORATED, enable);
}
void Window::enableVSync(bool enable)
{
    glfwSwapInterval(vsyncEnabled = enable);
}
void Window::hideCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)data, GLFW_CURSOR, enable ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}
void Window::lockCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)data, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void Window::maximize()
{
    glfwMaximizeWindow((GLFWwindow *)data);
}
void Window::refresh()
{
    glfwSwapBuffers((GLFWwindow *)g_window.data);
    glfwPollEvents();
}
void Window::setCursor(const Vector2& position)
{
    glfwSetCursorPos((GLFWwindow *)data, position.x, position.y);
}
void Window::setDefaultBackgroundColor(const Color &color)
{
    screen.defaultColor = color;
    screen.refreshResolution();
}
void Window::setIcon(const char *path)
{
    GLFWimage images[1];

    stbi_set_flip_vertically_on_load(false);
    images[0].pixels = stbi_load((g_source + std::string("resources/images/") + path).c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon((GLFWwindow *)data, 1, images); 
    stbi_image_free(images[0].pixels);
    stbi_set_flip_vertically_on_load(true);
}
void Window::setOpacity(float opacity)
{
    glfwSetWindowOpacity((GLFWwindow *)data, opacity);
}
void Window::setPosition(const Vector2I& position)
{
    glfwSetWindowPos((GLFWwindow *)data, position.x, position.y);
}
void Window::setTitle(const char *title)
{
    glfwSetWindowTitle((GLFWwindow *)data, title);
}
void Window::terminate()
{
    glfwTerminate();
}
float Window::getOpacity()
{
    return glfwGetWindowOpacity((GLFWwindow *)data);
}
Vector2I Window::monitorCenter()
{
    Vector2I result;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &result.x, &result.y);
    return result * 2;
}

bool createWindow(const char *name, uint32_t width, uint32_t height)
{
    TCHAR buffer[260] = { 0 };
    GetModuleFileName(NULL, buffer, 256);

    std::string source = std::string(buffer);
    std::replace(source.begin(), source.end(), '\\', '/');
    g_source = source.substr(0, source.find_last_of("/")) + "/";

    Window(name, width, height);
    if(!g_window.active)
        return false;

    stbi_set_flip_vertically_on_load(true);
    texture::set("", {"default"}, texture::PNG);
    mesh::set("square", shape::square());
    mesh::set("cube", shape::cube());

    g_keyboard.initialize(key::LAST);
    g_mouse.initialize(button::MOUSE_LAST);

    return true;
}