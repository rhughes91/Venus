#ifndef UI_H
#define UI_H

#include "shader.h"
#include <array>

struct Text;

//
struct CharacterTTF
{
    Vector2I min;
    Vector2I scale;
    int32_t lsb, rsb;
    std::vector<uint16_t> contourEnds;
    std::vector<Point> points;
};

//
struct Font
{
    Vector2 maxScale = 0;
    std::array<CharacterTTF, '~'+1> characters;
    uint16_t unitsPerEm;

    inline static std::unordered_map<std::string, Font> loadedFonts;
};

//
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

//
struct Sprite
{
    Texture texture;

    Sprite(){}
    Sprite(const Texture& texture__) : texture(texture__)
    {
        square = Mesh::get("square");
    }
    
    void refresh()
    {
        square.refresh();
    }
    void render()
    {
        square.draw(texture.texture);
    }

    Mesh square;
    float sorting = 0;
};


//
namespace text
{
    enum NewLineSetting
    {
        LETTER, WORD
    };
}

// Rect (struct): screen-space equivalent of Transform :: holds position, rotation, scale, and alignment of an entity
struct Rect
{
    Alignment alignment;
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;

    Rect() {}
    Rect(Alignment alignment__, Vector2 scale__ = vec2::one, Vector2 position__ = vec2::zero, Quaternion rotation__ = Quaternion()) : alignment(alignment__), position(position__), scale(scale__), rotation(rotation__) {}

    void setRenderScaling(bool scaled__)
    {
        renderScaling = scaled__;
    }
    void setAspectScaling(bool scaled__)
    {
        aspectScaling = scaled__;
    }

    // checks whether the provided position 'vec' is within the bounds of the Rect
    bool contains(const Vector2& vec, const Vector2& dim) const;
    bool scaled() const
    {
        return renderScaling;
    }
    bool useAspect() const
    {
        return aspectScaling;
    }

    Vector2 adjustedScale(const Vector2& dim) const;

    // determines this Rect's 'position' relative to its 'alignment'
    Vector2 relativePosition(const Vector2& dim) const;

    private:
        bool renderScaling = false, aspectScaling = true;

        // refreshes the relative origin of the Rect
        Vector2 relativeOrigin() const
        {
            return Vector2(alignment.horizontal-1, alignment.vertical-1);
        }
};

//
struct Text
{
    bool update = false;
    Shader textShader;
    // void(*run)(entity e, const Text& sprite, const Camera& camera, const Transform& cameraTransform);
        
    Text() {}
    Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__, const Shader& shader__ = Shader::get("ui_shader"), const Color& color__ = color::BLACK);

    void initialize(const Rect& bounds, const Vector2& dim);
    void refresh(const Rect& bounds, const Vector2& dim);
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


//
namespace ttf
{
    enum ErrorCode
    {
        SUCCESS, END_OF_FILE, INVALID_CHECKSUM, INVALID_BENCHMARK, UNSUPPORTED_CMAP, UNSUPPORTED_PLATFORM, UNSUPPORTED_FORMAT
    };

    void load(const std::string &fileName);
    Font& get(const std::string &fileName);
}

//
namespace file
{
    Font loadTTF(const std::string &fileName);
}

#endif