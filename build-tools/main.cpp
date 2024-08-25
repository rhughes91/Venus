#include <algorithm>
#include <filesystem>
#include <iostream>
#include <windows.h>

int main(int argc, char *argv[])
{
    if(argc == 3)
    {
        std::string relativePath = argv[2];
        if(relativePath.size() < 8 || std::string(relativePath).substr(0, 8) != "projects")
        {
            std::cout << "Invalid build location." << std::endl;
            return 0;
        }
    }
    else if(argc < 2)
    {
        return 0;
    }

    std::string source = argv[0];
    std::replace(source.begin(), source.end(), '\\', '/');
    source = source.substr(0, source.find_last_of("/"));

    std::filesystem::remove_all(source + "/../../Builds/"+argv[1]);
    std::filesystem::create_directory(source + "/../../Builds/"+argv[1]);
    std::filesystem::copy(source + "/../projects/"+argv[1]+"/resources", source + "/../../Builds/"+argv[1]+"/resources", std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::recursive);
    std::filesystem::copy(source + "/../build/"+argv[1]+"/"+argv[1]+".exe", source + "/../../Builds/"+argv[1]+"/"+argv[1]+".exe", std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::recursive);
    std::filesystem::copy(source + "/path.config", source + "/../../Builds/"+argv[1]+"/path.config", std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::recursive);
}