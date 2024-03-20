#include "audio.h"
#include "structure.h"

#include <unordered_map>

void audio::playMP3(const std::string& fileName)
{

}

extern std::unordered_map<std::string, Audio> g_loadedAudios;
void audio::load(const std::string& fileName)
{
    
}
Audio audio::get(const std::string& fileName)
{
    return g_loadedAudios[fileName];
}
void audio::remove()
{
    for(auto audio : g_loadedAudios)
    {

    }
}