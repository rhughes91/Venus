#pragma once

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

    static size_t length(const CharacterTTF& data)
    {        
        return 
            object::length(data.min) + 
            object::length(data.scale) +
            object::length(data.lsb) +
            object::length(data.rsb) +
            object::length(data.contourEnds) +
            object::length(data.points);
    }

    static size_t serialize(const CharacterTTF& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.min, stream, index + count);
        count += object::serialize(value.scale, stream, index + count);
        count += object::serialize(value.lsb, stream, index + count);
        count += object::serialize(value.rsb, stream, index + count);
        count += object::serialize(value.contourEnds, stream, index + count);
        count += object::serialize(value.points, stream, index + count);

        return count;
    }

    static CharacterTTF deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        CharacterTTF result = CharacterTTF();
        size_t count = 0;

        result.min = object::deserialize<Vector2I>(stream, index + count);
        count += object::length(result.min);

        result.scale = object::deserialize<Vector2I>(stream, index + count);
        count += object::length(result.scale);

        result.lsb = object::deserialize<int32_t>(stream, index + count);
        count += object::length(result.lsb);

        result.rsb = object::deserialize<int32_t>(stream, index + count);
        count += object::length(result.rsb);

        result.contourEnds = object::deserialize<std::vector<uint16_t>>(stream, index + count);
        count += object::length(result.contourEnds);

        result.points = object::deserialize<std::vector<Point>>(stream, index + count);
        count += object::length(result.points);

        return result;
    }
};

//
struct Font
{
    Vector2 maxScale = 0;
    std::vector<CharacterTTF> characters;
    uint16_t unitsPerEm;

    static size_t length(const Font& data)
    {        
        return 
            object::length(data.maxScale) + 
            object::length(data.characters) +
            object::length(data.unitsPerEm);
    }

    static size_t serialize(const Font& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.maxScale, stream, index + count);
        count += object::serialize(value.characters, stream, index + count);
        count += object::serialize(value.unitsPerEm, stream, index + count);

        return count;
    }

    static Font deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Font result = Font();
        size_t count = 0;

        result.maxScale = object::deserialize<Vector2>(stream, index + count);
        count += object::length(result.maxScale);

        result.characters = object::deserialize<std::vector<CharacterTTF>>(stream, index + count);
        count += object::length(result.characters);

        result.unitsPerEm = object::deserialize<uint16_t>(stream, index + count);
        count += object::length(result.unitsPerEm);

        return result;
    }

    static void loadAll(const std::string &directory);
    static void load(const std::string &fileName, const std::string &path);
    static Font& get(const std::string &fileName);

    private:
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

    static size_t length(const Sprite& data)
    {        
        return 
            object::length(data.texture) + 
            object::length(data.square);
    }

    static size_t serialize(const Sprite& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.texture, stream, index + count);
        count += object::serialize(value.square, stream, index + count);

        return count;
    }

    static Sprite deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Sprite result = Sprite();
        size_t count = 0;

        result.texture = object::deserialize<Texture>(stream, index + count);
        count += object::length(result.texture);

        result.square = object::deserialize<Mesh>(stream, index + count);
        count += object::length(result.square);

        return result;
    }

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

    private:
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
    Vector2 position, storedPosition;
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
    static size_t length(const Text& data)
    {        
        return 
            object::length(data.font) + 
            object::length(data.text) +
            object::length(data.scale) +
            object::length(data.spacing) +
            object::length(data.linePadding) +
            object::length(data.newLineSetting) +
            object::length(data.alignment);
    }

    static size_t serialize(const Text& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.font, stream, index + count);
        count += object::serialize(value.text, stream, index + count);
        count += object::serialize(value.scale, stream, index + count);
        count += object::serialize(value.spacing, stream, index + count);
        count += object::serialize(value.linePadding, stream, index + count);
        count += object::serialize(value.newLineSetting, stream, index + count);
        count += object::serialize(value.alignment, stream, index + count);

        return count;
    }

    static Text deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        Text result = Text();
        size_t count = 0;

        result.font = object::deserialize<Font>(stream, index + count);
        count += object::length(result.font);

        result.text = object::deserialize<std::string>(stream, index + count);
        count += object::length(result.text);

        result.scale = object::deserialize<float>(stream, index + count);
        count += object::length(result.scale);

        result.spacing = object::deserialize<float>(stream, index + count);
        count += object::length(result.spacing);

        result.linePadding = object::deserialize<float>(stream, index + count);
        count += object::length(result.linePadding);

        result.newLineSetting = object::deserialize<text::NewLineSetting>(stream, index + count);
        count += object::length(result.newLineSetting);

        result.alignment = object::deserialize<Alignment>(stream, index + count);
        count += object::length(result.alignment);

        return result;
    }


    Text() {}
    Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__);    

    private:
        Font font;
        std::string text;
        float scale, spacing, linePadding;

        text::NewLineSetting newLineSetting = text::WORD;
        Alignment alignment;
};


//
namespace ttf
{
    enum ErrorCode
    {
        SUCCESS, END_OF_FILE, INVALID_CHECKSUM, INVALID_BENCHMARK, UNSUPPORTED_CMAP, UNSUPPORTED_PLATFORM, UNSUPPORTED_FORMAT
    };
}

//
namespace file
{
    Font loadTTF(const std::string &fileName);
}