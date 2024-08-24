#include "ui.h"
#include <iostream>

bool Rect::contains(const Vector2& vec, const Vector2& dim) const
{
    Vector2 pos = relativePosition(dim);
    Vector2 target;
    if(useAspect())
    {
        target = vec - Vector2((dim.x - dim.y)/dim.y * pos.x, 0);
    }
    else
    {
        target = vec * Vector2(dim.y / dim.x, 1);
    }
    float angle = rotation.euler().x*2 - M_PI;

    Vector2 adjusted = adjustedScale(dim);
    Vector2 ul = pos - adjusted * 0.5f, br = pos + adjusted * 0.5f;
    return math::quadPointIntersect(Quad(vec2::rotatedAround(ul, pos, angle), vec2::rotatedAround(Vector2(br.x, ul.y), pos, angle), vec2::rotatedAround(br, pos, angle), vec2::rotatedAround(Vector2(ul.x, br.y), pos, angle)), target);
}
Vector2 Rect::adjustedScale(const Vector2& dim) const
{
    if(scaled())
    {
        return scale * 0.02f;
    }
    return (2*scale) / dim.y;
}
Vector2 Rect::relativePosition(const Vector2& dim) const
{
    float aspect = dim.y / dim.x, scalar = dim.y;
    if(!useAspect())
    {
        aspect = 1;
    }
    if(scaled())
    {
        scalar = 100;
    }

    Vector2 scale = adjustedScale(dim);
    Vector2 origin = relativeOrigin();
    return ((Vector2(dim.y / dim.x, 1) * 2 * position / scalar) + origin - scale * origin * Vector2(aspect, 1) * 0.5f);
}

Text::Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__) : font(font__), text(text__), scale(scale__), alignment(alignment__)
{
    spacing = -0.025f;
    linePadding = 0.05f;
    newLineSetting = text::WORD;
}