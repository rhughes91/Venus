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
    Color defaultColor;
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
        Vector2I cursorPosition;
        uint16_t SCR_WIDTH = 0;
        uint16_t SCR_HEIGHT = 0;

        void *data;
        bool active, vsyncEnabled;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);

        bool closing();
        bool decorated();
        bool throwError();

        void centerWindow()
        {
            setPosition(monitorCenter() - Vector2I(0.5*SCR_WIDTH, 0.5*SCR_HEIGHT));
        }
        void close();
        void enableDecoration(bool enable);
        void enableVSync(bool enable);
        void hideCursor(bool enable);
        void lockCursor(bool enable);
        void maximize();
        void refresh();
        void remove()
        {
            screen.remove();
        }
        void setCursor(const Vector2& position);
        void setDefaultBackgroundColor(const Color &color);
        void setIcon(const char *path);
        void setOpacity(float opacity);
        void setPosition(const Vector2I& position);
        void setTitle(const char *title);
        void terminate();

        float aspectRatioInv()
        {
            return (float)SCR_WIDTH/(float)SCR_HEIGHT;
        }
        float aspectRatio()
        {
            return (float)SCR_HEIGHT/(float)SCR_WIDTH;
        }
        float getOpacity();

        Vector2 cursorUniformScreenPosition()
        {
            return {((float)cursorPosition.x/(float)SCR_WIDTH)*2 - 1, -((float)cursorPosition.y/(float)SCR_HEIGHT)*2 + 1};
        }
        Vector2 cursorScreenPosition()
        {
            return cursorUniformScreenPosition() * Vector2{aspectRatioInv(), 1};
        }
        Vector2I center()
        {
            return Vector2I(monitorCenter() - Vector2I(0.5*SCR_WIDTH, 0.5*SCR_HEIGHT));
        }
        Vector2I monitorCenter();
        Vector2I resolution()
        {
            return Vector2I(SCR_WIDTH, SCR_HEIGHT);
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