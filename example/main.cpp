#include "vellichor.h"
extern Window g_window;

int main()
{
    vellichor::initialize();

    if(createWindow("Venus v.0.0.2", 800, 600))
    {
        beginEventLoop();
    }
    else 
        return 1;
    return 0;
}