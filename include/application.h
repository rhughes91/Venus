#pragma once

#include "audio.h"
#include "component.h"
#include "graphics.h"
#include "machine.h"
#include "structure.h"
#include "ui.h"


using AnimationStateUV = object::state_machine<AnimationUV>;
using AnimatorUV = object::state_machine<AnimationStateUV>;

using AnimationState = object::state_machine<Animation2D>;
using Animator2D = object::state_machine<AnimationState>;

struct Application;

struct AnimationManager{};
struct AnimationUVManager{};
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
struct UIManager{};

struct SimpleRenderer
{
    int model, view, projection, scale, color, offset, uvScale, flip;
};
struct AdvancedRenderer
{
    int model, view, projection, scale, color, viewPos, lightDir, lightColor, lightStr, matShininess, matAmb, matDiff, matSpec;
};
struct ComplexRenderer
{
    bool update = true;
    int model, view, projection, scale, color, offset, uvScale, flip;
};
struct UIRenderer
{
    bool update = true;
    int aspect, position, scale, model, color;
};
struct TextRenderer{};

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
    static size_t length(const MeshAddon& data)
    {        
        return object::length(data.additions);
    }

    static size_t serialize(const MeshAddon& value, std::vector<uint8_t>& stream, size_t index)
    {
        return object::serialize(value.additions, stream, index);
    }

    static MeshAddon deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        MeshAddon result = MeshAddon();
        result.additions = object::deserialize<std::vector<MeshModule>>(stream, index);

        return result;
    }


    MeshAddon() {}
    MeshAddon(const std::vector<MeshModule>& additions__) : additions(additions__) {}

    void append(Model& model, const Transform& parentTransform);

    private:
        std::vector<MeshModule> additions;
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

//
struct Scene
{
    uint32_t pause = -1;
    object::ecs container;

    Scene(const object::ecs& container__);

    template<typename T>
    void setPausable(uint8_t function)
    {
        container.addToToggle<T>(pause, function);
    }

    template<typename T>
    void setPausable()
    {
        container.addToToggle<T>(pause);
    }

    void pauseScene()
    {
        container.toggle(pause);
    }
};

// Time (struct): holds all the timing data that happens between frames :: controls when "fixedUpdate" is run
struct Time
{
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
        for(size_t i=0; i<framerates.size(); i++)
        {
            sum += framerates[i];
        }
        return sum / framerates.size();
    }

    private:
        int32_t framerateIndex = 0;
};

// Window (struct): determines the appearance of the application Window
class Window
{
    public:
        Screen screen;
        Vector2I lastPosition, lastResolution;
        uint16_t width = 0;
        uint16_t height = 0;

        void *data, *audioDevice, *audioContext;
        bool vsyncEnabled = false, isFullscreen = false;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height, uint32_t x, uint32_t y);

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
        void enableFloating(bool enable);
        void enablePassthrough(bool enable);
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
        void updateResolution();

        float aspectRatioInv();
        float aspectRatio();
        float getOpacity();

        Vector2I position();
        Vector2 fetchCursorPos();
        Vector2 cursorUniformScreenPosition();
        Vector2 cursorScreenPosition();
        Vector2I center();
        Vector2I monitorCenter();
        Vector2I resolution();

        Color getPixelColor(const Vector2& position);
        
    private:
        void configureGLAD();
};

//
namespace object
{
    using event = uint32_t;
    struct Event
    {
        Event() {}
        Event(void(*function)(Application&, void *))
        {
            set(function);
        }

        void set(void(*function)(Application&, void *))
        {
            func = function;
        }

        void run(Application &app, void *data)
        {
            func(app, data);
        }

        private:
            void (*func)(Application &app, void *data) = [](Application &app, void *data){};
    };

    namespace fn
    {
        extern uint8_t LOAD, START, UPDATE, LATE_UPDATE, FIXED_UPDATE, RENDER, DESTROY;
    }

    void defaultInsertion(entity e, std::vector<entity>& entities, std::vector<size_t>& map);
    void insertionSort(std::vector<entity>& entities, std::vector<size_t>& map, Application& app, object::ecs& container, bool (*criteria)(entity, entity, object::ecs&, void *));
    void setFunctionDefinitions(object::ecs& container, const std::vector<uint8_t *>& references);
}

struct Application
{
    static inline InputManager keyboard, mouse;
    static inline std::array<JoystickManager, 17> controllers;

    Application(const std::string& name = "default", uint32_t width = 800, uint32_t height = 600, uint32_t x = 0, uint32_t = 25);

    static Application& data(void *);
    static void runEventLoop(Application& app);

    Window& window()
    {
        return windows[currentWindow];
    }


    object::event createEvent(void(*function)(Application&, void *))
    {
        object::event e = events.size();
        events.push_back(object::Event(function));
        return e;
    }

    void runEvent(object::event event, void *data = NULL)
    {
        if(event >= events.size())
        {
            std::cout << "ERROR :: Event does not exist.\n";
            return;
        }
        events[event].run(*this, data);
    }


    uint32_t createScene();

    uint32_t getCurrentScene() const
    {
        return currentScene;
    }

    Scene& getScene(uint32_t scene)
    {
        return scenes[scene];
    }

    Scene& getScene();

    void setScene(uint32_t scene)
    {
        sceneChanged = true;
        lastScene = currentScene;
        currentScene = scene;
    }

    void setScene()
    {
        setScene(currentScene);
    }

    Time getTime()
    {
        return time;
    }

    static uint32_t currentController()
    {
        return currentJoystick;
    }

    static Vector2 getCursorPos()
    {
        return cursorPosition;
    }

    static void setCursorPos(const Vector2& pos)
    {
        cursorPosition = pos;
    }

    static void updateCursor()
    {
        lastCursorPosition = cursorPosition;
    }

    static Vector2 cursorDifference()
    {
        return lastCursorPosition - cursorPosition;
    }

    private:
        static inline uint32_t currentJoystick = 0;
        static inline Vector2 cursorPosition;
        static inline Vector2 lastCursorPosition = 0;

        uint32_t currentWindow = 0;
        std::vector<Window> windows;

        bool sceneChanged = false;
        uint32_t currentScene = -1, lastScene = -1;
        std::vector<Scene> scenes;

        std::vector<object::Event> events;

        Time time;

        void updateScene();
};


template <>
struct Serialization<ShaderUniforms>
{
    static size_t length(const ShaderUniforms& value)
    {        
        return 
            object::length(value.booleans) + 
            object::length(value.integers) +
            object::length(value.uIntegers) +
            object::length(value.floats) +
            object::length(value.doubles) +
            object::length(value.vBooleans) +
            object::length(value.vIntegers) +
            object::length(value.vUIntegers) +
            object::length(value.vFloats) +
            object::length(value.vDoubles);
    }

    static size_t serialize(const ShaderUniforms& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.booleans, stream, index + count);
        count += object::serialize(value.integers, stream, index + count);
        count += object::serialize(value.uIntegers, stream, index + count);
        count += object::serialize(value.floats, stream, index + count);
        count += object::serialize(value.doubles, stream, index + count);
        count += object::serialize(value.vBooleans, stream, index + count);
        count += object::serialize(value.vIntegers, stream, index + count);
        count += object::serialize(value.vUIntegers, stream, index + count);
        count += object::serialize(value.vFloats, stream, index + count);
        count += object::serialize(value.vDoubles, stream, index + count);

        return count;
    }

    static ShaderUniforms deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        ShaderUniforms result = ShaderUniforms();
        size_t count = 0;

        result.booleans = object::deserialize<std::vector<bool>>(stream, index + count);
        count += object::length(result.booleans);

        result.integers = object::deserialize<std::vector<int32_t>>(stream, index + count);
        count += object::length(result.integers);

        result.uIntegers = object::deserialize<std::vector<uint32_t>>(stream, index + count);
        count += object::length(result.uIntegers);

        result.floats = object::deserialize<std::vector<float>>(stream, index + count);
        count += object::length(result.floats);

        result.doubles = object::deserialize<std::vector<double>>(stream, index + count);
        count += object::length(result.doubles);

        result.vBooleans = object::deserialize<std::vector<Vector2>>(stream, index + count);
        count += object::length(result.vBooleans);
        
        result.vIntegers = object::deserialize<std::vector<Vector2>>(stream, index + count);
        count += object::length(result.vIntegers);

        result.vUIntegers = object::deserialize<std::vector<Vector2>>(stream, index + count);
        count += object::length(result.vUIntegers);

        result.vFloats = object::deserialize<std::vector<Vector2>>(stream, index + count);
        count += object::length(result.vFloats);

        result.vDoubles = object::deserialize<std::vector<Vector2>>(stream, index + count);
        count += object::length(result.vDoubles);

        return result;
    }
};


template <>
struct Serialization<SimpleShader>
{
    static size_t length(const SimpleShader& value)
    {        
        return 
            object::length(value.values) + 
            object::length(value.color) +
            object::length(value.flip);
    }

    static size_t serialize(const SimpleShader& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.values, stream, index + count);
        count += object::serialize(value.color, stream, index + count);
        count += object::serialize(value.flip, stream, index + count);

        return count;
    }

    static SimpleShader deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        SimpleShader result = SimpleShader();
        size_t count = 0;

        result.values = object::deserialize<ShaderUniforms>(stream, index + count);
        count += object::length(result.values);

        result.color = object::deserialize<Color>(stream, index + count);
        count += object::length(result.color);

        result.flip = object::deserialize<bool>(stream, index + count);
        count += object::length(result.flip);

        return result;
    }
};

template <>
struct Serialization<TextShader>
{
    static size_t length(const TextShader& value)
    {        
        return 
            object::length(value.values) + 
            object::length(value.color) +
            object::length(value.flip);
    }

    static size_t serialize(const TextShader& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.values, stream, index + count);
        count += object::serialize(value.color, stream, index + count);
        count += object::serialize(value.flip, stream, index + count);

        return count;
    }

    static TextShader deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        TextShader result = TextShader();
        size_t count = 0;

        result.values = object::deserialize<ShaderUniforms>(stream, index + count);
        count += object::length(result.values);

        result.color = object::deserialize<Color>(stream, index + count);
        count += object::length(result.color);

        result.flip = object::deserialize<bool>(stream, index + count);
        count += object::length(result.flip);

        return result;
    }
};

template <>
struct Serialization<ComplexShader>
{
    static size_t length(const ComplexShader& value)
    {        
        return 
            object::length(value.values) + 
            object::length(value.color) +
            object::length(value.flip);
    }

    static size_t serialize(const ComplexShader& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.values, stream, index + count);
        count += object::serialize(value.color, stream, index + count);
        count += object::serialize(value.flip, stream, index + count);

        return count;
    }

    static ComplexShader deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        ComplexShader result = ComplexShader();
        size_t count = 0;

        result.values = object::deserialize<ShaderUniforms>(stream, index + count);
        count += object::length(result.values);

        result.color = object::deserialize<Color>(stream, index + count);
        count += object::length(result.color);

        result.flip = object::deserialize<bool>(stream, index + count);
        count += object::length(result.flip);

        return result;
    }
};

template <>
struct Serialization<AdvancedShader>
{
    static size_t length(const AdvancedShader& value)
    {        
        return 
            object::length(value.values) + 
            object::length(value.color) +
            object::length(value.flip) +
            object::length(value.ambient) + 
            object::length(value.diffuse) +
            object::length(value.specular) +
            object::length(value.shine);
    }

    static size_t serialize(const AdvancedShader& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.values, stream, index + count);
        count += object::serialize(value.color, stream, index + count);
        count += object::serialize(value.flip, stream, index + count);
        count += object::serialize(value.ambient, stream, index + count);
        count += object::serialize(value.diffuse, stream, index + count);
        count += object::serialize(value.specular, stream, index + count);
        count += object::serialize(value.shine, stream, index + count);

        return count;
    }

    static AdvancedShader deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        AdvancedShader result = AdvancedShader();
        size_t count = 0;

        result.values = object::deserialize<ShaderUniforms>(stream, index + count);
        count += object::length(result.values);

        result.color = object::deserialize<Color>(stream, index + count);
        count += object::length(result.color);

        result.flip = object::deserialize<bool>(stream, index + count);
        count += object::length(result.flip);

        result.ambient = object::deserialize<float>(stream, index + count);
        count += object::length(result.ambient);

        result.diffuse = object::deserialize<float>(stream, index + count);
        count += object::length(result.diffuse);

        result.specular = object::deserialize<float>(stream, index + count);
        count += object::length(result.specular);

        result.shine = object::deserialize<int32_t>(stream, index + count);
        count += object::length(result.shine);
        

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
            object::length(value.texture) +
            object::length(value.offset) +
            object::length(value.scale);
    }

    static size_t serialize(const Model& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.data, stream, index + count);
        count += object::serialize(value.texture, stream, index + count);
        count += object::serialize(value.offset, stream, index + count);
        count += object::serialize(value.scale, stream, index + count);

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

        result.offset = object::deserialize<Vector2>(stream, index + count);
        count += object::length(result.offset);

        result.scale = object::deserialize<Vector2>(stream, index + count);
        count += object::length(result.scale);

        return result;
    }
};

template <>
struct Serialization<Animation2D>
{
    static size_t length(const Animation2D& data)
    {        
        return 
            object::length(data.frames) +
            object::length(data.currentFrame);
    }

    static size_t serialize(const Animation2D& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.frames, stream, index + count);
        count += object::serialize(value.currentFrame, stream, index + count);

        return count;
    }

    static Animation2D deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Animation2D result = Animation2D();
        size_t count = 0;

        result.frames = object::deserialize<std::vector<Texture>>(stream, index + count);
        count += object::length(result.frames);

        result.currentFrame = object::deserialize<uint32_t>(stream, index + count);
        count += object::length(result.currentFrame);

        return result;
    }
};