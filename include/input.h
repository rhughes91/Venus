#pragma once

#include <bit>
#include <limits>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

#include <iostream>

uint32_t glInputToKeyCode(uint32_t input);
uint32_t glInputToButtonCode(uint32_t input);
uint32_t glInputToJoystickCode(uint32_t input);


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

    struct KeyArray
    {
        KeyArray(const std::vector<KeyCode>& inputs)
        {
            one = two = 0;
            for(KeyCode code : inputs)
            {
                if(code < 64)
                {
                    one |= ((uint64_t)1) << code;
                }
                else
                {
                    two |= ((uint64_t)1) << (code % 64);
                }
            }
        }

        std::vector<KeyCode> get() const
        {
            std::vector<KeyCode> result = {};
            uint64_t temp = one;
            while(temp)
            {
                
                KeyCode code = (KeyCode)(std::countr_zero(temp));
                temp &= ~((uint64_t)1 << code);
                result.push_back(code);
            }
            temp = two;
            while(temp)
            {
                KeyCode code = (KeyCode)(std::countr_zero(temp));
                temp &= ~((uint64_t)1 << code);
                result.push_back((KeyCode)(code+64));
            }
            return result;
        }

        private:
            uint64_t one, two;
    };

    bool pressed(uint32_t id);
    bool pressed(KeyCode id);
    bool pressed(KeyArray ids);
    bool pressed(const std::vector<KeyCode>& ids);
    bool held(uint32_t id);
    bool held(KeyCode id);
    bool held(KeyArray ids);
    bool held(const std::vector<KeyCode>& ids);
    bool released(uint32_t id);
    bool released(KeyCode id);
    bool released(KeyArray ids);
    bool released(const std::vector<KeyCode>& ids);
    bool toggled(KeyCode id);

    char typed();
    bool ascii_default(KeyCode key);
}

namespace mouse
{
    enum ButtonCode
    {
        NIL, BUTTON_0, LEFT = BUTTON_0, BUTTON_1, RIGHT = BUTTON_1, BUTTON_2, MIDDLE = BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, UNKNOWN, LAST = UNKNOWN
    };

    struct ButtonArray
    {
        ButtonArray(const std::vector<ButtonCode>& inputs)
        {
            one = 0;
            for(ButtonCode code : inputs)
            {
                one |= ((uint16_t)1) << code;
            }
        }

        std::vector<ButtonCode> get() const
        {
            std::vector<ButtonCode> result = {};
            uint64_t temp = one;
            while(temp)
            {
                ButtonCode code = (ButtonCode)(std::countr_zero(temp));
                temp &= ~((uint16_t)1 << code);
                result.push_back(code);
            }
            return result;
        }

        private:
            uint16_t one;
    };

    bool pressed(ButtonCode id);
    bool pressed(ButtonArray ids);
    bool pressed(const std::vector<ButtonCode>& ids);
    bool held(ButtonCode id);
    bool held(ButtonArray ids);
    bool held(const std::vector<ButtonCode>& ids);
    bool released(ButtonCode id);
    bool released(ButtonArray ids);
    bool released(const std::vector<ButtonCode>& ids);
}


namespace controller
{
    enum ButtonCode
    {
        NIL, BUTTON_A, BUTTON_CROSS = BUTTON_A, BUTTON_B, BUTTON_CIRCLE = BUTTON_B, BUTTON_X, BUTTON_SQUARE = BUTTON_X, BUTTON_Y, BUTTON_TRIANGLE = BUTTON_Y, LEFT_BUMPER, RIGHT_BUMPER, LEFT_TRIGGER, RIGHT_TRIGGER, BACK, START, GUIDE, LEFT_THUMB, RIGHT_THUMB, DPAD_UP, DPAD_RIGHT, DPAD_DOWN, DPAD_LEFT, UNKNOWN, LAST = UNKNOWN
    };

    enum AxisCode
    {
        AXIS_NIL, AXIS_LEFT_X, AXIS_LEFT_Y, AXIS_RIGHT_X, AXIS_RIGHT_Y, AXIS_LEFT_TRIGGER, AXIS_RIGHT_TRIGGER, AXIS_UNKNOWN, AXIS_LAST = AXIS_UNKNOWN
    };

    struct ButtonArray
    {
        ButtonArray(const std::vector<ButtonCode>& inputs)
        {
            one = 0;
            for(ButtonCode code : inputs)
            {
                one |= ((uint32_t)1) << code;
            }
        }

        std::vector<ButtonCode> get() const
        {
            std::vector<ButtonCode> result = {};
            uint64_t temp = one;
            while(temp)
            {
                ButtonCode code = (ButtonCode)(std::countr_zero(temp));
                temp &= ~(1 << code);
                result.push_back(code);
            }
            return result;
        }

        private:
            uint32_t one;
    };

    bool pressed(uint32_t index, ButtonCode id);
    bool pressed(uint32_t index, ButtonArray ids);
    bool pressed(uint32_t index, const std::vector<ButtonCode>& ids);
    bool held(uint32_t index, ButtonCode id);
    bool held(uint32_t index, ButtonArray ids);
    bool held(uint32_t index, const std::vector<ButtonCode>& ids);
    bool released(uint32_t index, ButtonCode id);
    bool released(uint32_t index, ButtonArray ids);
    bool released(uint32_t index, const std::vector<ButtonCode>& ids);
    float axis(uint32_t index, AxisCode id);
}

// InputManager (struct): stores keys and regulates whether they are being pressed, held, or released
struct InputManager
{
    uint8_t heldKey = 0;

    std::vector<bool> toggles = std::vector<bool>();
    std::vector<Key> inputs = std::vector<Key>();
    std::set<Key> heldKeys = std::set<Key>();

    void initialize(int end, int toggleEnd = 0);    // initialized key values from 0 to 'end'
    void parse(int32_t input, bool pressed);        // determines whether the input is being pressed, held, or released
    void refresh();                                 // clears pressed and released keys after one frame, and erases held keys if they are released
};

// 
struct JoystickManager : InputManager
{
    uint8_t id;
    bool active = false;
    std::vector<float> axes;

    void initializeAxes(int end);
    void joystick_button_callback();
};

// Button (struct): triggers its 'trigger' function when its attached Rect component is clicked on
struct Button
{
    uint32_t event;
    Button(uint32_t event__ = -1) : event(event__) {}

    static uint32_t data(void *input)
    {
        return *((uint32_t *)input);
    }

    private:
        uint32_t keys = 0;
        uint32_t buttons = 0;
};