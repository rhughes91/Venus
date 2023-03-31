#include "glad/glad.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include "image/stb_image.h"

#include "windows.h"
#include "psapi.h"

#include "color.h"
#include "file_util.h"
#include "input.h"
#include "setup.h"
#include "structure.h"
#include "ui.h"

#define BENCHMARK 0
#if BENCHMARK
    #include <chrono>
    #include <utility>

    #define BENCHMARK_LOAD 1
    #define BENCHMARK_START 1
    #define BENCHMARK_UPDATE 0
    #define BENCHMARK_LATEUPDATE 0
    #define BENCHMARK_FIXEDUPDATE 0
    #define BENCHMARK_RENDER 1
    #define BENCHMARK_DESTROY 1
#endif

InputManager g_keyboard, g_mouse;
Window g_window;

void Screen::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Screen::initialize(const DirectionalLight& dirLight__, const Shader& screenShader__, uint32_t width, uint32_t height)
{
    dirLight = dirLight__;
    gamma = 1.5f;

    int maxSamples = getMaximumSamples();

    frameBuffer.initialize();
    subBuffer.initialize();
    depthBuffer.initialize();

    frameBuffer.addTexture("multiTexture", width, height, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, 0, GL_REPEAT, std::min(4, maxSamples));
    subBuffer.addTexture("texture", width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_REPEAT, 0);
    
    frameBuffer.addRenderBuffer("renderBuffer", width, height, std::min(4, maxSamples));

    shader = screenShader__;
    shader.use();
    shader.setInt("screenTexture", 0);

    int complete;
    if((complete = frameBuffer.complete()) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR :: " << complete << " :: Screen subbuffer is not complete." << std::endl;

    if((complete = subBuffer.complete()) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR :: " << complete << " :: Screen subbuffer is not complete." << std::endl;
}
void Screen::remove()
{
    frameBuffer.remove();
    subBuffer.remove();
    depthBuffer.remove();
    shader.remove();
}
void Screen::refreshResolution(uint32_t width, uint32_t height)
{
    frameBuffer.refresh(g_window.width, g_window.height, defaultColor.a == 1);
    subBuffer.refresh(g_window.width, g_window.height, defaultColor.a == 1);
    resolutionUpdated = true;
}
void Screen::store()
{
    glViewport(0, 0, g_window.width, g_window.height);
    frameBuffer.bind(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);
}
void Screen::draw()
{
    frameBuffer.bind(GL_READ_FRAMEBUFFER);
    subBuffer.bind(GL_DRAW_FRAMEBUFFER);
    glBlitFramebuffer(0, 0, g_window.width, g_window.height, 0, 0, g_window.width, g_window.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    frameBuffer.unbind(GL_FRAMEBUFFER);
    glDisable(GL_DEPTH_TEST);
    glClearColor(defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    shader.use();
    shader.setFloat("gamma", gamma);
    quad.draw(subBuffer.getTexture("texture").data);

    frameBuffer.unbind(GL_READ_FRAMEBUFFER);
    subBuffer.unbind(GL_DRAW_FRAMEBUFFER);
}
int Screen::getMaximumSamples()
{
    int maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    return maxSamples;
}

void object::load()
{
    #if BENCHMARK_LOAD
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.load();

    #if BENCHMARK_LOAD
        std::cout << "LOAD: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::start()
{
    #if BENCHMARK_START
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.start();
    
    #if BENCHMARK_START
        std::cout << "START: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::destroy()
{
    #if BENCHMARK_DESTROY
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.destroy();

    #if BENCHMARK_DESTROY
        std::cout << "DESTROY: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::update()
{
    #if BENCHMARK_UPDATE
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.update();

    #if BENCHMARK_UPDATE
        std::cout << "UPDATE: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::lateUpdate()
{
    #if BENCHMARK_LATEUPDATE
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.lateUpdate();

    #if BENCHMARK_LATEUPDATE
        std::cout << "LATE UPDATE: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::fixedUpdate()
{
    #if BENCHMARK_FIXEDUPDATE
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_manager.fixedUpdate();

    #if BENCHMARK_FIXEDUPDATE
        std::cout << "FIXED UPDATE: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void object::render()
{
    #if BENCHMARK_RENDER
        auto start = std::chrono::high_resolution_clock::now();
    #endif

    g_window.screen.store();
    window::clearScreen(object::getComponent<Camera>(window::camera()).backgroundColor);
    g_manager.render();
    g_window.screen.draw();

    #if BENCHMARK_RENDER
        std::cout << "RENDER: " << ((double)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) * 0.001) << " ms\n";
    #endif
}
void Mesh::draw(const uint32_t texture) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_window.screen.depthBuffer.getTexture("texture").data);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
void Text::render(const Vector2& position)
{
    shader.use();      
    shader.setFloat("aspect", window::aspectRatio());
    shader.setFloat("scale", scale);
    shader.setVec2("position", position);
    shader.setVec4("objColor", color);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, points.size());
}

void shader::simple(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)
{
    Transform& transform = object::getComponent<Transform>(entity);
    Shader shader = model.material.shader;
    shader.use();
    
    shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
    shader.setMat4("view", camera.view.matrix, true);
    shader.setMat4("projection", camera.projection.matrix, true);
    shader.setVec3("scale", transform.scale);
    shader.setVec4("objColor", model.color);
}
void shader::ui(Entity entity, const Model& model, const Camera& camera = Camera(), const Transform& cameraTransform = Transform())
{
    Rect& transform = object::getComponent<Rect>(entity);
    Shader shader = model.material.shader;
    shader.use();
    
    shader.setFloat("aspect", window::aspectRatio());
    shader.setVec2("position", transform.relativePosition());
    shader.setVec2("scale", transform.scale);
    shader.setMat4("model", (mat4x4(1) * (mat4x4)transform.rotation).matrix, false);
    shader.setVec4("objColor", model.color);
}
void shader::advanced(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform, const Vector3& strength, int32_t shininess)
{
    Transform& transform = object::getComponent<Transform>(entity);
    DirectionalLight& light = window::lighting();
    Shader shader = model.material.shader;
    shader.use();
    
    shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
    shader.setMat4("view", camera.view.matrix, true);
    shader.setMat4("projection", camera.projection.matrix, true);
    shader.setVec3("scale", transform.scale);
    
    shader.setVec3("dirLight.direction", light.direction);
    shader.setVec4("dirLight.color", light.color);
    shader.setFloat("dirLight.strength", light.strength);

    shader.setVec3("viewPos", cameraTransform.position);
    shader.setVec4("objColor", model.color);

    shader.setFloat("material.shininess", shininess);
    shader.setFloat("material.ambientStrength", strength.x);
    shader.setFloat("material.diffuseStrength", strength.y);
    shader.setFloat("material.specularStrength", strength.z);
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
    return key::UNKNOWN;
}
uint32_t glInputToButtonCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_MOUSE_BUTTON_1: return mouse::BUTTON_0;
        case GLFW_MOUSE_BUTTON_2: return mouse::BUTTON_1;
        case GLFW_MOUSE_BUTTON_3: return mouse::BUTTON_2;
        case GLFW_MOUSE_BUTTON_4: return mouse::BUTTON_3;
        case GLFW_MOUSE_BUTTON_5: return mouse::BUTTON_4;
        case GLFW_MOUSE_BUTTON_6: return mouse::BUTTON_5;
        case GLFW_MOUSE_BUTTON_7: return mouse::BUTTON_6;
        case GLFW_MOUSE_BUTTON_8: return mouse::BUTTON_7;
    }
    return mouse::UNKNOWN;
}

Time::Time()
{
    framerates = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
    advanceKey = key::BACKSLASH;
}
void Time::update()
{
    runtime = glfwGetTime();
    lastDeltaTime = deltaTime;
    deltaTime = (runtime - lastFrame);
    lastFrame = runtime;
    timer += deltaTime;
    framerates[framerateIndex = (framerateIndex+1)%10] = (1/deltaTime);
}
void Time::beginTimer()
{
    float currentFrame = glfwGetTime();
    lastFrame = currentFrame;
}

void error_callback(int error, const char* msg) {
	std::string s;
	s = " [" + std::to_string(error) + "] " + msg + '\n';
	std::cout << s << std::endl;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    for(GLenum k = 32; k<=GLFW_KEY_LAST; k++)
    {
        g_keyboard.parse(glInputToKeyCode(k), glfwGetKey(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_keyboard.parse(glInputToKeyCode(key), false);
    }

    if(key == GLFW_KEY_F && action == GLFW_PRESS && !g_window.screen.fullscreen)
    {
        if(window::minimized())
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
        window::fullscreen(!g_window.screen.fullscreen);
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
        g_window.width = width;
        g_window.height = height;
        g_window.screen.refreshResolution(width, height);
    }

    glViewport(0, 0, width, height);
}

struct FrameText : Script
{
    Timer fpsTimer = Timer(1.f/2);
    Text *text;
    Color fontColor = color::BLACK;

    MEMORYSTATUSEX memInfo;
    PROCESS_MEMORY_COUNTERS_EX pmc;
    DWORDLONG totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
    SIZE_T virtualMemUsedByMe, physMemUsedByMe;
};
Window::Window(std::string name, uint32_t width__, uint32_t height__)
{
    if(g_window.data != NULL)
        return;

    width = width__;
    height = height__;
    screen.defaultColor = color::RED;
    
    if(!glfwInit())
    {
        std::cout << "ERROR :: GLFW could not be initialized." << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwSetErrorCallback(error_callback);

    data = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
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
    screen.initialize(DirectionalLight(vec3::forward, color::WHITE), Shader("screen_vertex", "screen_frag"), width, height);
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
    glViewport(0, 0, g_window.width, g_window.height);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    active = true;
}
void Window::initText()
{
    text = &object::initializeScript<FrameText>();
    {
        Object textbox("text");
        Rect& box = textbox.addComponent<Rect>(Rect(Alignment(alignment::TOP, alignment::LEFT), {1, 0.5f}, {0.025f, -0.025f}));
        ((FrameText *)text) -> text = &textbox.addComponent<Text>(Text(ttf::get("times new roman.ttf"), "", color::BLACK, 3, Alignment(alignment::TOP, alignment::LEFT), box, shader::get("spline_shader")));

        ((FrameText *)text) -> load([]
        (System& script)
        {
            FrameText& data = script.data<FrameText>();
            data.fpsTimer.begin();

            data.text -> setColor(data.fontColor);

            data.memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&data.memInfo);

            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&data.pmc, sizeof(data.pmc));

            data.totalVirtualMem = data.memInfo.ullTotalPageFile;
            data.virtualMemUsed = data.memInfo.ullTotalPageFile - data.memInfo.ullAvailPageFile;
            data.virtualMemUsedByMe = data.pmc.PrivateUsage;

            data.totalPhysMem = data.memInfo.ullTotalPhys;
            data.physMemUsed = data.memInfo.ullTotalPhys - data.memInfo.ullAvailPhys;
            data.physMemUsedByMe = data.pmc.WorkingSetSize;

            std::string ramUsed = std::to_string((float)data.virtualMemUsedByMe / 1000000.0f);
            ramUsed = ramUsed.substr(0, ramUsed.size()-5);

            std::string ramTotal = std::to_string((float)data.totalVirtualMem / 1000000.0f);
            ramTotal = ramTotal.substr(0, ramTotal.size()-5);

            std::string diskUsed = std::to_string((float)data.physMemUsedByMe / 1000000.0f);
            diskUsed = diskUsed.substr(0, diskUsed.size()-5);

            std::string diskTotal = std::to_string((float)data.totalPhysMem / 1000000.0f);
            diskTotal = diskTotal.substr(0, diskTotal.size()-5);

            data.text -> setText("fps:\t" + std::to_string((int)event::framerate()) + "\nRAM:\t" + ramUsed + "\t/\t" + ramTotal + " MB" + "\nDisk:\t" + diskUsed + "\t/\t" + diskTotal + " MB");
            data.fpsTimer.reset();
        });
        ((FrameText *)text) -> update([]
        (System& script)
        {
            FrameText& data = script.data<FrameText>();

            data.text -> setColor(data.fontColor);

            data.memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&data.memInfo);

            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&data.pmc, sizeof(data.pmc));

            data.totalVirtualMem = data.memInfo.ullTotalPageFile;
            data.virtualMemUsed = data.memInfo.ullTotalPageFile - data.memInfo.ullAvailPageFile;
            data.virtualMemUsedByMe = data.pmc.PrivateUsage;

            data.totalPhysMem = data.memInfo.ullTotalPhys;
            data.physMemUsed = data.memInfo.ullTotalPhys - data.memInfo.ullAvailPhys;
            data.physMemUsedByMe = data.pmc.WorkingSetSize;

            data.fpsTimer.update(1/2.f);
            while(data.fpsTimer.set())
            {
                std::string ramUsed = std::to_string((float)data.virtualMemUsedByMe / 1000000.0f);
                ramUsed = ramUsed.substr(0, ramUsed.size()-5);

                std::string ramTotal = std::to_string((float)data.totalVirtualMem / 1000000.0f);
                ramTotal = ramTotal.substr(0, ramTotal.size()-5);

                std::string diskUsed = std::to_string((float)data.physMemUsedByMe / 1000000.0f);
                diskUsed = diskUsed.substr(0, diskUsed.size()-5);

                std::string diskTotal = std::to_string((float)data.totalPhysMem / 1000000.0f);
                diskTotal = diskTotal.substr(0, diskTotal.size()-5);

                data.text -> setText("fps:\t" + std::to_string((int)event::framerate()) + "\nRAM:\t" + ramUsed + "\t/\t" + ramTotal + " MB" + "\nDisk:\t" + diskUsed + "\t/\t" + diskTotal + " MB");
                data.fpsTimer.reset();
            }
        });
    }
}

bool key::pressed(int32_t id)
{
    return g_keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyCode id)
{
    return g_keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].pressed)
            return true;
    }
    return false;
}
bool key::held(int32_t id)
{
    return g_keyboard.inputs[id].held;
}
bool key::held(key::KeyCode id)
{
    return g_keyboard.inputs[id].held;
}
bool key::held(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].held)
            return true;
    }
    return false;
}
bool key::released(int32_t id)
{
    return g_keyboard.inputs[id].released;
}
bool key::released(key::KeyCode id)
{
    return g_keyboard.inputs[id].released;
}
bool key::released(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].released)
            return true;
    }
    return false;
}

bool mouse::pressed(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].pressed;
}
bool mouse::held(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].held;
}
bool mouse::released(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].held;
}

bool window::active()
{
    return g_window.active;
}
bool window::closing()
{
    return glfwWindowShouldClose((GLFWwindow *)g_window.data);
}
bool window::decorated()
{
    return glfwGetWindowAttrib((GLFWwindow *)g_window.data, GLFW_DECORATED);
}
bool window::maximized()
{
    return glfwGetWindowAttrib((GLFWwindow *)g_window.data, GLFW_MAXIMIZED);
}
bool window::minimized()
{
    return !glfwGetWindowAttrib((GLFWwindow *)g_window.data, GLFW_MAXIMIZED);
}
bool window::resolutionUpdated()
{
    return g_window.screen.resolutionUpdated;
}
bool window::throwError()
{
    uint32_t error = glGetError();
    std::string errorMessage;
    switch(error)
    {
        case 0:
            errorMessage = "SUCCESS";
        break;
        case 1280:
            errorMessage = "ERROR :: 1280 :: Invalid enumeration parameter.";
        break;
        case 1281:
            errorMessage = "ERROR :: 1281 :: Invalid value parameter.";
        break;
        case 1282:
            errorMessage = "ERROR :: 1282 :: Invalid command state for given parameter.";
        break;
        case 1283:
            errorMessage = "ERROR :: 1283 :: Stack overflow.";
        break;
        case 1284:
            errorMessage = "ERROR :: 1284 :: Stack underflow.";
        break;
        case 1285:
            errorMessage = "ERROR :: 1285 :: Memory could not be allocated.";
        break;
        case 1286:
            errorMessage = "ERROR :: 1286 :: Framebuffer incomplete.";
        break;
    }
    std::cout << errorMessage << std::endl;
    return error;
}
bool window::vsyncEnabled()
{
    return g_window.vsyncEnabled;
}
void window::centerWindow()
{
    setPosition(monitorCenter() - Vector2I(0.5*g_window.width, 0.5*g_window.height));
}
void window::clearScreen(const Color& color)
{
    g_window.screen.clear(color);
}
void window::close()
{
    glfwSetWindowShouldClose((GLFWwindow *)g_window.data, true);
}
void window::enableDecoration(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)g_window.data, GLFW_DECORATED, enable);
}
void window::enableVSync(bool enable)
{
    glfwSwapInterval(g_window.vsyncEnabled = enable);
}
void window::fullscreen(bool enable)
{
    if(enable && !g_window.screen.fullscreen)
    {
        g_window.screen.fullscreen = true;
        g_window.screen.m_lastResolution = Vector2(g_window.width, g_window.height);
        glfwGetWindowPos((GLFWwindow *)g_window.data, &g_window.screen.lastPosition.x, &g_window.screen.lastPosition.y);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor((GLFWwindow *)g_window.data, monitor, 0, 0, mode->width, mode->height, g_window.vsyncEnabled ? mode->refreshRate : GLFW_DONT_CARE);
    }
    else if(!enable && g_window.screen.fullscreen)
    {
        g_window.screen.fullscreen = false;
        glfwSetWindowMonitor((GLFWwindow *)g_window.data, nullptr, 0, 0, g_window.screen.m_lastResolution.x, g_window.screen.m_lastResolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_window.data, g_window.screen.lastPosition.x, g_window.screen.lastPosition.y);
    }
}
void window::hideCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)g_window.data, GLFW_CURSOR, enable ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}
void window::lockCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)g_window.data, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void window::maximize()
{
    if(g_window.screen.fullscreen)
    {
        glfwSetWindowMonitor((GLFWwindow *)g_window.data, nullptr, 0, 0, g_window.screen.m_lastResolution.x, g_window.screen.m_lastResolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_window.data, g_window.screen.lastPosition.x, g_window.screen.lastPosition.y);
        g_window.screen.fullscreen = false;
    }

    glfwMaximizeWindow((GLFWwindow *)g_window.data);
}
void window::minimize()
{
    if(g_window.screen.fullscreen)
    {
        glfwSetWindowMonitor((GLFWwindow *)g_window.data, nullptr, 0, 0, g_window.screen.m_lastResolution.x, g_window.screen.m_lastResolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_window.data, g_window.screen.lastPosition.x, g_window.screen.lastPosition.y);
        g_window.screen.fullscreen = false;
    }

    glfwRestoreWindow((GLFWwindow *)g_window.data);
}
void window::refresh()
{
    glfwSwapBuffers((GLFWwindow *)g_window.data);
    glfwPollEvents();
}
void window::remove()
{
    g_window.screen.remove();
}
void window::setCamera(uint32_t newCamera)
{
    g_window.screen.camera = newCamera;
}
void window::setCursor(const Vector2& position)
{
    glfwSetCursorPos((GLFWwindow *)g_window.data, position.x, position.y);
}
void window::setDefaultBackgroundColor(const Color &color)
{
    g_window.screen.defaultColor = color;
    g_window.screen.refreshResolution(g_window.width, g_window.height);
}
void window::setFontColor(const Color &color)
{
    ((FrameText *)g_window.text) -> fontColor = color;
}
void window::setIcon(const char *path)
{
    GLFWimage images[1];

    stbi_set_flip_vertically_on_load(false);
    images[0].pixels = stbi_load((source::root() + std::string("resources/images/") + path).c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon((GLFWwindow *)g_window.data, 1, images); 
    stbi_image_free(images[0].pixels);
    stbi_set_flip_vertically_on_load(true);
}
void window::setOpacity(float opacity)
{
    glfwSetWindowOpacity((GLFWwindow *)g_window.data, opacity);
}
void window::setPosition(const Vector2I& position)
{
    glfwSetWindowPos((GLFWwindow *)g_window.data, position.x, position.y);
}
void window::setTitle(const char *title)
{
    glfwSetWindowTitle((GLFWwindow *)g_window.data, title);
}
void window::storeRender()
{
    g_window.screen.store();
}
void window::drawRender()
{
    g_window.screen.draw();
}
void window::terminate()
{
    glfwTerminate();
}
float window::aspectRatioInv()
{
    return (float)g_window.width/(float)g_window.height;
}
float window::aspectRatio()
{
    return (float)g_window.height/(float)g_window.width;
}
float window::getOpacity()
{
    return glfwGetWindowOpacity((GLFWwindow *)g_window.data);
}
float window::height()
{
    return g_window.height;
}
float window::width()
{
    return g_window.width;
}
Vector2 window::cursorUniformScreenPosition()
{
    return {((float)g_window.cursorPosition.x/(float)g_window.width)*2 - 1, -((float)g_window.cursorPosition.y/(float)g_window.height)*2 + 1};
}
Vector2 window::cursorScreenPosition()
{
    return cursorUniformScreenPosition() * Vector2{aspectRatioInv(), 1};
}
Vector2I window::center()
{
    return Vector2I(monitorCenter() - Vector2I(0.5*g_window.width, 0.5*g_window.height));
}
Vector2I window::resolution()
{
    return Vector2I(g_window.width, g_window.height);
}
Vector2I window::monitorCenter()
{
    Vector2I result;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &result.x, &result.y);
    return result * 2;
}
uint32_t window::camera()
{
    return g_window.screen.camera;
}
DirectionalLight& window::lighting()
{
    return g_window.screen.dirLight;
}
DirectionalLight& window::setLighting(const DirectionalLight& light)
{
    return(g_window.screen.dirLight = light);
}

bool createWindow(const char *name, uint32_t width, uint32_t height)
{
    if(g_window.active)
        return false;

    g_window = Window(name, width, height);
    if(!g_window.active)
        return false;
    
    stbi_set_flip_vertically_on_load(true);
    // texture::load("default", file::loadPNG(source::root() + source::texture() + "default.png"), 16, 16, texture::RGBA, texture::PNG);
    texture::load("default", {Color8(255, 255, 255, 255)}, 1, 1, texture::RGBA, texture::PNG);

    mesh::load("square", shape::square());
    mesh::load("cube", shape::cube());

    file::loadFilesInDirectory(source::root() + source::font(), ttf::load);

    std::vector<std::string> shaderConfig = file::loadFileToStringVector(source::config() + "shader.config");
    for(const auto& line : shaderConfig)
    {
        int index = line.find(':');
        if(index != std::string::npos)
        {
            int comma = line.find(',');

            // std::cout << line.substr(0, index) << " : " << line.substr(index+2, comma - index-2) << std::endl;
            shader::load(line.substr(0, index), Shader(line.substr(index+2, comma - index-2), line.substr(comma+2)));
        }
    }

    shader::get("object_shader").use();
    shader::get("object_shader").setInt("material.diffuse", 0);
    
    shader::get("simple_shader").use();
    shader::get("simple_shader").setInt("material.texture", 0);
    
    shader::get("ui_shader").use();
    shader::get("ui_shader").setInt("text", 0);

    g_window.screen.quad = mesh::get("square");
    g_window.screen.quad.refresh();

    g_keyboard.initialize(key::LAST);
    g_mouse.initialize(mouse::LAST);

    g_window.initText();
    
    return true;
}