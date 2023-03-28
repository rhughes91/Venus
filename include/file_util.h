#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "vector.h"

class Source
{
    std::string source, configSource, textureSource, fontSource, modelSource, shaderSource;
    std::string getCurrentDirectoryName();

    public:
        Source();

        std::string getSource()
        {
            return source;
        }

        std::string getConfigSource()
        {
            return configSource;
        }
        void setConfigSource(const std::string& src)
        {
            configSource = src;
        }

        std::string getTextureSource()
        {
            return textureSource;
        }
        void setTextureSource(const std::string& src)
        {
            textureSource = src;
        }

        std::string getFontSource()
        {
            return fontSource;
        }
        void setFontSource(const std::string& src)
        {
            fontSource = src;
        }

        std::string getModelSource()
        {
            return modelSource;
        }
        void setModelSource(const std::string& src)
        {
            modelSource = src;
        }

        std::string getShaderSource()
        {
            return shaderSource;
        }
        void setShaderSource(const std::string& src)
        {
            shaderSource = src;
        }
};
namespace source
{
    std::string root();

    std::string config();
    void setConfig(const std::string& source);

    std::string texture();
    void setTexture(const std::string& source);

    std::string font();
    void setFont(const std::string& source);

    std::string model();
    void setModel(const std::string& source);

    std::string shader();
    void setShader(const std::string& source);
}

struct BinaryFile
{
    std::ifstream file;
    BinaryFile(const std::string& path);

    void close()
    {
        file.close();
    }
    void seek(int32_t position)
    {
        file.seekg(position);
    }
    int32_t position()
    {
        return file.tellg();
    }

    bool open()
    {
        return file.is_open();
    }

    uint8_t toUInt8()
    {
        char c;
        file.get(c);
        return c;
    }
    uint16_t toUInt16()
    {
        return ((toUInt8() << 8) | toUInt8());
    }
    uint16_t toUInt16(uint8_t *num)
    {
        uint64_t result;
        result = (result << 8) | num[0];
        result = (result << 8) | num[1];
        return result;
    }
    int16_t toInt16()
    {
        uint16_t number = toUInt16();
        if(number & 0x8000)
            number -= 1 << 16;
        return number;
    }
    int16_t toFWord()
    {
        return toInt16();
    }
    int16_t toUFWord()
    {
        return toUInt16();
    }
    int32_t toInt32()
    {
        return (toUInt8() << 24) | (toUInt8() << 16) | (toUInt8() << 8) | toUInt8();
    }
    uint32_t toUInt32()
    {
        return toInt32();
    }
    uint16_t toUInt32(uint8_t *num)
    {
        uint64_t result;
        result = (result << 8) | num[0];
        result = (result << 8) | num[1];
        result = (result << 8) | num[2];
        result = (result << 8) | num[3];
        return result;
    }
    uint64_t toUInt64()
    {
        uint64_t result;
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        return result;
    }
    std::string toString(int length)
    {
        std::string result;
        for (int i = 0; i < length; i++)
        {
            result += char(toUInt8());
        }
        return result;
    }
    float toFixed()
    {
        return toInt32() / (1 << 16);
    }
    uint32_t toDate()
    {
        uint32_t macTime = toUInt32() * 0x100000000 + toUInt32();
        return macTime * 1000;
    }
};

struct CharacterTTF
{
    int16_t xMin, yMin;
    Vector2 scale;
    float lsb, rsb;
    std::vector<uint16_t> contourEnds;
    std::vector<Point> points;
};
struct Font
{
    Vector2 maxScale = 0;
    std::unordered_map<char, CharacterTTF> characters;
};

namespace file
{
    // loads file at 'fileName' into a std::string
    std::string loadFileToString(const std::string &fileName);

    // loads file at 'fileName' into std::string vector separated by line
    std::vector<std::string> loadFileToStringVector(const std::string &fileName);

    void loadFilesInDirectory(const std::string &fileName, void(*load)(const std::string&));

    Font loadTTF(const std::string &fileName);

    std::vector<char> loadPNG(const std::string &fileName);
}

namespace binary
{
    int8_t bitArrayToInt8(const std::vector<bool>& arr);
}

namespace ttf
{
    enum ErrorCode
    {
        SUCCESS, END_OF_FILE, INVALID_CHECKSUM, INVALID_BENCHMARK, UNSUPPORTED_CMAP, UNSUPPORTED_PLATFORM, UNSUPPORTED_FORMAT
    };

    void load(const std::string &fileName);
    Font& get(const std::string &fileName);
}

namespace image
{

}

#endif