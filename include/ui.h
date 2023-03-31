#ifndef UI_H
#define UI_H

#include "file_util.h"
#include "vector.h"

namespace alignment
{
    enum horizontal
    {
        LEFT, CENTER, RIGHT, JUSTIFIED
    };
    enum vertical
    {
        BOTTOM, MIDDLE, TOP
    };
};

// Alignment (struct): holds basic data to determine how a UI entity should be rendered to the screen
struct Alignment
{
    alignment::vertical vertical;
    alignment::horizontal horizontal;

    Alignment() : vertical(alignment::MIDDLE), horizontal(alignment::CENTER) {}
    Alignment(alignment::vertical vAlignment__, alignment::horizontal hAlignment__) : vertical(vAlignment__), horizontal(hAlignment__) {}
};

// Rect (struct): screen-space equivalent of Transform :: holds position, rotation, scale, and alignment of an entity
struct Rect
{
    Alignment alignment;
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;

    Rect() {}
    Rect(Alignment alignment__, Vector2 scale__ = vec3::one, Vector2 position__ = vec3::zero, Quaternion rotation__ = Quaternion()) : alignment(alignment__), position(position__), scale(scale__), rotation(rotation__)
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
    bool contains(const Vector2& vec);

    // determines this Rect's 'position' relative to its 'alignment'
    Vector2 relativePosition();

    private:
        Vector2 relativeOrigin;

        // refreshes the relative origin of the Rect
        void refresh()
        {
            relativeOrigin = Vector2(alignment.horizontal-1, alignment.vertical-1);
        }
};

namespace text
{
    enum NewLineSetting
    {
        LETTER, WORD
    };
}
struct Text
{
    Text() {}
    Text(const Font& font__, const std::string& text__, const Color& color__, float scale__, const Alignment& alignment__, const Rect& bounds__, const Shader& shader__);

    void refresh();
    void render(const Vector2& position);

    void setText(const std::string& newText)
    {
        text = newText;
        refresh();
    }
    void addText(const std::string& newText)
    {
        text += newText;
        refresh();
    }

    float getScale()
    {
        return scale;
    }
    void setScale(float scale__)
    {
        scale = scale;
        refresh();
    }

    text::NewLineSetting getNewLineSetting()
    {
        return newLineSetting;
    }
    void setScale(text::NewLineSetting newLineSetting__)
    {
        newLineSetting = newLineSetting__;
        refresh();
    }

    Alignment getAlignment()
    {
        return alignment;
    }
    void setAlignment(Alignment alignment__)
    {
        alignment = alignment__;
        refresh();
    }

    float getCeiling()
    {
        return font.maxScale.y * 0.025f * scale;
    }
    float getHeight()
    {
        return height * 0.1f;
    }

    Color getColor()
    {
        return color;
    }
    void setColor(const Color& p_color)
    {
        color = p_color;
    }

    private:

        Vector2 relativeOrigin;

        std::vector<Vector2> points;

        Font font;
        std::string text;
        Color color;
        float scale, height;

        text::NewLineSetting newLineSetting = text::WORD;
        Alignment alignment;
        Rect bounds;

        Shader shader;
        uint32_t VAO, VBO;

        void leftAligned();
        void centerAligned();
        void rightAligned();
};

#endif