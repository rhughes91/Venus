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

    void initialize(const DirectionalLight& dirLight__, const Shader& screenShader__, uint32_t width, uint32_t height);
    void remove();
    void refreshResolution(uint32_t width, uint32_t height);

    void store();
    void draw();
    void clear(const Color& color);

    int getMaximumSamples();
};

// Window (struct): determines the appearance of the application Window
class Window
{
    public:
        Screen screen;
        Vector2I cursorPosition;
        uint16_t width = 0;
        uint16_t height = 0;

        void *data;
        bool active, vsyncEnabled;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);
        
    private:
        void configureGLAD();
};

namespace window
{
    bool active();
    bool closing();
    bool decorated();
    bool maximized();
    bool minimized();
    bool resolutionUpdated();
    bool throwError();

    void centerWindow();
    void clearScreen(const Color& color);
    void close();
    void enableDecoration(bool enable);
    void enableVSync(bool enable);
    void fullscreen(bool enable);
    void hideCursor(bool enable);
    void lockCursor(bool enable);
    void maximize();
    void minimize();
    void refresh();
    void remove();
    void setCamera(uint32_t newCamera);
    void setCursor(const Vector2& position);
    void setDefaultBackgroundColor(const Color &color);
    void setIcon(const char *path);
    void setOpacity(float opacity);
    void setPosition(const Vector2I& position);
    void setTitle(const char *title);
    void storeRender();
    void drawRender();
    void terminate();

    float aspectRatioInv();
    float aspectRatio();
    float getOpacity();
    float height();
    float width();

    Vector2 cursorUniformScreenPosition();
    Vector2 cursorScreenPosition();
    Vector2I center();
    Vector2I monitorCenter();
    Vector2I resolution();

    uint32_t camera();
    DirectionalLight& lighting();
}

// initializes the global 'g_window' Window singleton
bool createWindow(const char * name, uint32_t width, uint32_t height);

// begins the primary game loop
void beginEventLoop();

// prints the currect OpenGL error buffer
int32_t throwGLError(const std::string& error);

#endif