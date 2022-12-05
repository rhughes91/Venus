#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

uint32_t glInputToKeyCode(uint32_t input);
uint32_t glInputToButtonCode(uint32_t input);

namespace key
{
    enum KeyCode
    {
        NIL, UP, DOWN, LEFT, RIGHT, PAGE_UP, PAGE_DOWN, NUM_LOCK, BACKSPACE, TAB, ENTER, K_ENTER, K_DECIMAL, K_ADD, K_SUBTRACT, K_MULTIPLY, K_DIVIDE, 
        K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9, 
        ESCAPE, LEFT_SHIFT, RIGHT_SHIFT, LEFT_CTRL, RIGHT_CTRL, SPACE, INSERT, PRINT_SCREEN, LEFT_ALT, RIGHT_ALT, LEFT_SUPER, RIGHT_SUPER, APOSTROPHE, 
        HOME, END, CAPS_LOCK, SCROLL_LOCK, COMMA, MINUS, PERIOD, SLASH, 
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, 
        PAUSE, SEMICOLON, K_EQUAL, EQUAL, MENU, DECR_VOLUME, INCR_VOLUME, 
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, MUTE, CALCULATOR, GRAVE, 
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24, F25, 
        GLOBAL_0, GLOBAL_1, GLOBAL_2, GLOBAL_3, GLOBAL_4, DEL, LAST = DEL
    };

    bool pressed(KeyCode id);
    bool held(KeyCode id);
    bool released(KeyCode id);
}

namespace mouse
{
    enum ButtonCode
    {
        NIL, BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, LEFT = BUTTON_0,RIGHT = BUTTON_1, MIDDLE = BUTTON_2, LAST = BUTTON_8
    };

    bool pressed(ButtonCode id);
    bool held(ButtonCode id);
    bool released(ButtonCode id);
}

#endif