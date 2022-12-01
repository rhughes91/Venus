#include "vellichor.h"
extern Window g_window;

#include <iostream>

int main()
{
    vellichor::initialize();

    if(initGL("Venus v.0.0.2", 800, 600))
    {
        beginEventLoop();
    }
    else 
        return 1; 
    return 0;
}