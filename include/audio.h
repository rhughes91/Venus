#ifndef AUDIO_H
#define AUDIO_H

#include <string>

struct Audio
{
    uint32_t audio;
};

namespace audio
{
    void playMP3(const std::string& fileName);

    void load(const std::string& fileName);

    Audio get(const std::string& fileName);

    void remove(); 
}

#endif