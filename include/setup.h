#ifndef SETUP_H
#define SETUP_H

#include "image/stb_image.h"
#include "glad/glad.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include <string>

#include "component.h"

// Screen (struct): holds basic data about how data is rendered to the screen
struct Screen
{
    bool fullscreen = false;

    mat4x4 lightSpaceMatrix;

    Entity camera;

    bool resolutionUpdated;
    Vector2 m_lastResolution;
    Vector2I lastPosition;

    Mesh quad;
    FrameBuffer frameBuffer, subBuffer, depthBuffer;

    DirectionalLight dirLight;

    Shader shader;
    float gamma;

    void initialize(const DirectionalLight& dirLight__, const Shader& screenShader__);
    void remove();
    void refreshResolution();

    void store();
    void draw();
    void clear(const Color& color);
};

// Window (struct): determines the appearance of the application Window
class Window
{
    public:
        Screen screen;
        Vector2I mousePosition;
        uint16_t SCR_WIDTH = 0;
        uint16_t SCR_HEIGHT = 0;

        GLFWwindow *data;
        bool active;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);
        bool throwError();
        void enableVSync(bool enable)
        {
            glfwSwapInterval(enable);
        }
        void hideMouse(bool enable)
        {
            glfwSetInputMode(data, GLFW_CURSOR, enable ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
        }
        void lockMouse(bool enable)
        {
            glfwSetInputMode(data, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        void remove()
        {
            screen.remove();
        }

        float aspectRatioInv()
        {
            return (float)SCR_WIDTH/(float)SCR_HEIGHT;
        }
        float aspectRatio()
        {
            return (float)SCR_HEIGHT/(float)SCR_WIDTH;
        }
    
        Vector2 mouseUniformScreenPosition()
        {
            return {((float)mousePosition.x/(float)SCR_WIDTH)*2 - 1, -((float)mousePosition.y/(float)SCR_HEIGHT)*2 + 1};
        }
        Vector2 mouseScreenPosition()
        {
            return mouseUniformScreenPosition() * Vector2{aspectRatioInv(), 1};
        }
    private:
        void configureGLAD();
};

// initializes the global 'g_window' Window singleton
bool initGL(const char * name, uint32_t width, uint32_t height);

// begins the primary game loop
void beginEventLoop();

// prints the currect OpenGL error buffer
int32_t throwGLError(const std::string& error);

#endif