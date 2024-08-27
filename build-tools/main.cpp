#include <algorithm>
#include <filesystem>
#include <iostream>

#if defined(_WIN32)
  #include <windows.h>
#endif

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

    std::string target = "/../export/";

    auto options = std::filesystem::copy_options::skip_existing | std::filesystem::copy_options::recursive;

    std::filesystem::remove_all(source + target + argv[1]);
    std::filesystem::create_directory(source + target +argv[1]);
    std::filesystem::copy(source + "/../projects/"+argv[1]+"/resources", source + target + argv[1]+"/resources", options);
    
    // Built default project.
    if(std::filesystem::exists(source + "/../build/CMakeCache.txt"))
    {
        #if defined(_WIN32)
        std::string sourceEx = source + "/../build";
        if(std::filesystem::exists(sourceEx + "/Venus.sln"))
        {
            if(std::filesystem::exists(sourceEx + "/Debug"))
                std::filesystem::copy(sourceEx + "/Debug/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceEx + "/Release"))
                std::filesystem::copy(sourceEx + "/Release/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceEx + "/RelWithDebInfo"))
                std::filesystem::copy(sourceEx + "/RelWithDebInfo/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceEx + "/MinSizeRel"))
                std::filesystem::copy(sourceEx + "/MinSizeRel/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
        }
        else
        {
            std::filesystem::copy(source + "/../build/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
        }
        #elif defined(__linux__)
        std::filesystem::copy(source + "/../build/"+argv[1], source + target + argv[1]+"/"+argv[1], options);
        #endif
    }
    else
    {
        #if defined(_WIN32)
        std::string sourceExt = source + "/../build/"+argv[1];
        if(std::filesystem::exists(sourceExt+"/Venus.sln"))
        {
            if(std::filesystem::exists(sourceExt+"/Debug"))
                std::filesystem::copy(sourceExt+"/Debug/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceExt+"/Release"))
                std::filesystem::copy(sourceExt+"/Release/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceExt+"/RelWithDebInfo"))
                std::filesystem::copy(sourceExt+"/RelWithDebInfo/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
            else if(std::filesystem::exists(sourceExt+"/MinSizeRel"))
                std::filesystem::copy(sourceExt+"/MinSizeRel/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
        }
        else
        {
            std::filesystem::copy(sourceExt+"/"+argv[1]+".exe", source + target + argv[1]+"/"+argv[1]+".exe", options);
        }
        #elif defined(__linux__)
        std::filesystem::copy(source + "/../build/"+argv[1]+"/"+argv[1], source + target + argv[1]+"/"+argv[1], options);
        #endif
    }
    
    std::filesystem::copy(source + "/path.config", source + target + argv[1]+"/path.config", options);
}