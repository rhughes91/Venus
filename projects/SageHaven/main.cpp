#include "platformer.h"

int main()
{
    project::initialize();

    if(createWindow("Venus v.0.0.2", 800, 600))
    {
        // window::enableDecoration(false);
        // window::maximize();
        window::enableVSync(true);
        // window::setDefaultBackgroundColor(color::CLEAR);

        beginEventLoop();
    }
    else 
        return 1;
    return 0;
}