#include "file_util.h"
#include "input.h"
#include "setup.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "image/stb_image.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <type_traits>


std::string Physics2D::collisionsToString() const
{
    return "[UP : " + std::to_string(collisions[physics::UP]) + ", DOWN : " + std::to_string(collisions[physics::DOWN]) + ", LEFT : " + std::to_string(collisions[physics::LEFT]) + ", RIGHT : " + std::to_string(collisions[physics::RIGHT]);
}
void physics::collisionTrigger(entity e, entity collision, bool edge, int triggered, object::ecs& container)
{    
    Physics2D& physics = container.getComponent<Physics2D>(e);
    if(!triggered)
    {
        physics.resetCollisions();
    }

    if(edge)
        return;

    Transform& transform = container.getComponent<Transform>(e);
    BoxCollider& collider = container.getComponent<BoxCollider>(e);
    Vector3 boxDim = transform.scale * collider.scale * 0.5f;

    Transform& transform2 = container.getComponent<Transform>(collision);
    BoxCollider& collider2 = container.getComponent<BoxCollider>(collision);
    Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;
   
    Vector3 delta = ((transform.position + collider.offset) - (transform.storedPosition + collider.offset)) + ((transform2.position + collider2.offset) - (transform2.storedPosition + collider2.offset));
    float precision = 4;

    bool above = math::roundTo(transform.storedPosition.y + collider.offset.y - boxDim.y, precision) >= math::roundTo(transform2.storedPosition.y + collider2.offset.y + boxDim2.y, precision), below = math::roundTo(transform.storedPosition.y + collider.offset.y + boxDim.y, precision) <= math::roundTo(transform2.storedPosition.y + collider2.offset.y - boxDim2.y, precision);
    bool right = math::roundTo(transform.storedPosition.x + collider.offset.x - boxDim.x, precision) >= math::roundTo(transform2.storedPosition.x + collider2.offset.x + boxDim2.x, precision), left = math::roundTo(transform.storedPosition.x + collider.offset.x + boxDim.x, precision) <= math::roundTo(transform2.storedPosition.x + collider2.offset.x - boxDim2.x, precision);

    bool vertical = above || below;
    bool horizontal = right || left;

    if(!horizontal && delta.y < 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::DOWN] = true;
        transform.position.y = transform2.position.y - collider.offset.y + (boxDim2.y + boxDim.y) + collider2.offset.y;
    }
    else if(!horizontal && delta.y > 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::UP] = true;
        transform.position.y = transform2.position.y - collider.offset.y - (boxDim2.y + boxDim.y) + collider2.offset.y;
    }
    else if(!vertical && delta.x < 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::LEFT] = true;
        transform.position.x = transform2.position.x - collider.offset.x + (boxDim2.x + boxDim.x) + collider2.offset.x;
    }
    else if(!vertical && delta.x > 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::RIGHT] = true;
        transform.position.x = transform2.position.x - collider.offset.x - (boxDim2.x + boxDim.x) + collider2.offset.x;
    }
}
void physics::collisionMiss(entity e, object::ecs& container)
{
    container.getComponent<Physics2D>(e).resetCollisions();
}

Camera::Camera(Color color__, float aspect__, Vector3 front__, Vector3 up__, float near__, float far__, float fov__) : backgroundColor(color__), front(front__), up(up__), nearDistance(near__), farDistance(far__), fov(fov__)
{
    projection = mat4::per(math::radians(fov), aspect__, near__, far__);
}

Frustum Camera::getFrustum(const Vector3& position, float aspect)
{
    Frustum frustum;
    float halfVSide = farDistance * std::tan(math::radians(fov) * 0.5f);
    float halfHSide = halfVSide * aspect;
    Vector3 frontMultFar = farDistance * front;
    Vector3 right = front.cross(up);

    frustum.nearPlane = { position + nearDistance * front, front };
    frustum.farPlane = { position + frontMultFar, -front };
    frustum.rightPlane = { position, (frontMultFar - right * halfHSide).cross(up).normalized() };
    frustum.leftPlane = { position, up.cross(frontMultFar + right * halfHSide).normalized() };
    frustum.topPlane = { position, right.cross(frontMultFar - up * halfVSide).normalized() };
    frustum.bottomPlane = { position, (frontMultFar + up * halfVSide).cross(right).normalized() };

    return frustum;
}

Vector3 object::brightness(int32_t value)
{
    switch(value)
    {
        case 1: return Vector3(1.0f, 0.7f, 1.8f);
        case 2: return Vector3(1.0f, 0.35f, 0.44f);
        case 3: return Vector3(1.0f, 0.22f, 0.20f);
        case 4: return Vector3(1.0f, 0.14f, 0.07f);
        case 5: return Vector3(1.0f, 0.09f, 0.032f);
        case 6: return Vector3(1.0f, 0.07f, 0.017f);
        case 7: return Vector3(1.0f, 0.045f, 0.0075f);
        case 8: return Vector3(1.0f, 0.027f, 0.0028f);
        case 9: return Vector3(1.0f, 0.022f, .0019f);
        case 10: return Vector3(1.0f, 0.014f, 0.0007f);
        case 11: return Vector3(1.0f, 0.0014f, 0.000007f);
    };
    return 0;
}


namespace object::fn
{
    uint8_t LOAD, START, UPDATE, LATE_UPDATE, FIXED_UPDATE, RENDER, DESTROY;
}

void object::defaultInsertion(entity e, std::vector<entity>& entities, std::vector<size_t>& map)
{
    map[e] = entities.size();
    entities.push_back(e);
}
void object::insertionSort(std::vector<entity>& entities, std::vector<size_t>& map, Application& app, object::ecs& container, bool (*criteria)(entity, entity, object::ecs& container, void *data))
{
    for(int i=1; i<entities.size(); i++)
    {
        entity key = entities[i];
        int j = i-1;

        while( j >= 0 && criteria(entities[j], key, container, &app))
        {
            map[entities[j+1]] = map[entities[j]];
            entities[j+1] = entities[j];

            j = j-1;
        }
        map[entities[j+1]] = map[key];
        entities[j+1] = key;
    }
}
void object::setFunctionDefinitions(object::ecs& container, const std::vector<uint8_t*>& references)
{
    for(uint8_t *reference : references)
    {
        *reference = container.createSystemFunction();
    }
}

// void object::save(std::string fileName)
// {
//     std::vector<uint8_t> test = std::vector<uint8_t>(100000);
//     std::cout << object::serialize(g_resources.manager, test, 0) << '\n';
//     object::deserialize<object::ecs>(test, 0);
// }

void MeshAddon::append(Model& model, const Transform& parentTransform)
{
    std::vector<Vertex> newVertices = model.data.getVertices();
    Vector3 average = 0, max = 0, min = -std::numeric_limits<float>::lowest();
    for(int i=0; i<additions.size(); i++)
    {
        Vector3 variable = additions[i].transform.position;
        average = ((i+1) * average + variable) / (i+2);

        additions[i].transform.position = (mat4x4)parentTransform.rotation.conjugate() * additions[i].transform.position;

        additions[i].transform.position.x /= parentTransform.scale.x == 0 ? 1:parentTransform.scale.x;
        additions[i].transform.position.y /= parentTransform.scale.y == 0 ? 1:parentTransform.scale.y;
        additions[i].transform.position.z /= parentTransform.scale.z == 0 ? 1:parentTransform.scale.z;
        max = vec3::max(max, additions[i].transform.position);
        min = vec3::min(min, additions[i].transform.position);
        
        additions[i].transform.position = (mat4x4)parentTransform.rotation * additions[i].transform.position;
        model.data.append(newVertices, additions[i].transform.position, additions[i].transform.rotation, parentTransform.rotation, additions[i].uvScale, additions[i].uvOffset);
    }

    model.data.offset += average;
    model.data.dimensions += (max-min) * model.data.dimensions;

    std::string addendumPath = model.data.identifier();
    if(!Mesh::contains(addendumPath))
    {
        model.data.generate();
        addendumPath = model.data.identifier();
    }
    Mesh::set(addendumPath, model.data);
    model.data.refresh();
}

// cannot interpret MUTE, DECR_VOLUME, INCR_VOLUME, CALCULATOR, GLOBAL_2, GLOBAL_3, and GLOBAL_4
void InputManager::initialize(int end, int toggleEnd)
{
    inputs = std::vector<Key>(end+1);
    for(uint32_t i = 0; i <= end; i++)
    {
        inputs[i] = Key{i};
    }

    toggles = std::vector<bool>(toggleEnd);
    for(uint32_t i = 0; i < toggleEnd; i++)
    {
        toggles[i] = false;
    }
}
void InputManager::parse(int32_t input, bool pressed)
{
    Key& key = inputs[input];
    if(pressed && !key.held)
    {
        key.pressed = key.held = true;
        key.released = false;

        heldKeys.insert(key);
    }
    else if(!pressed && key.held)
    {
        key.released = true;
        key.held = false;
    }
}
void InputManager::refresh()
{
    std::vector bufferKeys = std::vector<Key>();
    for(Key key : heldKeys)
    {
        inputs[key.data].pressed = inputs[key.data].released = false;
        if(!inputs[key.data].held)
        {
            bufferKeys.push_back(key);
        }
    }
    for(Key key : bufferKeys)
    {
      heldKeys.erase(key);
    }
    heldKey = '\0';
}
void JoystickManager::initializeAxes(int end)
{
    axes = std::vector<float>(end+1);
    for(uint32_t i = 0; i <= end; i++)
    {
        axes[i] = 0.0f;
    }
}

void Timer::update(float delta, float max)
{
    timer += math::clamp(delta, 0.f, max);
}

void Screen::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Screen::initialize(const DirectionalLight& dirLight__, const Shader& screenShader__, uint32_t width, uint32_t height)
{    
    camera = -1;
    dirLight = dirLight__;
    gamma = 2.2f;

    int maxSamples = getMaximumSamples();

    frameBuffer.initialize();
    subBuffer.initialize();
    depthBuffer.initialize();

    frameBuffer.addTexture("multiTexture", width, height, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, 0, GL_REPEAT, std::min(4, maxSamples));
    subBuffer.addTexture("texture", width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_REPEAT, 0);
    
    frameBuffer.addRenderBuffer("renderBuffer", width, height, std::min(4, maxSamples));

    screenShader = screenShader__;
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

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
    screenShader.remove();
}
void Screen::refreshResolution(const Vector2& res)
{
    frameBuffer.refresh(res.x, res.y, defaultColor.a == 1);
    subBuffer.refresh(res.x, res.y, defaultColor.a == 1);
    resolution = res;
}
void Screen::store()
{
    glViewport(0, 0, resolution.x, resolution.y);
    frameBuffer.bind(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);
}
void Screen::draw()
{
    frameBuffer.bind(GL_READ_FRAMEBUFFER);
    subBuffer.bind(GL_DRAW_FRAMEBUFFER);
    glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    frameBuffer.unbind(GL_FRAMEBUFFER);
    glDisable(GL_DEPTH_TEST);
    glClearColor(defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    screenShader.use();
    screenShader.setFloat("gamma", gamma);
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
uint32_t glInputToJoystickCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_GAMEPAD_BUTTON_A: return controller::BUTTON_A;
        case GLFW_GAMEPAD_BUTTON_B: return controller::BUTTON_B;
        case GLFW_GAMEPAD_BUTTON_X: return controller::BUTTON_X;
        case GLFW_GAMEPAD_BUTTON_Y: return controller::BUTTON_Y;
        case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER: return controller::LEFT_BUMPER;
        case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER: return controller::RIGHT_BUMPER;
        case GLFW_GAMEPAD_BUTTON_BACK: return controller::BACK;
        case GLFW_GAMEPAD_BUTTON_START: return controller::START;
        case GLFW_GAMEPAD_BUTTON_GUIDE: return controller::GUIDE;
        case GLFW_GAMEPAD_BUTTON_LEFT_THUMB: return controller::LEFT_THUMB;
        case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB: return controller::RIGHT_THUMB;
        case GLFW_GAMEPAD_BUTTON_DPAD_UP: return controller::DPAD_UP;
        case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT: return controller::DPAD_RIGHT;
        case GLFW_GAMEPAD_BUTTON_DPAD_DOWN: return controller::DPAD_DOWN;
        case GLFW_GAMEPAD_BUTTON_DPAD_LEFT: return controller::DPAD_LEFT;
    }
    return controller::UNKNOWN;
}
uint32_t glInputToJoystickAxisCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_GAMEPAD_AXIS_LEFT_X: return controller::AXIS_LEFT_X;
        case GLFW_GAMEPAD_AXIS_LEFT_Y: return controller::AXIS_LEFT_Y;
        case GLFW_GAMEPAD_AXIS_RIGHT_X: return controller::AXIS_RIGHT_X;
        case GLFW_GAMEPAD_AXIS_RIGHT_Y: return controller::AXIS_RIGHT_Y;
        case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER: return controller::AXIS_LEFT_TRIGGER;
        case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER: return controller::AXIS_RIGHT_TRIGGER;
    }
    return controller::AXIS_UNKNOWN;
}

void error_callback(int error, const char* msg)
{
	std::string s;
	s = " [" + std::to_string(error) + "] " + msg + '\n';
	std::cout << s << std::endl;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application::keyboard.toggles[key::CAPS_LOCK - key::TOGGLE_START - 1] = ((bool)(mods & GLFW_MOD_CAPS_LOCK));
    Application::keyboard.toggles[key::NUM_LOCK - key::TOGGLE_START - 1] = ((bool)(mods & GLFW_MOD_NUM_LOCK));
    Application::keyboard.toggles[key::SCROLL_LOCK - key::TOGGLE_START - 1] = (key == GLFW_KEY_SCROLL_LOCK);

    if(action != GLFW_RELEASE)
    {
        Application::keyboard.heldKey = glInputToKeyCode(key);
    }

    if(action == GLFW_REPEAT)
    {
        return;
    }

    for(GLenum k = 32; k<=GLFW_KEY_LAST; k++)
    {
        Application::keyboard.parse(glInputToKeyCode(k), glfwGetKey(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        Application::keyboard.parse(glInputToKeyCode(key), false);
    }

    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_F && (mods & GLFW_MOD_CONTROL) && glfwGetWindowMonitor(window) == NULL)
        {
            if(glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
            {
                glfwRestoreWindow(window);
            }
            else
            {
                glfwMaximizeWindow(window);
            }
        }
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    for(GLenum k = 0; k <= GLFW_MOUSE_BUTTON_LAST; k++)
    {
        Application::mouse.parse(glInputToButtonCode(k), glfwGetMouseButton(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        Application::mouse.parse(glInputToButtonCode(button), false);
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Application::cursorPosition = {(float)xpos, (float)ypos};
}
void JoystickManager::joystick_button_callback()
{
    GLFWgamepadstate state;
    if(active && glfwGetGamepadState(id, &state))
    {
        for(GLenum k = 0; k <= GLFW_GAMEPAD_BUTTON_LAST; k++)
        {
            parse(glInputToJoystickCode(k), state.buttons[k]);
        }
        parse(controller::LEFT_TRIGGER, state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.5f);
        parse(controller::RIGHT_TRIGGER, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.5f);
        for(GLenum k = 0; k <= GLFW_GAMEPAD_AXIS_LAST; k++)
        {
            axes[glInputToJoystickAxisCode(k)] = state.axes[k];
        }
    }
}
void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        Application::controllers[jid].active = true;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        Application::controllers[jid].active = false;
    }
}
void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
}

Time::Time()
{
    if(!advanceKey)
    {
        framerates = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
        advanceKey = key::BACKSLASH;
    }
}
void Time::update()
{
    runtime = glfwGetTime();
    lastDeltaTime = deltaTime;
    deltaTime = std::min(runtime - lastFrame, 0.1);
    lastFrame = runtime;
    timer += deltaTime;
    framerates[framerateIndex = (framerateIndex+1)%10] = (1/deltaTime);
}
void Time::beginTimer()
{
    float currentFrame = glfwGetTime();
    lastFrame = currentFrame;
}

bool key::pressed(uint32_t id)
{
    return Application::keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyCode id)
{
    return Application::keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyArray ids)
{
    for(const key::KeyCode &id : ids.get())
    {
        if(Application::keyboard.inputs[id].pressed)
            return true;
    }
    return false;
}
bool key::pressed(const std::vector<key::KeyCode>& ids)
{
    for(const key::KeyCode &id : ids)
    {
        if(Application::keyboard.inputs[id].pressed)
            return true;
    }
    return false;
}
bool key::held(uint32_t id)
{
    return Application::keyboard.inputs[id].held;
}
bool key::held(key::KeyCode id)
{
    return Application::keyboard.inputs[id].held;
}
bool key::held(key::KeyArray ids)
{
    for(const KeyCode &id : ids.get())
    {
        if(Application::keyboard.inputs[id].held)
        {
            return true;
        }
    }
    return false;
}
bool key::held(const std::vector<key::KeyCode>& ids)
{
    for(const KeyCode &id : ids)
    {
        if(Application::keyboard.inputs[id].held)
        {
            return true;
        }
    }
    return false;
}
bool key::released(uint32_t id)
{
    return Application::keyboard.inputs[id].released;
}
bool key::released(key::KeyCode id)
{
    return Application::keyboard.inputs[id].released;
}
bool key::released(key::KeyArray ids)
{
    for(const KeyCode &id : ids.get())
    {
        if(Application::keyboard.inputs[id].released)
            return true;
    }
    return false;
}
bool key::released(const std::vector<key::KeyCode>& ids)
{
    for(const KeyCode &id : ids)
    {
        if(Application::keyboard.inputs[id].released)
            return true;
    }
    return false;
}
bool key::toggled(key::KeyCode id)
{
    return Application::keyboard.toggles[id - key::TOGGLE_START];
}
char key::typed()
{
    KeyCode key = (KeyCode)Application::keyboard.heldKey;
    if(key::held({key::LEFT_CTRL, key::RIGHT_CTRL}) || !ascii_default(key))
    {
        return '\0';
    }

    key = (KeyCode)std::tolower(key);
    if(key::held({key::LEFT_SHIFT, key::RIGHT_SHIFT}))
    {
        if(key >= 'a' && key <='z')
        {
            return !key::toggled(key::CAPS_LOCK) ? key : std::toupper(key);
        }
        
        if(!key::toggled(key::CAPS_LOCK))
        {
            switch(key)
            {
                case '1': return '!';
                case '2': return '@';
                case '3': return '#';
                case '4': return '$';
                case '5': return '%';
                case '6': return '^';
                case '7': return '&';
                case '8': return '*';
                case '9': return '(';
                case '0': return ')';
                case '-': return '_';
                case '=': return '+';
                case '[': return '{';
                case ']': return '}';
                case '\\': return '|';
                case ';': return ':';
                case '\'': return '"';
                case ',': return '<';
                case '.': return '>';
                case '/': return '?';
                case '`': return '~';
            }
        }
    }
    
    if(key::toggled(key::NUM_LOCK))
    {
        switch(key)
        {
            case key::K_0: return '0';
            case key::K_1: return '1';
            case key::K_2: return '2';
            case key::K_3: return '3';
            case key::K_4: return '4';
            case key::K_5: return '5';
            case key::K_6: return '6';
            case key::K_7: return '7';
            case key::K_8: return '8';
            case key::K_9: return '9';
            case key::K_DECIMAL: return '.';
        }
    }
    else if(key::toggled(key::CAPS_LOCK))
    {
        return std::toupper(key);
    }
    
    if((key >= K_0 && key <= K_9) || key == K_DECIMAL)
        return '\0';
    switch(key)
    {
        case key::K_ADD: return '+';
        case key::K_SUBTRACT: return '-';
        case key::K_DIVIDE: return '/';
        case key::K_MULTIPLY: return '*';
        case key::K_ENTER: return '\n';
        case key::K_EQUAL: return '=';
    }
    return key;
}
bool key::ascii_default(KeyCode key)
{
    return (key >= key::BACKSPACE && key <= key::K_9) || key == key::SPACE || key == key::APOSTROPHE || (key >= key::COMMA && key <= key::NINE)
        || (key >= key::SEMICOLON && key <= key::EQUAL) || (key >= key::A && key <= key::RIGHT_BRACKET) || key == key::GRAVE;
}

bool mouse::pressed(mouse::ButtonCode id)
{
    return Application::mouse.inputs[id].pressed;
}
bool mouse::pressed(mouse::ButtonArray ids)
{
    for(const mouse::ButtonCode &id : ids.get())
    {
        if(mouse::pressed(id))
            return true;
    }
    return false;
}
bool mouse::pressed(const std::vector<mouse::ButtonCode>& ids)
{
    for(const mouse::ButtonCode &id : ids)
    {
        if(mouse::pressed(id))
            return true;
    }
    return false;
}
bool mouse::held(mouse::ButtonCode id)
{
    return Application::mouse.inputs[id].held;
}
bool mouse::held(mouse::ButtonArray ids)
{
    for(const mouse::ButtonCode &id : ids.get())
    {
        if(mouse::held(id))
            return true;
    }
    return false;
}
bool mouse::held(const std::vector<mouse::ButtonCode>& ids)
{
    for(const mouse::ButtonCode &id : ids)
    {
        if(mouse::held(id))
            return true;
    }
    return false;
}
bool mouse::released(mouse::ButtonCode id)
{
    return Application::mouse.inputs[id].held;
}
bool mouse::released(mouse::ButtonArray ids)
{
    for(const mouse::ButtonCode &id : ids.get())
    {
        if(mouse::released(id))
            return true;
    }
    return false;
}
bool mouse::released(const std::vector<mouse::ButtonCode>& ids)
{
    for(const mouse::ButtonCode &id : ids)
    {
        if(mouse::released(id))
            return true;
    }
    return false;
}

bool controller::pressed(uint32_t index, controller::ButtonCode id)
{
    JoystickManager& controller =  Application::controllers[index];
    if(controller.active)
        return controller.inputs[id].pressed;
    return false;
}
bool controller::pressed(uint32_t index, controller::ButtonArray ids)
{
    for(const controller::ButtonCode &id : ids.get())
    {
        if(controller::pressed(index, id))
            return true;
    }
    return false;
}
bool controller::pressed(uint32_t index, const std::vector<controller::ButtonCode>& ids)
{
    for(const controller::ButtonCode &id : ids)
    {
        if(controller::pressed(index, id))
            return true;
    }
    return false;
}
bool controller::held(uint32_t index, controller::ButtonCode id)
{
    JoystickManager& controller =  Application::controllers[index];
    if(controller.active)
        return controller.inputs[id].held;
    return false;
}
bool controller::held(uint32_t index, controller::ButtonArray ids)
{
    for(const controller::ButtonCode &id : ids.get())
    {
        if(controller::held(index, id))
            return true;
    }
    return false;
}
bool controller::held(uint32_t index, const std::vector<controller::ButtonCode>& ids)
{
    for(const controller::ButtonCode &id : ids)
    {
        if(controller::held(index, id))
            return true;
    }
    return false;
}
bool controller::released(uint32_t index, controller::ButtonCode id)
{
    JoystickManager& controller =  Application::controllers[index];
    if(controller.active)
        return controller.inputs[id].held;
    return false;
}
bool controller::released(uint32_t index, controller::ButtonArray ids)
{
    for(const controller::ButtonCode &id : ids.get())
    {
        if(controller::released(index, id))
            return true;
    }
    return false;
}
bool controller::released(uint32_t index, const std::vector<controller::ButtonCode>& ids)
{
    for(const controller::ButtonCode &id : ids)
    {
        if(controller::released(index, id))
            return true;
    }
    return false;
}
float controller::axis(uint32_t index, AxisCode id)
{
    return Application::controllers[index].axes[id];
}

bool Window::closing()
{
    return glfwWindowShouldClose((GLFWwindow *)data);
}
bool Window::decorated()
{
    return glfwGetWindowAttrib((GLFWwindow *)data, GLFW_DECORATED);
}
bool Window::enableVSync(bool enable)
{
    vsyncEnabled = enable;
    glfwSwapInterval(enable);
    return enable;
}
bool Window::fullscreened()
{
    return glfwGetWindowMonitor((GLFWwindow *)data) != NULL;
}
bool Window::maximized()
{
    return glfwGetWindowAttrib((GLFWwindow *)data, GLFW_MAXIMIZED);
}
bool Window::throwError()
{
    uint32_t error = glGetError();
    std::string errorMessage;
    switch(error)
    {
        case 0:
            errorMessage = "SUCCESS (GL)";
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
    std::cout << errorMessage << '\n';
    return error;
}
bool Window::throwAudioError()
{
    return false;
}
void Window::centerWindow()
{
    Vector2I res = resolution();
    setPosition(monitorCenter() - Vector2I(0.5*res.x, 0.5*res.y));
}
void Window::close()
{
    glfwSetWindowShouldClose((GLFWwindow *)data, true);
}
void Window::enableDecoration(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)data, GLFW_DECORATED, enable);
}
void Window::enableFloating(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)data, GLFW_FLOATING, enable);
}
void Window::enablePassthrough(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)data, GLFW_MOUSE_PASSTHROUGH, enable);
}
void Window::fullscreen(bool enable, bool vsync)
{
    if(enable && !fullscreened())
    {
        lastPosition = position();
        lastResolution = resolution();
        
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    
        glfwSetWindowMonitor((GLFWwindow *)data, monitor, 0, 0, mode->width, mode->height, vsync ? mode->refreshRate : GLFW_DONT_CARE);
        enableVSync(vsync);
    }
    else if(!enable && fullscreened())
    {
        glfwSetWindowMonitor((GLFWwindow *)data, nullptr, 0, 0, lastResolution.x, lastResolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)data, lastPosition.x, lastPosition.y);
    }
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
    if(Window::fullscreened())
    {
        Vector2I res = Window::resolution();
        Vector2I pos = Window::position();
        glfwSetWindowMonitor((GLFWwindow *)data, nullptr, 0, 0, res.x, res.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)data, pos.x, pos.y);
    }

    glfwMaximizeWindow((GLFWwindow *)data);
}
void Window::minimize()
{
    if(Window::fullscreened())
    {
        Vector2I res = Window::resolution();
        Vector2I pos = Window::position();
        glfwSetWindowMonitor((GLFWwindow *)data, nullptr, 0, 0, res.x, res.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)data, pos.x, pos.y);
    }

    glfwRestoreWindow((GLFWwindow *)data);
}
void Window::poll()
{
    glfwPollEvents();
}
void Window::refresh()
{
    glfwSwapBuffers((GLFWwindow *)data);
}
void Window::setCursor(const Vector2& position)
{
    glfwSetCursorPos((GLFWwindow *)data, position.x, position.y);
}
void Window::setIcon(const char *path)
{
    GLFWimage images[1];

    stbi_set_flip_vertically_on_load(false);
    images[0].pixels = stbi_load((Source::root() + std::string("resources/images/") + path).c_str(), &images[0].width, &images[0].height, 0, 4);
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
void Window::setSize(const Vector2I& size)
{
    glfwSetWindowSize((GLFWwindow *)data, size.x, size.y);
}
void Window::setTitle(const char *title)
{
    glfwSetWindowTitle((GLFWwindow *)data, title);
}
void Window::terminate(void *audioDevice, void *audioContext)
{
    glfwTerminate();
}

float Window::aspectRatioInv()
{
    Vector2I res = Window::resolution();
    return (float)res.x/(float)res.y;
}
float Window::aspectRatio()
{
    Vector2I res = Window::resolution();
    return (float)res.y/(float)res.x;
}
float Window::getOpacity()
{
    return glfwGetWindowOpacity((GLFWwindow *)data);
}

Vector2 Window::fetchCursorPos()
{
    double xPos, yPos;
    glfwGetCursorPos((GLFWwindow *)data, &xPos, &yPos);
    return Vector2((float)xPos, (float)yPos);
}
Vector2 Window::cursorUniformScreenPosition()
{
    Vector2 cursorPosition = Application::cursorPosition;
    Vector2 res = Window::resolution();

    return {((float)cursorPosition.x/(float)res.x)*2 - 1, -((float)cursorPosition.y/(float)res.y)*2 + 1};
}
Vector2 Window::cursorScreenPosition()
{
    return cursorUniformScreenPosition() * Vector2{aspectRatioInv(), 1};
}
Vector2I Window::position()
{
    Vector2I result;
    glfwGetWindowPos((GLFWwindow *)data, &result.x, &result.y);
    return result;
}
Vector2I Window::center()
{
    Vector2I pos = Window::position();
    return Vector2I(monitorCenter() - Vector2I(0.5*pos.x, 0.5*pos.y));
}
Vector2I Window::resolution()
{
    int width, height;
    glfwGetWindowSize((GLFWwindow *)data, &width, &height);
    return Vector2I(width, height);
}
Vector2I Window::monitorCenter()
{
    Vector2I result;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &result.x, &result.y);
    return result * 2;
}
Color Window::getPixelColor(const Vector2& position)
{
    GLubyte pixel[4];
    glReadPixels(position.x, resolution().y - position.y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    Color result = Color();
    result.r = pixel[0];
    result.g = pixel[1];
    result.b = pixel[2];
    result.a = pixel[3];
    return result;
}

Window::Window(std::string name, uint32_t width__, uint32_t height__)
{
    width = width__;
    height = height__;
    screen.defaultColor = color::RED;

    data = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if(data == NULL)
    {
        std::cout << "ERROR :: Failed to create GLFW window." << std::endl;
        return;
    }

    glfwMakeContextCurrent((GLFWwindow *)data);

    glfwShowWindow((GLFWwindow *)data);
    glfwSetWindowPos((GLFWwindow *)data, 0, 25);
    glfwSetFramebufferSizeCallback((GLFWwindow *)data, framebuffer_size_callback);
    glfwSetKeyCallback((GLFWwindow *)data, key_callback);
    glfwSetCursorPosCallback((GLFWwindow *)data, mouse_callback);
    glfwSetMouseButtonCallback((GLFWwindow *)data, mouse_button_callback);
    glfwSetInputMode((GLFWwindow *)data, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    configureGLAD();
    glViewport(0, 0, width, height);

    stbi_set_flip_vertically_on_load(true);
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void initializeECS(object::ecs& manager)
{  
    object::setFunctionDefinitions(manager, {&object::fn::LOAD, &object::fn::START, &object::fn::UPDATE, &object::fn::LATE_UPDATE, &object::fn::FIXED_UPDATE, &object::fn::RENDER, &object::fn::DESTROY});
    
    auto& pointlights = manager.createSystem<PointLightManager, PointLight, Transform>({}, 3);
    pointlights.setFunction(object::fn::UPDATE, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        std::vector<entity>& entities = container.entities<PointLightManager>();

        Shader& shader = Shader::get("object_shader");
        shader.use();
        shader.setInt("totalPointLights", entities.size());

        uint32_t iterator = 0;
        for(entity e : entities)
        {
            Transform& transform = container.getComponent<Transform>(e);
            PointLight& light = container.getComponent<PointLight>(e);

            std::string name = "pointLights[" + std::to_string(iterator) + "]";
            shader.setVec3(name + ".position", transform.position);
            shader.setVec4(name + ".color", light.color);
            shader.setFloat(name + ".strength", light.strength);
            
            shader.setFloat(name + ".constant", light.constant);
            shader.setFloat(name + ".linear", light.linear);
            shader.setFloat(name + ".quadratic", light.quadratic);
            iterator++;
        }
    });

    auto& spotlights = manager.createSystem<SpotLightManager, SpotLight, Transform>({}, 3);
    spotlights.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        const std::vector<entity>& entities = container.entities<SpotLightManager>();
        Shader& shader = Shader::get("object_shader");
        shader.use();
        shader.setInt("totalSpotLights", entities.size());

        uint32_t iterator = 0;
        for(entity e : entities)
        {
            Transform& transform = container.getComponent<Transform>(e);
            SpotLight& light = container.getComponent<SpotLight>(e);

            std::string name = "spotLights[" + std::to_string(iterator) + "]";
            shader.setVec3(name + ".position", transform.position);
            shader.setVec3(name + ".direction", light.direction);
            shader.setVec4(name + ".color", light.color);
            shader.setFloat(name + ".strength", light.strength);
            
            shader.setFloat(name + ".constant", light.constant);
            shader.setFloat(name + ".linear", light.linear);
            shader.setFloat(name + ".quadratic", light.quadratic);

            shader.setFloat(name + ".cutOff", light.cutoff);
            shader.setFloat(name + ".outerCutOff", light.outerCutOff);
            iterator++;
        }
    });

    auto& physics = manager.createSystem<PhysicsManager, Physics2D, Transform>({}, 6);
    physics.setFunction(object::fn::START, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<PhysicsManager>())
        {
            Physics2D& physics = container.getComponent<Physics2D>(e);
            physics.delta = physics.lastDelta = container.getComponent<Transform>(e).position;
            physics.time.begin();
        }
    });
    physics.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<PhysicsManager>())
        {
            Vector3& position = container.getComponent<Transform>(e).position;
            Physics2D& physics = container.getComponent<Physics2D>(e);

            physics.time.update((Application::data(data).time).deltaTime, physics.maxDeltaTime);
            float time = physics.time.interval;

            while(physics.time.timer > physics.time.interval)
            {
                Vector3 acceleration = physics.acceleration();
                Vector3 deltaStep = vec3::roundTo(physics.lastDelta - physics.delta, 5);
                if((physics.collisions[physics::LEFT] && deltaStep.x >= 0) || (physics.collisions[physics::RIGHT] && deltaStep.x <= 0))
                {
                    physics.delta.x = position.x;
                }
                if((physics.collisions[physics::DOWN] && deltaStep.y >= 0) || (physics.collisions[physics::UP] && deltaStep.y <= 0))
                {
                    physics.delta.y = position.y;
                }
                
                physics.lastDelta = physics.delta;

                if(physics.dragDirection && (physics.collisions[physics::UP] || physics.collisions[physics::DOWN]) && math::sign0(physics.force.x) != math::sign0(physics.velocity.x))
                {
                    physics.velocity.x = physics.velocity.x / (1 + physics.drag.x * time);
                }
                else if(!physics.dragDirection && (physics.collisions[physics::RIGHT] || physics.collisions[physics::LEFT]) && math::sign0(physics.force.y) != math::sign0(physics.velocity.y))
                {
                    physics.velocity.y = physics.velocity.y / (1 + physics.drag.y * time);
                }

                if(physics.terminal != 0)
                {
                    physics.velocity = vec3::min(vec3::abs(physics.velocity + acceleration * time), physics.terminal) * vec3::sign(physics.velocity + acceleration * time);
                }
                else
                {
                    physics.velocity += acceleration * time;
                }
                
                physics.velocity += physics.impulse;

                physics.delta += Vector3(physics.velocity * time + acceleration * 0.5f * time * time);
                physics.time.reset();
                physics.resetImpulse();
            }

            position = vec3::lerp(physics.lastDelta, physics.delta, physics.time.timer / physics.time.interval);
            physics.resetForce();
        }
    });

    auto& aabb = manager.createSystem<AABBHandler, AABB, BoxCollider, Physics2D, Transform>({}, 9);
    aabb.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<AABBHandler>())
        {
            if(!container.containsComponent<Physics2D>(e))
                continue;

            BoxCollider& collider = container.getComponent<BoxCollider>(e);
            Transform& transform = container.getComponent<Transform>(e);
            bool triggered = false;
            if(collider.mobile)
            {
                Vector3 position = transform.position + collider.offset;
                Vector3 boxDim = (transform.scale * collider.scale + vec3::abs(position - transform.storedPosition - collider.offset)) * 0.5f;
    
                for(entity compare : container.entities<AABBHandler>())
                {
                    if(compare == e)
                        continue;
    
                    BoxCollider& collider2 = container.getComponent<BoxCollider>(compare);
                    Transform& transform2 = container.getComponent<Transform>(compare);
    
                    Vector3 position2 = transform2.position + collider2.offset;
                    Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;
    
                    int precision = 6;
                    Vector3 positive = vec3::roundTo(position + boxDim, precision), negative = vec3::roundTo(position - boxDim, precision);
                    Vector3 positive2 = vec3::roundTo(position2 + boxDim2, precision), negative2 = vec3::roundTo(position2 - boxDim2, precision);
                    if(positive.x >= negative2.x && negative.x <= positive2.x && positive.y >= negative2.y && negative.y <= positive2.y && positive.z >= negative2.z && negative.z <= positive2.z)
                    {                     
                        bool edge = (positive.x == negative2.x || negative.x == positive2.x || positive.y == negative2.y || negative.y == positive2.y);
                        physics::collisionTrigger(e, compare, edge, triggered, container);
                        // collider2.trigger(compare, e, edge, triggered);
                        collider.enter = collider2.enter = triggered = true;
                    }
                }
                if(!triggered)
                {
                    physics::collisionMiss(e, container);
                    collider.enter = false;
                }
            }
        }
    });

    auto& aabb2D = manager.createSystem<AABB2DHandler, AABB2D, BoxCollider, Transform>({}, 9);
    aabb2D.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<AABB2DHandler>())
        {
            if(!container.containsComponent<Physics2D>(e))
                continue;

            BoxCollider& collider = container.getComponent<BoxCollider>(e);
            Transform& transform = container.getComponent<Transform>(e);

            bool triggered = false;
            Vector3 position = transform.position + collider.offset;
            Vector3 boxDim = (transform.scale * collider.scale + vec3::abs(position - transform.storedPosition - collider.offset)) * 0.5f;
    
            for(entity compare : container.entities<AABB2DHandler>())
            {
                if(compare == e)
                    continue;

                BoxCollider& collider2 = container.getComponent<BoxCollider>(compare);
                Transform& transform2 = container.getComponent<Transform>(compare);

                Vector3 position2 = transform2.position + collider2.offset;
                Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;

                int precision = 6;
                Vector3 positive = vec3::roundTo(position + boxDim, precision), negative = vec3::roundTo(position - boxDim, precision);
                Vector3 positive2 = vec3::roundTo(position2 + boxDim2, precision), negative2 = vec3::roundTo(position2 - boxDim2, precision);
                if(positive.x >= negative2.x && negative.x <= positive2.x && positive.y >= negative2.y && negative.y <= positive2.y)
                {          
                    bool edge = (positive.x == negative2.x || negative.x == positive2.x || positive.y == negative2.y || negative.y == positive2.y);
                    physics::collisionTrigger(e, compare, edge, triggered, container);
                    // collider2.trigger(compare, e, edge, triggered);
                    collider.enter = collider2.enter = triggered = true;
                }
            }
            if(!triggered)
            {
                physics::collisionMiss(e, container);
                collider.enter = false;
            }
        }
    });

    auto& billboards = manager.createSystem<BillboardManager, Billboard, Transform>({}, 21);
    billboards.setFunction(object::fn::LATE_UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<BillboardManager>())
        {
            Billboard& billboard = container.getComponent<Billboard>(e);
            Transform& transform = container.getComponent<Transform>(e);
            transform.rotation = Quaternion(mat4::lookAt(transform.position, ((container.getComponent<Transform>(billboard.target).position - transform.position) * billboard.limit).normalized(), vec3::up)).inverted();
        }
    });

    auto& animations = manager.createSystem<AnimationManager, Animator2D, Model>({}, 24);
    animations.setFunction(object::fn::FIXED_UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<AnimationManager>())
        {
            Animator2D animator = container.getComponent<Animator2D>(e);
            AnimationState state = animator.getCurrent();
            Animation2D animation = state.getCurrent();
            Model model = container.getComponent<Model>(e);
            Texture current = animation.current();
            if(current != Texture() && model.texture != current)
            {
                model.texture = current;
                container.setComponent<Model>(e, model);
            }

            animation.currentFrame = (animation.currentFrame + 1) % animation.frames.size();
            state.setState(animation);
            animator.setState(state);
            container.setComponent<Animator2D>(e, animator);
        }
    });

    auto& animationsUV = manager.createSystem<AnimationUVManager, AnimatorUV, Model>({}, 24);
    animationsUV.setFunction(object::fn::FIXED_UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<AnimationUVManager>())
        {
            AnimatorUV animator = container.getComponent<AnimatorUV>(e);
            AnimationStateUV state = animator.getCurrent();
            AnimationUV animation = state.getCurrent();
            Model model = container.getComponent<Model>(e);
            Texture current = animation.texture;
            
            if(model.texture != current || (animation.update() && (animation.type != AnimationUV::STOP || animation.currentFrame != animation.length-1)))
            {
                model.texture = current;
                model.scale = Vector2(1.0f / animation.bounds.x, 1.0f / animation.bounds.y);
                model.offset = Vector2((animation.currentFrame % animation.bounds.x) / (float)animation.bounds.x, (animation.bounds.y - (int)(animation.currentFrame / animation.bounds.x) - 1) / (float)animation.bounds.y);
                container.setComponent<Model>(e, model);
                animation.currentFrame = (animation.currentFrame + 1) % animation.length;
            }

            state.setState(animation);
            animator.setState(state);
            container.setComponent<AnimatorUV>(e, animator);
        }
    });

    auto& meshes = manager.createSystem<MeshManager, MeshAddon, Transform, Model>({}, 27);
    meshes.setFunction(object::fn::START, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<MeshManager>())
        {
            Model model = container.getComponent<Model>(e);
            MeshAddon addon = container.getComponent<MeshAddon>(e);

            addon.append(model, container.getComponent<Transform>(e));

            container.setComponent<Model>(e, model);
        }
    });

    auto& simpleRendering = manager.createSystem<SimpleRenderer, Transform, Model, SimpleShader>({}, 32);
    simpleRendering.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        uint32_t cam = win.screen.camera;
        if(cam == -1)
            return;

        Camera& camera = container.getComponent<Camera>(cam);
        Transform& cameraTransform = container.getComponent<Transform>(cam);
        Frustum frustum = camera.getFrustum(cameraTransform.position, win.aspectRatioInv());

        Shader& shader = Shader::get("simple_shader");
        shader.use();

        for(entity e : container.entities<SimpleRenderer>())
        {
            Model model = container.getComponent<Model>(e);
            Transform& transform = container.getComponent<Transform>(e);
            if(!frustum.contains(transform.position + model.data.offset, model.data.dimensions.length() * vec3::high(transform.scale)))
                continue;
                
            SimpleShader& mat = container.getComponent<SimpleShader>(e);

            shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
            shader.setMat4("view", camera.view.matrix, true);
            shader.setMat4("projection", camera.projection.matrix, true);
            shader.setVec3("scale", transform.scale);
            shader.setVec4("objColor", mat.color);
            shader.setVec2("offset", model.offset);
            shader.setVec2("uvScale", model.scale);
            shader.setBool("flip", mat.flip);

            model.render();
        }
    });

    auto& advancedRendering = manager.createSystem<AdvancedRenderer, Transform, Model, AdvancedShader>({}, 32);
    advancedRendering.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        uint32_t cam = win.screen.camera;
        if(cam == -1)
            return;
        
        Camera& camera = container.getComponent<Camera>(cam);
        Transform& cameraTransform = container.getComponent<Transform>(cam);
        Frustum frustum = camera.getFrustum(cameraTransform.position, win.aspectRatioInv());

        Shader& shader = Shader::get("object_shader");
        shader.use();

        for(entity e : container.entities<AdvancedRenderer>())
        {
            Model model = container.getComponent<Model>(e);
            Transform& transform = container.getComponent<Transform>(e);
            if(!frustum.contains(transform.position + model.data.offset, model.data.dimensions.length() * vec3::high(transform.scale)))
            {
                continue;
            }

            DirectionalLight& light = win.screen.dirLight;
            AdvancedShader& mat = container.getComponent<AdvancedShader>(e);
    
            shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
            shader.setMat4("view", camera.view.matrix, true);
            shader.setMat4("projection", camera.projection.matrix, true);
            shader.setVec3("scale", transform.scale);
            
            shader.setVec3("dirLight.direction", light.direction);
            shader.setVec4("dirLight.color", light.color);
            shader.setFloat("dirLight.strength", light.strength);

            shader.setVec3("viewPos", cameraTransform.position);
            shader.setVec4("objColor", mat.color);

            shader.setFloat("material.shininess", mat.shine);
            shader.setFloat("material.ambientStrength", mat.ambient);
            shader.setFloat("material.diffuseStrength", mat.diffuse);
            shader.setFloat("material.specularStrength", mat.specular);
            model.render();
        }
    });

    auto& complexRendering = manager.createSystem<ComplexRenderer, Transform, Model, ComplexShader>({}, 33);
    complexRendering.setFunction(object::fn::LOAD, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        ComplexRenderer& renderer = system.getInstance<ComplexRenderer>();
        renderer.update = true;
        std::vector<int *> values = {&renderer.model, &renderer.view, &renderer.projection, &renderer.scale, &renderer.color, &renderer.offset, &renderer.uvScale, &renderer.flip};
        Shader::get("simple_shader").setUniforms({"model", "view", "projection", "scale", "objColor", "offset", "uvScale", "flip"}, values);
    });
    complexRendering.setFunction(object::fn::LATE_UPDATE, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        uint32_t& cam = (Application::data(data).window()).screen.camera;
        ComplexRenderer& rendering = system.getInstance<ComplexRenderer>();
        for(entity e : container.entities<ComplexRenderer>())
        {
            Transform& transform = container.getComponent<Transform>(e);
            if(cam != -1 && math::abs((transform.position - transform.storedPosition).dot(container.getComponent<Camera>(cam).front)) > 0.0001f)
            {
                rendering.update = true;
                break;
            }
        }
    });
    complexRendering.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        glDepthMask(GL_FALSE);

        Window& win = Application::data(data).window();
        uint32_t& cam = win.screen.camera;
        if(cam == -1)
            return;

        Camera& camera = container.getComponent<Camera>(cam);
        Transform& cameraTransform = container.getComponent<Transform>(cam);
        Frustum frustum = camera.getFrustum(cameraTransform.position, win.aspectRatioInv());
        
        ComplexRenderer& rendering = system.getInstance<ComplexRenderer>();
        std::vector<entity>& entities = container.entities<ComplexRenderer>();
        if(rendering.update || vec3::abs(cameraTransform.rotation - cameraTransform.lastRotation).length2() > 0.0001f)
        {
            object::insertionSort(entities, container.getMapping<ComplexRenderer>(), (*(Application *)data), container, [](entity one, entity two, object::ecs& container, void *data)
            {
                entity cam = (Application::data(data).window()).screen.camera;
                Camera& camera = container.getComponent<Camera>(cam);

                return camera.front.dot(container.getComponent<Transform>(one).position) < camera.front.dot(container.getComponent<Transform>(two).position);
            });
            rendering.update = false;
        }

        Shader& shdr = Shader::get("simple_shader");

        shdr.use();


        for(entity e : entities)
        {
            Model model = container.getComponent<Model>(e);
            Transform& transform = container.getComponent<Transform>(e);
            if(!frustum.contains(transform.position + model.data.offset, model.data.dimensions.length() * vec3::high(transform.scale)))
            {
                continue;
            }

            ComplexShader& mat = container.getComponent<ComplexShader>(e);

            if(container.containsComponent<Fade>(e))
            {
                Shader& temp = Shader::get("fade_shader");
                temp.use();

                Fade& fade = container.getComponent<Fade>(e);

                temp.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
                temp.setMat4("view", camera.view.matrix, true);
                temp.setMat4("projection", camera.projection.matrix, true);
                temp.setVec3("scale", transform.scale);
                temp.setVec4("objColor", mat.color);
                temp.setVec2("offset", model.offset);
                temp.setVec2("uvScale", model.scale);
                temp.setBool("flip", mat.flip);
                temp.setFloat("rate", fade.rate);
                temp.setFloat("distance", fade.distance);

                model.render();

                shdr.use();
            }
            else
            {
                shdr.setMat4(rendering.model, (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
                shdr.setMat4(rendering.view, camera.view.matrix, true);
                shdr.setMat4(rendering.projection, camera.projection.matrix, true);
                shdr.setVec3(rendering.scale, transform.scale);
                shdr.setVec4(rendering.color, mat.color);
                shdr.setVec2(rendering.offset, model.offset);
                shdr.setVec2(rendering.uvScale, model.scale);
                shdr.setBool(rendering.flip, mat.flip);

                model.render();
            }
        }
        glDepthMask(GL_TRUE);
    });

    auto& uiRendering = manager.createSystem<UIRenderer, Rect, Sprite, SimpleShader>({}, 34);
    uiRendering.setFunction(object::fn::RENDER, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        Vector2I res = win.resolution();
        uint32_t cam = win.screen.camera;
        if(cam == -1)
            return;

        Camera& camera = container.getComponent<Camera>(cam);
        Shader& shader = Shader::get("ui_shader");
        shader.use();

        glDisable(GL_DEPTH_TEST);
        for(entity e : container.entities<UIRenderer>())
        {
            Sprite sprite = container.getComponent<Sprite>(e);
            Rect& rect = container.getComponent<Rect>(e);
            SimpleShader& mat = container.getComponent<SimpleShader>(e);

            shader.setFloat("aspect", rect.useAspect() ? win.aspectRatio():1);
            shader.setVec2("position", rect.relativePosition(res));
            shader.setVec2("scale", rect.adjustedScale(res));
            shader.setMat4("model", (mat4x4(1) * (mat4x4)rect.rotation).matrix, false);
            shader.setVec4("objColor", mat.color);

            sprite.render();
        }
        glEnable(GL_DEPTH_TEST);
    });

    auto& textRendering = manager.createSystem<TextRenderer, Text, Rect>({}, 36);
    textRendering.setFunction(object::fn::RENDER, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        Vector2I res = win.resolution();
        uint32_t cam = win.screen.camera;
        if(cam == -1)
            return;

        Camera& camera = container.getComponent<Camera>(cam);
        Shader& shader = Shader::get("text_shader");
        shader.use();

        glDisable(GL_DEPTH_TEST);
        for(entity e : container.entities<TextRenderer>())
        {
            Text text = container.getComponent<Text>(e);
            Rect& rect = container.getComponent<Rect>(e);
            TextShader& mat = container.getComponent<TextShader>(e);

            // shader.setFloat("aspect", rect.useAspect() ? win.aspectRatio():1);
            // shader.setVec2("position", rect.relativePosition(res));
            // shader.setVec2("scale", rect.adjustedScale(res));
            // shader.setMat4("model", (mat4x4(1) * (mat4x4)rect.rotation).matrix, false);
            // shader.setVec4("objColor", mat.color);

            // sprite.render();
        }
        glEnable(GL_DEPTH_TEST);
    });

    auto& cameras = manager.createSystem<CameraManager, Camera, Transform>({}, 36);
    cameras.setFunction(object::fn::LOAD, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        for(entity e : container.entities<CameraManager>())
        {
            Camera& camera = container.getComponent<Camera>(e);
            camera.view = mat4::lookAt(container.getComponent<Transform>(e).position, -camera.front, vec3::up);

            if(win.screen.resolutionUpdated)
            {
                camera.projection = mat4::inter(math::radians(45.0f), 2.5f, win.aspectRatioInv(), camera.nearDistance, camera.farDistance, 1);
            }
        }
    });
    cameras.setFunction(object::fn::LATE_UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        for(entity e : container.entities<CameraManager>())
        {
            Camera& camera = container.getComponent<Camera>(e);

            Vector3 right = camera.front.cross(vec3::up).normalized();
            camera.up = right.cross(camera.front).normalized();
            camera.view = mat4::lookAt(container.getComponent<Transform>(e).position, -camera.front, vec3::up);

            if(win.screen.resolutionUpdated)
            {
                camera.projection = mat4::inter(math::radians(camera.fov), 2.5f, win.aspectRatioInv(), camera.nearDistance, camera.farDistance, 1);
            }
        }
    });
    cameras.setFunction(object::fn::RENDER, []
    (object::ecs& container, object::ecs::system &system, void *data)
    {
        Window& win = Application::data(data).window();
        uint32_t& cam = win.screen.camera;
        if(cam != -1)
        {
            Camera& camera = container.getComponent<Camera>(cam);
            Transform& transform = container.getComponent<Transform>(cam);

            Vector3& position = transform.position;
            transform.lastRotation = transform.rotation;
        }
    });
    cameras.setFunction(object::fn::DESTROY, []
    (object::ecs& container, object::ecs::system &system, void *data)
    {
        Application::data(data).window().setCamera(-1);
    });

    auto& graphics = manager.createSystem<GraphicsManager, Transform, Model>({}, 36);
    graphics.setFunction(object::fn::START, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<GraphicsManager>())
        {
            container.getComponent<Model>(e).refresh();
        }
    });
    graphics.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<GraphicsManager>())
        {
            Transform& transform = container.getComponent<Transform>(e);
            transform.storedPosition = transform.position;
        }
    });

    auto& uis = manager.createSystem<UIManager, Rect, Sprite>({}, 36);
    uis.setFunction(object::fn::START, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<UIManager>())
        {
            container.getComponent<Sprite>(e).refresh();
        }
    });
    uis.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system, void *data)
    {
        for(entity e : container.entities<UIManager>())
        {
            Rect& transform = container.getComponent<Rect>(e);
            transform.storedPosition = transform.position;
        }
    });

    auto& buttons = manager.createSystem<ButtonManager, Button, Rect>({}, 36);
    buttons.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system, void *data)
    {
        Application& app = Application::data(data);
        Window& win = app.window();
        for(entity e : container.entities<ButtonManager>())
        {
            Rect& rect = container.getComponent<Rect>(e);
            Button& button = container.getComponent<Button>(e);\
            if(mouse::pressed(mouse::LEFT))
            {
                if(rect.contains(win.cursorScreenPosition(), win.resolution()))
                {
                    app.runEvent(button.event, &e);
                }
            }
        }
    });
}

Scene::Scene(const object::ecs& container__)
{
    container = container__;
    pause = container.createSystemToggle();
    container.addToToggle<AudioManager>(pause, object::fn::UPDATE);
    container.addToToggle<PointLightManager>(pause, object::fn::UPDATE);
    container.addToToggle<SpotLightManager>(pause, object::fn::UPDATE);
    container.addToToggle<PhysicsManager>(pause, object::fn::UPDATE);
    container.addToToggle<AABBHandler>(pause, object::fn::UPDATE);
    container.addToToggle<AABB2DHandler>(pause, object::fn::UPDATE);
    container.addToToggle<BillboardManager>(pause, object::fn::LATE_UPDATE);
    container.addToToggle<AnimationManager>(pause, object::fn::FIXED_UPDATE);
    container.addToToggle<AnimationUVManager>(pause, object::fn::FIXED_UPDATE);
}

ProjectManager::ProjectManager(std::vector<Window>& windows, const std::string& name, uint32_t width, uint32_t height)
{
    Source::initialize();

    if(windows.size() > 0)
        return;
    
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
    glfwSetJoystickCallback(joystick_callback);

    windows.push_back(Window(name, width, height));
    Window& window = windows.back();

    Texture::load("default", {Color8(255, 255, 255, 255)}, 1, 1, Texture::RGBA, Texture::PNG);
    Mesh::load("square", shape::square());
    Mesh::load("cube", shape::cube());
    file::loadFilesInDirectory(Source::root() + Source::font(), Font::load);

    std::vector<std::string> shaderConfig = file::loadFileToStringVector(Source::config() + "shader.config");
    for(const auto& line : shaderConfig)
    {
        int index = line.find(':');
        if(index != std::string::npos)
        {
            int comma = line.find(',');
            Shader::load(line.substr(0, index), Shader(line.substr(index+2, comma - index-2), line.substr(comma+2)));
        }
    }

    Shader::get("object_shader").use();
    Shader::get("object_shader").setInt("material.diffuse", 0);
    
    Shader::get("simple_shader").use();
    Shader::get("simple_shader").setInt("material.texture", 0);
    
    Shader::get("ui_shader").use();
    Shader::get("ui_shader").setInt("text", 0);
    
    (window.screen.quad = Mesh::get("square")).refresh();
    Application::keyboard.initialize(key::LAST, 3);
    Application::mouse.initialize(mouse::LAST);
    for(int i=GLFW_JOYSTICK_1; i<=GLFW_JOYSTICK_LAST; i++)
    {
        JoystickManager& manager = Application::controllers[i] = JoystickManager();
        manager.initialize(controller::LAST);
        manager.initializeAxes(controller::AXIS_LAST);
        if((int)glfwJoystickPresent(i))
        {
            manager.id = i;
            if (glfwJoystickIsGamepad(i))
            {
                manager.active = true;
                Application::currentJoystick = i;
            }
            else
            {
                int count;
                const float* axes = glfwGetJoystickAxes(i, &count);
                std::cout << "Axes: " << count << '\n';
                for(int j=0; j<count; j++)
                {
                    std::cout << axes[j] << '\n';
                }

                const unsigned char* buttons = glfwGetJoystickButtons(i, &count);
                std::cout << "\nButtons: " << count << '\n';
                for(int j=0; j<count; j++)
                {
                    std::cout << (int)buttons[j] << '\n';
                }

                const unsigned char* hats = glfwGetJoystickHats(i, &count);
                std::cout << "\nHats: " << " : " << count << '\n';
                for(int j=0; j<count; j++)
                {
                    std::cout << (int)hats[j] << '\n';
                }
            }
        }
    }
}

Application::Application(const std::string& name, uint32_t width, uint32_t height)
{
    manager = ProjectManager(windows, name, width, height);
    currentScene = createScene();
}

uint32_t Application::createScene()
{
    uint32_t result = scenes.size();
    object::ecs ecs;
    initializeECS(ecs);
    scenes.push_back(Scene(ecs));
    return result;
}

Scene& Application::getScene()
{
    if(currentScene == -1)
        std::cout << "ERROR :: Current scene has not been set.\n";
    return scenes[currentScene];
}

void Application::updateScene()
{
    if(sceneChanged)
    {
        if(lastScene != -1)
        {
            object::ecs& last = scenes[lastScene].container;
            last.run(object::fn::DESTROY, this);
            last.clearEntities();
        }

        object::ecs& current = scenes[currentScene].container;
        current.run(object::fn::LOAD, this);
        current.run(object::fn::START, this);

        sceneChanged = false;
    }
}

Application& Application::data(void *data)
{
    return *((Application *)data);
}
