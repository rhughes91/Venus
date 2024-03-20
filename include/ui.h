#ifndef UI_H
#define UI_H

#include "file_util.h"
#include "graphics.h"
#include "vector.h"

#include <cctype>

struct Text;

namespace shader
{
    void ui(entity e, const Text& text, const Camera& camera, const Transform& cameraTransform);
}

namespace align
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
    align::vertical vertical;
    align::horizontal horizontal;

    Alignment() : vertical(align::MIDDLE), horizontal(align::CENTER) {}
    Alignment(align::vertical vAlignment__, align::horizontal hAlignment__) : vertical(vAlignment__), horizontal(hAlignment__) {}
    Alignment(align::horizontal hAlignment__, align::vertical vAlignment__) : vertical(vAlignment__), horizontal(hAlignment__) {}
};

// Rect (struct): screen-space equivalent of Transform :: holds position, rotation, scale, and alignment of an entity
struct Rect
{
    Alignment alignment;
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;

    Rect() {}
    Rect(Alignment alignment__, Vector2 scale__ = vec2::one, Vector2 position__ = vec2::zero, Quaternion rotation__ = Quaternion()) : alignment(alignment__), position(position__), scale(scale__), rotation(rotation__)
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
    void setRenderScaling(bool scaled__)
    {
        renderScaling = scaled__;
    }
    void setAspectScaling(bool scaled__)
    {
        aspectScaling = scaled__;
    }

    // checks whether the provided position 'vec' is within the bounds of the Rect
    bool contains(const Vector2& vec) const;
    bool scaled() const
    {
        return renderScaling;
    }
    bool useAspect() const
    {
        return aspectScaling;
    }

    Vector2 adjustedScale() const;

    // determines this Rect's 'position' relative to its 'alignment'
    Vector2 relativePosition() const;

    private:
        Vector2 relativeOrigin;
        bool renderScaling = true, aspectScaling = false;

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
    bool update = false;
    Shader shader;
    void(*run)(entity e, const Text& sprite, const Camera& camera, const Transform& cameraTransform);
        
    Text() {}
    Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__ = {align::TOP, align::LEFT}, const Color& color__ = color::BLACK, const Shader& shader__ = shader::get("ui_shader"), void(*run__)(entity, const Text&, const Camera&, const Transform&) = shader::ui);

    void initialize(const Rect& bounds);
    void refresh(const Rect& bounds);
    void render(entity e, const Camera& camera = Camera(), const Transform& cameraTransform = Transform())
    {
        run(e, *this, camera, cameraTransform);

        if(result0.vertices.size())
            result0.draw(texture0);
        if(result1.vertices.size())
            result1.draw(texture1);
    }
    void destroy()
    {
        result0.remove();
        result1.remove();
    }

    void setText(const std::string& newText)
    {
        text = newText;
        update = true;
    }
    void setText(char newText)
    {
        text = std::string(1, newText);
        update = true;
    }
    void addText(const std::string& newText)
    {
        for(auto c : newText)
            addText(c);
    }
    void addText(char newText)
    {
        if(newText == '\b')
        {
            if(text.size() > 0)
            {
                text = text.substr(0, text.length()-1);
                update = true;
            }
            return;
        }
        text += newText;
        update = true;
    }


    float getScale() const
    {
        return scale;
    }
    void setScale(float scale__)
    {
        scale = scale__;
        reinit();
        update = true;
    }

    float getSpacing() const
    {
        return spacing;
    }
    void setSpacing(float spacing__)
    {
        spacing = spacing__;
        update = true;
    }

    text::NewLineSetting getNewLineSetting() const
    {
        return newLineSetting;
    }
    void setNewLineSetting(text::NewLineSetting newLineSetting__)
    {
        newLineSetting = newLineSetting__;
        update = true;
    }

    Alignment getAlignment() const
    {
        return alignment;
    }
    void setAlignment(Alignment alignment__)
    {
        alignment = alignment__;
        update = true;
    }

    Color getColor() const
    {
        return color;
    }
    void setColor(const Color& p_color)
    {
        color = p_color;
    }

    private:
        Font font;
        std::string text;
        Color color;
        float scale, spacing, linePadding;

        text::NewLineSetting newLineSetting = text::WORD;
        Alignment alignment;

        uint32_t texture0, texture1;
        Mesh result0, result1;

        CharacterTTF letterData(char index);
        void reinit();
        void renderMesh(const Rect& bounds, std::vector<float>& result0, std::vector<float>& result1, std::vector<Vector3>& positions0, std::vector<Vector3>& positions1);
        void getUVs(char index, float scaling, Vector2& position, std::vector<float>& result0, std::vector<float>& result1, std::vector<Vector3>& positions0, std::vector<Vector3>& positions1);
};

#endif