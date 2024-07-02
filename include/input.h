#ifndef INPUT_H
#define INPUT_H

#include <set>
#include <stdint.h>
#include <string>
#include <vector>

uint32_t glInputToKeyCode(uint32_t input);
uint32_t glInputToButtonCode(uint32_t input);

namespace key
{
    enum KeyCode
    {
        NIL, UP, DOWN, LEFT, RIGHT, PAGE_UP, PAGE_DOWN, END, BACKSPACE, TAB, ENTER, K_ENTER, K_DECIMAL, K_ADD, K_SUBTRACT, K_MULTIPLY, K_DIVIDE, 
        K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, 
        ESCAPE, LEFT_SHIFT, RIGHT_SHIFT, LEFT_CTRL, RIGHT_CTRL, SPACE, INSERT, PRINT_SCREEN, LEFT_ALT, RIGHT_ALT, LEFT_SUPER, RIGHT_SUPER, APOSTROPHE, 
        TOGGLE_START, MUTE = TOGGLE_START, NUM_LOCK, CAPS_LOCK, SCROLL_LOCK, COMMA, MINUS, PERIOD, SLASH, 
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, 
        PAUSE, SEMICOLON, K_EQUAL, EQUAL, MENU, DECR_VOLUME, INCR_VOLUME, 
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, HOME, CALCULATOR, GRAVE, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, 
        GLOBAL_0, GLOBAL_1, GLOBAL_2, GLOBAL_3, DEL, UNKNOWN, LAST = UNKNOWN
    };

    using KeyArray = std::vector<KeyCode>;

    bool pressed(uint32_t id);
    bool pressed(KeyCode id);
    bool pressed(KeyArray ids);
    bool held(uint32_t id);
    bool held(KeyCode id);
    bool held(KeyArray ids);
    bool released(uint32_t id);
    bool released(KeyCode id);
    bool released(KeyArray ids);
    bool toggled(KeyCode id);

    char typed();
    bool ascii_default(KeyCode key);
}

namespace mouse
{
    enum ButtonCode
    {
        NIL, BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, LEFT = BUTTON_0,RIGHT = BUTTON_1, MIDDLE = BUTTON_2, UNKNOWN, LAST = UNKNOWN
    };

    bool pressed(ButtonCode id);
    bool held(ButtonCode id);
    bool released(ButtonCode id);
}

// Key (struct): stores whether the stored key data (corresponding to GLFW key values) is being pressed, held, or released
struct Key
{
    uint32_t data;
    bool pressed, held, released;

    operator std::string()
    {
        return "Pressed: "+std::to_string(pressed) + ", Held: "+std::to_string(held) + ", Released: "+std::to_string(released);
    }
    bool operator <(const Key& key) const
    {
        return data < key.data;
    }
};

// InputManager (struct): stores keys and regulates whether they are being pressed, held, or released
struct InputManager
{
    uint8_t heldKey = 0;

    std::vector<bool> toggles;
    std::vector<Key> inputs;
    std::set<Key> heldKeys;

    void initialize(int end, int toggleEnd = 0);                // initialized key values from 0 to 'end'
    void parse(int32_t input, bool pressed); // determines whether the input is being pressed, held, or released
    void refresh();                          // clears pressed and released keys after one frame, and erases held keys if they are released
};

// Button (struct): triggers its 'trigger' function when its attached Rect component is clicked on
struct Button
{
    void(*trigger)(uint32_t entity);
    std::vector<key::KeyCode> keyInputs;
    std::vector<mouse::ButtonCode> buttonInputs;

    Button() : trigger([](uint32_t entity){}) {};
    Button(void(*trigger__)(uint32_t entity)) : trigger(trigger__) {};
    Button(void(*trigger__)(uint32_t entity), key::KeyCode key) : trigger(trigger__)
    {
        keyInputs.push_back(key);
    }
    Button(void(*trigger__)(uint32_t entity), char key) : trigger(trigger__)
    {
        keyInputs.push_back((key::KeyCode)key);
    }
    Button(void(*trigger__)(uint32_t entity), mouse::ButtonCode button) : trigger(trigger__)
    {
        buttonInputs.push_back(button);
    }
    
    Button(void(*trigger__)(uint32_t entity), key::KeyCode key, mouse::ButtonCode button) : trigger(trigger__)
    {
        keyInputs.push_back(key);
        buttonInputs.push_back(button);
    }
    Button(void(*trigger__)(uint32_t entity), char key, mouse::ButtonCode button) : trigger(trigger__)
    {
        keyInputs.push_back((key::KeyCode)key);
        buttonInputs.push_back(button);
    }
    Button(void(*trigger__)(uint32_t entity), mouse::ButtonCode button, key::KeyCode key) : trigger(trigger__)
    {
        buttonInputs.push_back(button);
        keyInputs.push_back(key);
    }
    Button(void(*trigger__)(uint32_t entity), mouse::ButtonCode button, char key) : trigger(trigger__)
    {
        keyInputs.push_back((key::KeyCode)key);
        buttonInputs.push_back(button);
    }

    Button(void(*trigger__)(uint32_t entity), const std::vector<key::KeyCode>& keyInputs__) : trigger(trigger__), keyInputs(keyInputs__) {}
    Button(void(*trigger__)(uint32_t entity), const std::vector<mouse::ButtonCode>& buttonInputs__) : trigger(trigger__), buttonInputs(buttonInputs__) {}

    Button(void(*trigger__)(uint32_t entity), const std::vector<key::KeyCode>& keyInputs__, const std::vector<mouse::ButtonCode>& buttonInputs__) : trigger(trigger__), keyInputs(keyInputs__), buttonInputs(buttonInputs__) {}
    Button(void(*trigger__)(uint32_t entity), const std::vector<mouse::ButtonCode>& buttonInputs__, const std::vector<key::KeyCode>& keyInputs__) : trigger(trigger__), keyInputs(keyInputs__), buttonInputs(buttonInputs__) {}
};

#endif