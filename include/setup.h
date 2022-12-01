#ifndef SETUP_H
#define SETUP_H

#include "graphics.h"
#include "shader.h"

// DirectionalLight (struct): holds data needed to render a directional light
struct DirectionalLight
{
    Vector3 direction;
    Color color;
    float strength;
};

// Screen (struct): holds basic data about how data is rendered to the screen
struct Screen
{
    bool fullscreen = false;
    uint32_t camera;

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

        void *data;
        bool active;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);

        bool closing();
        bool throwError();

        void close();
        void enableVSync(bool enable);
        void hideMouse(bool enable);
        void lockMouse(bool enable);
        void refresh();
        void remove()
        {
            screen.remove();
        }
        void terminate();

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
bool createWindow(const char * name, uint32_t width, uint32_t height);

// begins the primary game loop
void beginEventLoop();

// prints the currect OpenGL error buffer
int32_t throwGLError(const std::string& error);

#endif