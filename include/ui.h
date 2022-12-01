#ifndef UI_H
#define UI_H

enum Horizontal
{
    LEFT, CENTER, RIGHT, JUSTIFIED
};
enum Vertical
{
    BOTTOM, MIDDLE, TOP
};

// Alignment (struct): holds basic data to determine how a UI entity should be rendered to the screen
struct Alignment
{
    Horizontal hAlignment;
    Vertical vAlignment;

    Alignment() : hAlignment(CENTER), vAlignment(MIDDLE) {}
    Alignment(Horizontal hAlignment__, Vertical vAlignment__) : hAlignment(hAlignment__), vAlignment(vAlignment__) {}
};

#endif