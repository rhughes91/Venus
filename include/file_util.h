#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>
#include <unordered_map>

#include "vector.h"

class Source
{
    std::string source, audioSource, configSource, textureSource, fontSource, modelSource, shaderSource;
    std::string getCurrentDirectoryName();

    public:
        Source();

        std::string getSource()
        {
            return source;
        }

        std::string getAudioSource()
        {
            return audioSource;
        }
        void setAudioSource(const std::string& src)
        {
            audioSource = src;
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

    std::string audio();
    void setAudio(const std::string& source);

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

struct CharacterTTF
{
    Vector2I min;
    Vector2I scale;
    int32_t lsb, rsb;
    std::vector<uint16_t> contourEnds;
    std::vector<Point> points;
};
struct Font
{
    Vector2 maxScale = 0;
    std::unordered_map<char, CharacterTTF> characters;
    uint16_t unitsPerEm;
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

    void loadWAV(const std::string& fileName);

    // bool save(const std::string &fileName, const std::vector<char>& data);
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