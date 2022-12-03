#include "platformer.h"

extern Window g_window;
int main()
{
    project::initialize();

    if(createWindow("Venus v.0.0.2", 800, 600))
    {
        g_window.maximize();
        g_window.enableVSync(true);
        g_window.enableDecoration(false);
        g_window.setDefaultBackgroundColor(color::CLEAR);

        beginEventLoop();
    }
    else 
        return 1;
    return 0;
}