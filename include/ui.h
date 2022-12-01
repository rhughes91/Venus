#ifndef UI_H
#define UI_H

#include "vector.h"

namespace alignment
{
    enum Horizontal
    {
        LEFT, CENTER, RIGHT, JUSTIFIED
    };
    enum Vertical
    {
        BOTTOM, MIDDLE, TOP
    };
};

// Alignment (struct): holds basic data to determine how a UI entity should be rendered to the screen
struct Alignment
{
    alignment::Horizontal hAlignment;
    alignment::Vertical vAlignment;

    Alignment() : hAlignment(alignment::CENTER), vAlignment(alignment::MIDDLE) {}
    Alignment(alignment::Horizontal hAlignment__, alignment::Vertical vAlignment__) : hAlignment(hAlignment__), vAlignment(vAlignment__) {}
};

// Rect (struct): screen-space equivalent of Transform :: holds position, rotation, scale, and alignment of an entity
struct Rect
{
    Alignment alignment;
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;

    Rect() {}
    Rect(Alignment alignment__, Vector2 position__ = vec3::zero, Vector2 scale__ = vec3::one, Quaternion rotation__ = Quaternion()) : alignment(alignment__), position(position__), scale(scale__), rotation(rotation__)
    {
        refresh();
    }

    void setAlignment(const Alignment& alignment__)
    {
        alignment = alignment__;
        refresh();
    }
    void setScale(const Vector2& scale__)
    {
        scale = scale__;
        refresh();
    }

    // checks whether the provided position 'vec' is within the bounds of the Rect
    bool contains(const Vector2& vec)
    {
        Vector2 pos = relativePosition();
        return pos.x - scale.x/2 <= vec.x && pos.x + scale.x/2 >= vec.x && pos.y - scale.y/2 <= vec.y && pos.y + scale.y/2 >= vec.y;
    }

    // determines this Rect's 'position' relative to its 'alignment'
    Vector2 relativePosition();

    private:
        Vector2 relativeOrigin;

        // refreshes the relative origin of the Rect
        void refresh()
        {
            relativeOrigin = Vector2(alignment.hAlignment-1, alignment.vAlignment-1);
        }
};

// TextBox (struct): holds texts within the bounds of its attached Rect component (INCOMPLETE: TEXT RENDERING NOT YET IMPLEMENTED)
struct TextBox
{
    Alignment alignment;
    std::string text;
};

#endif