#ifndef COLOR_H
#define COLOR_H

#include <string>

// Color (struct): structure that holds four variables (r, g, b, a) that represent the four basic color channels (red, green, blue, alpha)
struct Color
{
    float r, g, b, a;
    Color()
    {
        r = g = b = a = 0;
    }
    Color(float r__, float g__, float b__, float a__) : r{r__}, g{g__}, b{b__}, a{a__}{};

    operator std::string() const
    {
        return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a);
    }
};

// color (namespace): provides easy access to basic colors
namespace color
{
    const Color
    CLEAR(1, 1, 1, 0),
    BLACK(0, 0, 0, 1),
    SKY_BLUE(0.5f, 0.8f, 0.92f, 1),
    BLUE(0, 0, 1, 1),
    NAVY(0, 0, 0.25f, 1),
    GREEN(0, 1, 0, 1),
    DARK_GREEN(0, 0.5f, 0.01f, 1),
    RED(1, 0, 0, 1),
    CYAN(0, 1, 1, 1),
    MAGENTA(1, 1, 0, 1),
    WHITE(1, 1, 1, 1),
    GREY(0.5f, 0.5f, 0.5f, 1),
    YELLOW(1, 0.92, 0.016, 1),
    PURPLE( 1, 0, 1, 1),
    PINK( 0.9f, 0.75f, 0.8f, 1),
    ORANGE(1.0f, 0.1f, 0, 1);
}

#endif