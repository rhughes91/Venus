#include "vellichor.h"

int main()
{
    vellichor::initialize();

    if(createWindow("Venus v.0.0.2", 800, 600))
    {
        window::enableVSync(true);
        window::lockCursor(true);
        window::setDefaultBackgroundColor(color::BLACK);
        
        beginEventLoop();
    }
    else
        return 1;
    return 0;
}