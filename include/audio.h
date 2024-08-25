#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

//
struct Audio
{
    uint32_t source, buffer;

    Audio(uint32_t buffer);
    void play();
    void destroy();

    static void load(const std::string& fileName);
    static uint32_t get(const std::string& fileName);
    static void clear();

    private:
        inline static std::unordered_map<std::string, uint32_t> loadedAudios;
};

struct WAV
{
    std::vector<uint8_t> data;
    uint16_t format, bitsPerSample;
    uint32_t sampleRate;

    WAV()
    {
        data = std::vector<uint8_t>();
    }
};

struct MP3
{

};

//
namespace file
{
    WAV loadWAV(const std::string& fileName);

    MP3 loadMP3(const std::string& fileName);
}