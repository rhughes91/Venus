#include "audio.h"
#include "file_util.h"

#include "OpenAL/al.h"
#include "OpenAL/alc.h"

#include <iostream>
#include <unordered_map>

Audio::Audio(uint32_t buffer)
{
    alGenSources((ALuint)1, &source);
    alSourcei(source, AL_BUFFER, buffer);
}

void Audio::play()
{
    alSourcePlay(source);
}

void Audio::destroy()
{
    alDeleteSources(1, &source);
}


void Audio::load(const std::string& fileName)
{
    uint32_t pathSize;
    if((pathSize = fileName.size()) < 4)
    {
        return;
    }

    uint32_t extensionIndex = fileName.find_last_of('.');
    if(extensionIndex+1 >= pathSize)
    {
        return;
    }

    std::string extension = fileName.substr(extensionIndex+1, fileName.size());
    if(extension == "wav")
    {
        uint32_t buffer;
        alGenBuffers((ALuint)1, &buffer);

        WAV file = file::loadWAV(Source::root() + Source::audio() + fileName);
        ALenum format;
        switch(file.format)
        {
            case 1:
                if(file.bitsPerSample == 8)
                    format = AL_FORMAT_MONO8;
                else
                    format = AL_FORMAT_MONO16;
            break;
            case 2:
                if(file.bitsPerSample == 8)
                    format = AL_FORMAT_STEREO8;
                else
                    format = AL_FORMAT_STEREO16;
            break;
        }
        alBufferData(buffer, format, (ALvoid *)file.data.data(), file.data.size(), file.sampleRate);

        loadedAudios[fileName] = buffer;
    }
    else if(extension == "mp3")
    {
        // uint32_t buffer;
        // alGenBuffers((ALuint)1, &buffer);

        MP3 file = file::loadMP3(Source::root() + Source::audio() + fileName);
    }
}

uint32_t Audio::get(const std::string& fileName)
{
    return loadedAudios[fileName];
}

void Audio::clear()
{
    for(auto audio : loadedAudios)
    {
        alDeleteBuffers(1, &audio.second);
    }
}