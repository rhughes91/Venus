#pragma once

#include <cstdint>
#include <string>
#include <vector>

//
class Source
{
    static inline std::string projSource, audioSource, configSource, textureSource, fontSource, modelSource, shaderSource;
    static std::string getCurrentDirectoryName();

    public:
        static void initialize();

        static std::string root()
        {
            return projSource;
        }

        static std::string audio()
        {
            return audioSource;
        }
        static void setAudio(const std::string& src)
        {
            audioSource = src;
        }

        static std::string config()
        {
            return configSource;
        }
        static void setConfig(const std::string& src)
        {
            configSource = src;
        }

        static std::string texture()
        {
            return textureSource;
        }
        static void setTexture(const std::string& src)
        {
            textureSource = src;
        }

        static std::string font()
        {
            return fontSource;
        }
        static void setFont(const std::string& src)
        {
            fontSource = src;
        }

        static std::string model()
        {
            return modelSource;
        }
        static void setModel(const std::string& src)
        {
            modelSource = src;
        }

        static std::string shader()
        {
            return shaderSource;
        }
        static void setShader(const std::string& src)
        {
            shaderSource = src;
        }
};

namespace file
{
    // loads file at 'fileName' into a std::string
    std::string loadFileToString(const std::string &fileName);

    // loads file at 'fileName' into std::string vector separated by line
    std::vector<std::string> loadFileToStringVector(const std::string &fileName);

    void loadFilesInDirectory(const std::string &fileName, void(*load)(const std::string&));

    // bool save(const std::string &fileName, const std::vector<char>& data);
}

namespace binary
{
    int8_t bitArrayToInt8(const std::vector<bool>& arr);
}

namespace image
{

}