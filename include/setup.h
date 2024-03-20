#ifndef SETUP_H
#define SETUP_H

#include "shader.h"
#include "structure.h"

extern object::ecs g_manager;

namespace object
{
    namespace fn
    {
        extern uint8_t LOAD, START, UPDATE, LATE_UPDATE, FIXED_UPDATE, RENDER, DESTROY;
    }

    entity createEntity();
    void removeEntity(entity);
    entity numberOfEntities();
    bool active(entity);
    void setActive(entity, bool);

    size_t numberOfComponents();

    void defaultInsertion(entity e, std::vector<entity>& entities, std::vector<size_t>& map);
    void setFunctionDefinitions(const std::vector<uint8_t *>& references);
    uint8_t createSystemFunction();
    void run(uint8_t index);
    void parseError();

    template<typename T>
    bool active(entity e)
    {
        return g_manager.active<T>(e);
    }

    template<typename T>
    void setActive(entity e, bool active)
    {
        g_manager.setActive<T>(e, active);
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    T& addComponent(entity e, const T& component = T())
    {
        return g_manager.addComponent<T>(e, component);
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    T addComponent(entity e, const T& component = T())
    {
        return g_manager.addComponent<T>(e, component);
    }

    template<typename T>
    void shareComponent(entity e, entity share)
    {
        g_manager.shareComponent<T>(e, share);
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    T& getComponent(entity e)
    {
        return g_manager.getComponent<T>(e);
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    T getComponent(entity e)
    {
        return g_manager.getComponent<T>(e);
    }

    template<typename T>
    bool containsComponent(entity e)
    {
        return g_manager.containsComponent<T>(e);
    }

    template <typename T>
    T removeComponent(entity e)
    {
        return g_manager.removeComponent<T>(e);
    }

    template<typename T, typename... Args>
    object::ecs::system& createSystem(const T& instance = T(), int32_t priority = 0)
    {
        return g_manager.createSystem<T, Args...>(instance, priority);
    }

    template<typename T>
    void setInsertion(void (*insert)(entity, std::vector<entity>&, std::vector<size_t>&))
    {
        g_manager.setInsertion<T>(insert);
    }

    template<typename T>
    std::vector<size_t>& getMapping()
    {
        return g_manager.getMapping<T>();
    }

    template<typename T>
    void setComponent(entity e, const T& update)
    {
        g_manager.setComponent<T>(e, update);
    }

    template<typename T>
    std::vector<entity>& entities()
    {
        return g_manager.entities<T>();
    }

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
            object::length(data.dimensions);
    }

    static size_t serialize(const Mesh& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.vertices, stream, index + count);
        count += object::serialize(value.VAO, stream, index + count);
        count += object::serialize(value.VBO, stream, index + count);
        count += object::serialize(value.dimensions, stream, index + count);

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
            object::length(data.color) + 
            object::length(data.shader) + 
            object::length(data.offset) + 
            object::length(data.scale) + 
            object::length(data.updateSorting) + 
            object::length(data.flip) + 
            object::length(data.square) + 
            object::length(data.sorting);
    }

    static size_t serialize(const Sprite& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.texture, stream, index + count);
        count += object::serialize(value.color, stream, index + count);
        count += object::serialize(value.shader, stream, index + count);
        count += object::serialize(value.offset, stream, index + count);
        count += object::serialize(value.scale, stream, index + count);
        count += object::serialize(value.updateSorting, stream, index + count);
        count += object::serialize(value.flip, stream, index + count);
        count += object::serialize(value.square, stream, index + count);
        count += object::serialize<float>(value.sorting, stream, index + count);

        return count;
    }

    static Sprite deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Sprite result = Sprite();
        size_t count = 0;

        result.texture = object::deserialize<Texture>(stream, index + count);
        count += object::length(result.texture);

        result.color = object::deserialize<Color>(stream, index + count);
        count += object::length(result.color);

        result.shader = object::deserialize<Shader>(stream, index + count);
        count += object::length(result.shader);

        result.offset = object::deserialize<Vector2>(stream, index + count);
        count += object::length(result.offset);

        result.scale = object::deserialize<Vector2>(stream, index + count);
        count += object::length(result.scale);

        result.updateSorting = object::deserialize<bool>(stream, index + count);
        count += object::length(result.updateSorting);

        result.flip = object::deserialize<bool>(stream, index + count);
        count += object::length(result.flip);

        result.square = object::deserialize<Mesh>(stream, index + count);
        count += object::length(result.square);

        result.sorting = object::deserialize<float>(stream, index + count);
        count += object::length(result.sorting);

        return result;
    }
};


struct AnimationManager{};
struct AnimationManager2D{};
struct BillboardManager{};
struct ButtonManager{};
struct CameraManager{};
struct CollisionManager{};
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

struct AABB2DHandler{};
struct AABBHandler{};

struct ProjectManager
{
    ProjectManager();
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
    Color defaultColor;
    uint32_t camera;

    bool resolutionUpdated;
    Vector2 resolution;
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

        void *data, *audioDevice, *audioContext;
        bool active, vsyncEnabled, fullscreen, capslock = false, numpad = false;

        Window() {};
        Window(std::string name, uint32_t width, uint32_t height);
        
    private:
        void configureGLAD();
};

namespace window
{
    void *handle();

    bool active();
    bool capslockEnabled();
    bool closing();
    bool decorated();
    bool fullscreened();
    bool maximized();
    bool numpadEnabled();
    bool resolutionUpdated();
    bool throwAudioError();
    bool throwError();
    bool vsyncEnabled();

    void centerWindow();
    void clearScreen(const Color& color);
    void close();
    void drawRender();
    void enableCapslock(bool enable);
    void enableDecoration(bool enable);
    void enableNumpad(bool enable);
    void enableVSync(bool enable);
    void fullscreen(bool enable);
    float gamma();
    void hideCursor(bool enable);
    void lockCursor(bool enable);
    void maximize();
    void minimize();
    void poll();
    void refresh();
    void remove();
    void setActiveWindow(uint32_t index);
    void setCamera(uint32_t newCamera);
    void setCursor(const Vector2& position);
    void setDefaultBackgroundColor(const Color &color);
    void setGamma(float value);
    void setIcon(const char *path);
    void setOpacity(float opacity);
    void setPosition(const Vector2I& position);
    void setSize(int width, int height);
    void setTitle(const char *title);
    void storeRender();
    void terminate();
    void updatedResolution(bool updated);

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

    int32_t getMaximumSamples();
    uint32_t camera();
    DirectionalLight& lighting();
    DirectionalLight& setLighting(const DirectionalLight& light);
}

// begins the primary game loop
void beginEventLoop();

// prints the currect OpenGL error buffer
int32_t throwGLError(const std::string& error);

#endif