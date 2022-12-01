#include "structure.h"
#include "setup.h"

extern ObjectManager g_manager;
extern Window g_window;
extern Time g_time;

Camera::Camera(float speed__, Color color__, Vector3 front__, Vector3 up__) : speed(speed__), backgroundColor(color__), front(front__), up(up__)
{
    projection = mat4::inter(math::radians(45.0f), 2.5f, g_window.aspectRatioInv(), 0.01f, 200.0f, 1.0f);
}

void Model::draw()
{
    data.draw(texture);
}

Vector2 Rect::relativePosition()
{
    return position + relativeOrigin - scale * vec2::sign(relativeOrigin) * Vector2(g_window.aspectRatio(), 1) * 0.5f;
}

uint32_t Animation::step()
{
    if(frameCount != frameRate-1)
    {
        frameCount++;
    }
    else
    {
        frameCount = 0;
        if(frames.size() == 0)
            return texture::get("default.png");
        frame = (frame + 1) % frames.size();
    }
    return frames[frame];
}

void anim::reset(Animation& animation)
{
    animation.frame = 0;
}

void anim::keep(Animation& current, Animation& last)
{
    current.frame = (last.frame < current.frames.size() ? last.frame:0);
}