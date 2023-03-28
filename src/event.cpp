#include "input.h"
#include "setup.h"
#include "structure.h"
#include "component.h"

// #include "windows.h"
// #include "psapi.h"

extern Time g_time;
extern InputManager g_keyboard, g_mouse;

void beginEventLoop()
{
    if(!window::active())
    {
        std::cout << "ERROR :: Window has not been initialized" << std::endl;
        return;
    }

    // MEMORYSTATUSEX memInfo;
    // PROCESS_MEMORY_COUNTERS_EX pmc;
    // DWORDLONG totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
    // SIZE_T virtualMemUsedByMe, physMemUsedByMe;
    
    float timeScale = 0.02f;
    object::load();
    object::start();
    g_time.beginTimer();
    while (!window::closing())
    {
        g_time.update();
        
        if(g_keyboard.inputs[key::ESCAPE].pressed)
            window::close();

        while(g_time.timer > timeScale)
        {
            object::fixedUpdate();
            g_time.resetTimer(timeScale);
        }

        if(!g_time.frozen || key::pressed(g_time.advanceKey))
        {
            object::update();
            object::lateUpdate();
        }
        object::render();

        
        // memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        // GlobalMemoryStatusEx(&memInfo);

        // GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        // totalVirtualMem = memInfo.ullTotalPageFile;
        // virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
        // virtualMemUsedByMe = pmc.PrivateUsage;

        // totalPhysMem = memInfo.ullTotalPhys;
        // physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        // physMemUsedByMe = pmc.WorkingSetSize;

        // std::cout << totalVirtualMem << " :: " << virtualMemUsedByMe << " ---- " << totalPhysMem << " :: " << physMemUsedByMe << std::endl;

        g_keyboard.refresh();
        g_mouse.refresh();
        window::refresh();
    }

    object::destroy();
    
    window::remove();
    mesh::remove();
    shader::remove();
    texture::remove();

    std::cout << event::framerate() << " FPS : " << g_time.deltaTime*1000 << " ms" << std::endl;
    window::throwError();
    window::terminate();
}