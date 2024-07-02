#ifndef SETUP_H
#define SETUP_H

#include "audio.h"
#include "component.h"
#include "graphics.h"
#include "machine.h"
#include "structure.h"
#include "ui.h"


using AnimationState = object::state_machine<Animation2D>;
using Animator2D = object::state_machine<AnimationState>;

struct AnimationManager{};
struct AnimationManager2D{};
struct AudioManager{};
struct BillboardManager{};
struct ButtonManager{};
struct CameraManager{};
struct GraphicsManager{};
struct LineManager{};
struct MeshManager{};
struct Mesh2DManager{};
struct PhysicsManager{};
struct PointLightManager{};
struct SpotLightManager{};
struct SpriteManager{};
struct TextManager{};
struct UIManager{};

struct SimpleRenderer{};
struct AdvancedRenderer{};
struct ComplexRenderer
{
    bool update = true;
    int model, view, projection, scale, color, offset, uvScale, flip;
};
struct UIRenderer{};

struct AABB2DHandler{};
struct AABBHandler{};


struct MeshModule
{
    Transform transform;
    Vector2 uvOffset = 0, uvScale = 1;
    Color hue = color::WHITE;

    MeshModule() {}
    MeshModule(const Transform& transform__) : transform(transform__), uvOffset(0), uvScale(1) {}
    MeshModule(const Transform& transform__, const Vector2& uvOffset__, const Vector2& uvScale__ = 1): transform(transform__), uvOffset(uvOffset__), uvScale(uvScale__) {}
};

struct MeshAddon
{
    std::vector<MeshModule> additions;

    MeshAddon() {}
    MeshAddon(const std::vector<MeshModule>& additions__) : additions(additions__) {}

    void append(Model& model, const Transform& parentTransform);
};


// DirectionalLight (struct): holds data needed to render a directional light
struct DirectionalLight
{
    Vector3 direction;
    Color color;
    float strength;

    DirectionalLight() {}
    DirectionalLight(const Vector3& direction__, const Color& color__, float strength__ = 1) : direction(direction__), color(color__), strength(strength__) {}
};

// Screen (struct): holds basic data about how data is rendered to the screen
struct Screen
{
    bool fullscreen = false;
    Color defaultColor = Color(1, 0, 0, 1);
    uint32_t camera = -1;

    bool resolutionUpdated = true;
    Vector2 resolution;
    Vector2I lastPosition;

    Mesh quad;
    FrameBuffer frameBuffer, subBuffer, depthBuffer;

    DirectionalLight dirLight;
    Shader screenShader;
    float gamma;

    void initialize(const DirectionalLight& dirLight__, const Shader& screenShader__, uint32_t width, uint32_t height);
    void remove();
    void refreshResolution(const Vector2& res);

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
        Vector2I cursorPosition, lastPosition, lastResolution;
        uint16_t width = 0;
        uint16_t height = 0;

        void *data, *audioDevice, *audioContext;
        bool vsyncEnabled = false, isFullscreen = false;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);

        void setCamera(uint32_t cam)
        {
            screen.camera = cam;
        }

        bool closing();
        bool decorated();
        bool fullscreened();
        bool maximized();
        bool throwAudioError();
        bool throwError();

        void centerWindow();
        void close();
        void enableDecoration(bool enable);
        bool enableVSync(bool enable);
        void fullscreen(bool enable, bool vsync);
        void hideCursor(bool enable);
        void lockCursor(bool enable);
        void maximize();
        void minimize();
        void poll();
        void refresh();
        void setCursor(const Vector2& position);
        void setIcon(const char *path);
        void setOpacity(float opacity);
        void setPosition(const Vector2I& position);
        void setSize(const Vector2I& size);
        void setTitle(const char *title);
        void terminate(void *audioDevice, void *audioContext);

        float aspectRatioInv();
        float aspectRatio();
        float getOpacity();

        Vector2I position();
        Vector2 cursorUniformScreenPosition();
        Vector2 cursorScreenPosition();
        Vector2I center();
        Vector2I monitorCenter();
        Vector2I resolution();
        
    private:
        void configureGLAD();
};

//
struct ProjectManager
{
    ProjectManager() {}
    ProjectManager(std::vector<Window>& windows);
};


// Time (struct): holds all the timing data that happens between frames :: controls when "fixedUpdate" is run
struct Time
{
    bool frozen = false;
    int32_t advanceKey = '\\';

    double deltaTime = 0.0f;
    double lastDeltaTime = 0.0f;
    double lastFrame = 0.0f;
    double timer = 0;
    double runtime = 0;

    std::array<double, 10> framerates;

    Time();

    // updates the "fixedUpdate" timer, interpolates deltaTime, and tracks the average framerate
    void update();
    void beginTimer();
    void resetTimer(double interval)
    {
        timer = timer-interval;
    }

    double framerate()
    {
        double sum = 0;
        for(int i=0; i<framerates.size(); i++)
        {
            sum += framerates[i];
        }
        return sum / framerates.size();
    }

    private:
        int32_t framerateIndex = 0;
};


struct Application
{
    static inline InputManager keyboard, mouse;
    static inline Vector2 cursorPosition;

    uint32_t currentWindow = 0;
    std::vector<Window> windows;

    Time time;
    object::ecs container;
    ProjectManager manager;

    Application();

    static Application& data(void *);
    static void beginEventLoop(Application& app);

    Window& window()
    {
        return windows[currentWindow];
    }
};


namespace object
{
    namespace fn
    {
        extern uint8_t LOAD, START, UPDATE, LATE_UPDATE, FIXED_UPDATE, RENDER, DESTROY;
    }

    void defaultInsertion(entity e, std::vector<entity>& entities, std::vector<size_t>& map);
    void insertionSort(std::vector<entity>& entities, std::vector<size_t>& map, Application& app, object::ecs& container, bool (*criteria)(entity, entity, object::ecs&, void *));
    void setFunctionDefinitions(object::ecs& container, const std::vector<uint8_t *>& references);
}


template <>
struct Serialization<MeshAddon>
{
    static size_t length(const MeshAddon& data)
    {        
        return 
            object::length(data.additions);
    }

    static size_t serialize(const MeshAddon& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.additions, stream, index + count);

        return count;
    }

    static MeshAddon deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        MeshAddon result = MeshAddon();
        size_t count = 0;

        result.additions = object::deserialize<std::vector<MeshModule>>(stream, index + count);
        count += object::length(result.additions);

        return result;
    }
};

template <>
struct Serialization<Mesh>
{
    static size_t length(const Mesh& data)
    {        
        return 
            object::length(data.vertices) + 
            object::length(data.VAO) + 
            object::length(data.VBO) + 
            object::length(data.dimensions) +
            object::length(data.offset);
    }

    static size_t serialize(const Mesh& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.vertices, stream, index + count);
        count += object::serialize(value.VAO, stream, index + count);
        count += object::serialize(value.VBO, stream, index + count);
        count += object::serialize(value.dimensions, stream, index + count);
        count += object::serialize(value.offset, stream, index + count);

        return count;
    }

    static Mesh deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Mesh result = Mesh();
        size_t count = 0;

        result.vertices = object::deserialize<std::vector<Vertex>>(stream, index + count);
        count += object::length(result.vertices);

        result.VAO = object::deserialize<uint32_t>(stream, index + count);
        count += object::length(result.VAO);

        result.VBO = object::deserialize<uint32_t>(stream, index + count);
        count += object::length(result.VBO);

        result.dimensions = object::deserialize<Vector3>(stream, index + count);
        count += object::length(result.dimensions);

        result.offset = object::deserialize<Vector3>(stream, index + count);
        count += object::length(result.offset);

        return result;
    }
};

template <>
struct Serialization<Model>
{
    static size_t length(const Model& value)
    {        
        return 
            object::length(value.data) + 
            object::length(value.texture);
    }

    static size_t serialize(const Model& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.data, stream, index + count);
        count += object::serialize(value.texture, stream, index + count);

        return count;
    }

    static Model deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Model result = Model();
        size_t count = 0;

        result.data = object::deserialize<Mesh>(stream, index + count);
        count += object::length(result.data);

        result.texture = object::deserialize<Texture>(stream, index + count);
        count += object::length(result.texture);

        return result;
    }
};

template <>
struct Serialization<Sprite>
{
    static size_t length(const Sprite& data)
    {        
        return 
            object::length(data.texture) + 
            object::length(data.square);
    }

    static size_t serialize(const Sprite& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.texture, stream, index + count);
        count += object::serialize(value.square, stream, index + count);

        return count;
    }

    static Sprite deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Sprite result = Sprite();
        size_t count = 0;

        result.texture = object::deserialize<Texture>(stream, index + count);
        count += object::length(result.texture);

        result.square = object::deserialize<Mesh>(stream, index + count);
        count += object::length(result.square);

        return result;
    }
};

template <>
struct Serialization<Animation2D>
{
    static size_t length(const Animation2D& data)
    {        
        return 
            object::length(data.frames);
    }

    static size_t serialize(const Animation2D& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.frames, stream, index + count);

        return count;
    }

    static Animation2D deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Animation2D result = Animation2D();
        size_t count = 0;

        result.frames = object::deserialize<std::vector<Texture>>(stream, index + count);
        count += object::length(result.frames);

        return result;
    }
};

#endif