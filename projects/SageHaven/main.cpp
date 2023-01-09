#include "platformer.h"

int main()
{
    project::initialize();
    if(createWindow("Venus v.0.0.2", 800, 600))
    {
        // window::maximize();
        // window::setDefaultBackgroundColor(color::CLEAR);
        // window::enableDecoration(false);
        window::enableVSync(true);
        beginEventLoop();
    }
    else 
        return 1;
    return 0;
}