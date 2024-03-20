#include "glad/glad.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"
#include "image/stb_image.h"

#include "color.h"
#include "component.h"
#include "file_util.h"
#include "input.h"
#include "setup.h"
#include "ui.h"

#include <algorithm>
#include <iostream>
#include <type_traits>


InputManager g_keyboard, g_mouse;
extern uint32_t currentWindow;
extern std::vector<Window> g_windows;

extern object::ecs g_manager;
extern ProjectManager g_handler;
extern Time g_time;


entity object::createEntity()
{
    return g_manager.createEntity();
}
void object::removeEntity(entity e)
{
    g_manager.removeEntity(e);
}
entity object::numberOfEntities()
{
    return g_manager.numberOfEntities();
}
bool object::active(entity e)
{
    return g_manager.getComponent<bool>(e);
}
void object::setActive(entity e, bool active)
{
    g_manager.setActive(e, active);
}
size_t object::numberOfComponents()
{
    return g_manager.numberOfComponents();
}

namespace object::fn
{
    uint8_t LOAD, START, UPDATE, LATE_UPDATE, FIXED_UPDATE, RENDER, DESTROY;
}
void object::defaultInsertion(entity e, std::vector<entity>& entities, std::vector<size_t>& map)
{
    map[e] = entities.size();
    entities.push_back(e);
}
void object::setFunctionDefinitions(const std::vector<uint8_t*>& references)
{
    for(uint8_t *reference : references)
    {
        *reference = object::createSystemFunction();
    }
}
uint8_t object::createSystemFunction()
{
    return g_manager.createSystemFunction();
}

void object::run(uint8_t index)
{
    g_manager.run(index);
}
void object::parseError()
{
    g_manager.parseError();
}


float event::delta()
{
    return g_time.deltaTime;
}
float event::framerate()
{
    return (g_time.framerates[0] + g_time.framerates[1] + g_time.framerates[2] + g_time.framerates[3] + g_time.framerates[4] + g_time.framerates[5] + g_time.framerates[6] + g_time.framerates[7] + g_time.framerates[8] + g_time.framerates[9])/10.f;
}
float event::time()
{
    return g_time.runtime;
}
void event::freezeTime(bool freeze)
{
    g_time.frozen = freeze;
}

Camera::Camera(Color color__, Vector3 front__, Vector3 up__, float near__, float far__) : backgroundColor(color__), front(front__), up(up__), nearDistance(near__), farDistance(far__)
{
    projection = mat4::per(math::radians(45.0f), window::aspectRatioInv(), near__, far__);
}

bool Rect::contains(const Vector2& vec) const
{
    Vector2 pos = relativePosition();
    Vector2 target = vec - Vector2((window::width() - window::height())/window::height() * pos.x, 0);
    float angle = rotation.euler().x*2 - M_PI;

    Vector2 adjusted = adjustedScale();
    Vector2 ul = pos - adjusted * 0.5f, br = pos + adjusted * 0.5f;
    return math::quadPointIntersect(Quad(vec2::rotatedAround(ul, pos, angle), vec2::rotatedAround(Vector2(br.x, ul.y), pos, angle), vec2::rotatedAround(br, pos, angle), vec2::rotatedAround(Vector2(ul.x, br.y), pos, angle)), target);
}
Vector2 Rect::adjustedScale() const
{
    return scaled() ? scale * 0.1f : (scale*5) / window::height();
}
Vector2 Rect::relativePosition() const
{
    return (position + relativeOrigin - adjustedScale() * relativeOrigin * Vector2(window::aspectRatio(), 1) * 0.5f);
}

Vector3 object::brightness(int32_t value)
{
    switch(value)
    {
        case 1: return Vector3(1.0f, 0.7f, 1.8f);
        case 2: return Vector3(1.0f, 0.35f, 0.44f);
        case 3: return Vector3(1.0f, 0.22f, 0.20f);
        case 4: return Vector3(1.0f, 0.14f, 0.07f);
        case 5: return Vector3(1.0f, 0.09f, 0.032f);
        case 6: return Vector3(1.0f, 0.07f, 0.017f);
        case 7: return Vector3(1.0f, 0.045f, 0.0075f);
        case 8: return Vector3(1.0f, 0.027f, 0.0028f);
        case 9: return Vector3(1.0f, 0.022f, .0019f);
        case 10: return Vector3(1.0f, 0.014f, 0.0007f);
        case 11: return Vector3(1.0f, 0.0014f, 0.000007f);
    };
    return 0;
}

// cannot interpret MUTE, DECR_VOLUME, INCR_VOLUME, CALCULATOR, GLOBAL_2, GLOBAL_3, and GLOBAL_4
void InputManager::initialize(int end)
{
    for(uint32_t i = 0; i <= end; i++)
    {
        inputs.insert({i, Key{i}});
    }
}
void InputManager::initialize(int start, int end)
{
    for(uint32_t i = start; i <= end; i++)
    {
        inputs.insert({i, Key{i}});
    }
}
void InputManager::parse(int32_t input, bool pressed)
{
    Key& key = inputs[input];
    if(pressed && !key.held)
    {
        key.pressed = key.held = true;
        key.released = false;

        heldKeys.insert(key);
    }
    else if(!pressed && key.held)
    {
        key.released = true;
        key.held = false;
    }
}
void InputManager::refresh()
{
    for(Key key : heldKeys)
    {
        inputs[key.data].pressed = inputs[key.data].released = false;
        if(!inputs[key.data].held)
        {
            heldKeys.erase(key);
        }
    }
    heldKey = '\0';
}

void Timer::update(float max)
{
    timer += math::clamp(event::delta(), 0.f, max);
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
            return 0;

        if(loop)
        {
            frame = (frame + 1) % frames.size();
        }
        else
        {
            frame = std::min((float)frame+1, (float)frames.size()-1);
        }
    }
    return frame;
}
void anim::reset(Animation& animation)
{
    animation.frame = 0;
}
bool anim::restart(Animation& current, Animation& last)
{
    current.frame = 0;
    return true;
}
bool anim::keep(Animation& current, Animation& last)
{
    current.frame = (last.frame < current.frames.size() ? last.frame:0);
    return true;
}

void anim::texture(const Animation& animation, void *render, uint32_t frame, size_t size)
{
    Sprite& sprite = *(Sprite *)render;
    sprite.texture.texture = animation.frames[frame].texture;
    sprite.offset = 0;
    sprite.scale = 1;
}
void anim::uv(const Animation& animation, void *render, uint32_t frame, size_t size)
{
    Sprite& sprite = *(Sprite *)render;
    sprite.texture.texture = animation.frames[0].texture;

    float rawSquare = std::ceil((float)std::sqrt((double)size));
    int rowMiss = (rawSquare*rawSquare-size)/rawSquare;

    uint32_t x = frame%(int)rawSquare;
    uint32_t y = frame/(int)rawSquare;

    Vector2 scale = 1/Vector2(rawSquare, rawSquare-rowMiss);
    sprite.scale = scale;
    sprite.offset = Vector2(sprite.flip ? rawSquare-x-1:x, rawSquare-rowMiss-y-1) * scale;
}

std::string Physics2D::collisionsToString() const
{
    return "[UP : " + std::to_string(collisions[physics::UP]) + ", DOWN : " + std::to_string(collisions[physics::DOWN]) + ", LEFT : " + std::to_string(collisions[physics::LEFT]) + ", RIGHT : " + std::to_string(collisions[physics::RIGHT]);
}
void physics::collisionTrigger(entity e, entity collision, bool edge, int triggered)
{    
    Physics2D& physics = object::getComponent<Physics2D>(e);
    if(!triggered)
    {
        physics.resetCollisions();
    }

    if(edge)
        return;

    Transform& transform = object::getComponent<Transform>(e);
    BoxCollider& collider = object::getComponent<BoxCollider>(e);
    Vector3 boxDim = transform.scale * collider.scale * 0.5f;

    Transform& transform2 = object::getComponent<Transform>(collision);
    BoxCollider& collider2 = object::getComponent<BoxCollider>(collision);
    Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;
   
    Vector3 delta = ((transform.position + collider.offset) - collider.storedPosition) + ((transform2.position + collider2.offset) - collider2.storedPosition);
    float precision = 4;

    bool above = math::roundTo(collider.storedPosition.y - boxDim.y, precision) >= math::roundTo(collider2.storedPosition.y + boxDim2.y, precision), below = math::roundTo(collider.storedPosition.y + boxDim.y, precision) <= math::roundTo(collider2.storedPosition.y - boxDim2.y, precision);
    bool right = math::roundTo(collider.storedPosition.x - boxDim.x, precision) >= math::roundTo(collider2.storedPosition.x + boxDim2.x, precision), left = math::roundTo(collider.storedPosition.x + boxDim.x, precision) <= math::roundTo(collider2.storedPosition.x - boxDim2.x, precision);

    bool vertical = above || below;
    bool horizontal = right || left;

    if(!horizontal && delta.y < 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::DOWN] = true;
        transform.position.y = transform2.position.y - collider.offset.y + (boxDim2.y + boxDim.y) + collider2.offset.y;
    }
    else if(!horizontal && delta.y > 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::UP] = true;
        transform.position.y = transform2.position.y - collider.offset.y - (boxDim2.y + boxDim.y) + collider2.offset.y;
    }
    else if(!vertical && delta.x < 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::LEFT] = true;
        transform.position.x = transform2.position.x - collider.offset.x + (boxDim2.x + boxDim.x) + collider2.offset.x;
    }
    else if(!vertical && delta.x > 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::RIGHT] = true;
        transform.position.x = transform2.position.x - collider.offset.x - (boxDim2.x + boxDim.x) + collider2.offset.x;
    }
}
void physics::collisionMiss(entity e)
{
    object::getComponent<Physics2D>(e).resetCollisions();
}

struct CurveWinder
{
    std::vector<size_t> contour;
    int32_t sum;
};

std::vector<Vector2> bezier3(const std::vector<Vector2>& controlPoints, float partition)
{
    std::vector<Vector2> result;
    for(float i=0; i<1; i += partition)
    {
        result.push_back(std::pow(1-i, 2)*controlPoints[0] + 2*std::pow(1-i, 1)*i*controlPoints[1] + std::pow(i, 2)*controlPoints[2]);
    }
    return result;
}

Vector2I rescalePoint(const Vector2& point, const Vector2& offset, float scaling)
{
    return (Vector2I)((point + offset)*scaling);
}
void plotLine(std::vector<CurveWinder>& winders, const Vector2I& size, int maxSize, Vector2I start, const Vector2I& end, const Vector2I& difference, const Vector2 offset, float scaling, bool decide, const Color8& color)
{
    int pk = 2*difference.y - difference.x, windMod = 0;
    Vector2I point;

    for(int i=0; i<difference.x; i++)
    {
        bool vertical = pk >= 0;
        start += Vector2I((start.x < end.x)*2 - 1, ((start.y < end.y)*2 - 1)*vertical);
        pk += 2*difference.y - 2*difference.x*vertical;

        if(decide)
        {
            point = start.yx();
        }
        else
        {
            point = start;
        }
        std::vector<size_t>& winder = winders[winders.size()-1].contour;
        size_t result = (point.y+size.y/2) * maxSize + point.x + size.x/2;

        if(!winder.size() || winder[winder.size()-1] != result)
        {
            winder.push_back(result);
        }
    }
}
void drawSegment(std::vector<CurveWinder>& winders, const Vector2I& size, int maxSize, const Vector2I& position, const Vector2I& lastPosition, const Vector2& offset, float scaling, const Color8& color)
{
    Vector2I p0 = rescalePoint(lastPosition, offset, scaling);
    Vector2I p1 = rescalePoint(position, offset, scaling);
    Vector2I difference = (Vector2I)vec2::abs(p0 - p1);

    if(p0 != p1)
    {
        if(difference.x > difference.y)
        {
            plotLine(winders, size, maxSize, p0, p1, difference, offset, scaling, 0, color);
        }
        else
        {
            plotLine(winders, size, maxSize, p0.yx(), p1.yx(), difference.yx(), offset, scaling, 1, color);
        }

        std::vector<size_t>& winder = winders[winders.size()-1].contour;
        size_t result = ((p1.y % size.y)+size.y/2) * maxSize + (p1.x % size.x) + size.x/2;

        if(!winder.size() || winder[winder.size()-1] != result)
        {
            winder.push_back(result);
        }
        winders[winders.size()-1].sum += (p1.x - p0.x)*(p1.y + p0.y);
    }
}

void renderBezier(std::vector<CurveWinder>& winders, const Vector2I& size, int maxSize, const std::vector<Vector2>& bezierPoints, const Vector2& offset, float scaling, Vector2I& lastPosition, const Color8& color, float partition)
{
    std::vector<Vector2> points = {bezierPoints[0]}, result;
    for(int i=1; i<bezierPoints.size()-2; i++)
    {
        points.push_back(bezierPoints[i]);
        points.push_back((bezierPoints[i] + bezierPoints[i+1])*0.5f);
    }
    points.push_back(bezierPoints[bezierPoints.size()-2]);
    points.push_back(bezierPoints[bezierPoints.size()-1]);

    for(int j=0; j<points.size()-2; j+=2)
    {
        std::vector<Vector2> bezier = bezier3({points[j], points[j+1], points[j+2]}, partition);
        result.insert(std::end(result), std::begin(bezier), std::end(bezier));
    }
    // result = points;

    Vector2I position;
    for(int j=1; j<result.size()-1; j++)
    {
        position = (Vector2I)result[j];
        drawSegment(winders, size, maxSize, position, lastPosition, offset, scaling, color);
        lastPosition = position;
    }
}
uint32_t renderCharacterSet(Font font, int start, int scale)
{
    int size = std::sqrt(scale*1000);
    Vector2I index = Vector2I(0, 7);
    std::vector<Color8> render = std::vector<Color8>((size) * (size) * 8 * 8, Color8(0, 0));
        
    for(int c=start; c<start+64; c++)
    {
        std::vector<CurveWinder> winders = std::vector<CurveWinder>(1);
        CharacterTTF letter = font.characters[c];
        int ppi = 96, ppem = (size*0.4f) * ppi / 72, pointSize = letter.points.size();
        
        if(pointSize && c > ' ')
        {
            float scaling = (float)ppem / font.unitsPerEm;
            Vector2 offset = Vector2(-letter.lsb - letter.scale.x*0.5f, -letter.scale.y*0.5f + 300);
            Color8 color = Color8(255);

            Vector2I lastPosition = letter.points[0].position;
            Vector2I scale = Vector2I(size*(2*index.x+1), size*(2*index.y+1));

            int contourIndex = 0, contourEnd = letter.contourEnds.size() ? letter.contourEnds[contourIndex] : -1;
            int loop = 0;

            for(int i=0; i<pointSize; i++)
            {                
                Vector2I position = letter.points[i%pointSize].position;
                if(i == contourEnd+1)
                {
                    drawSegment(winders, scale, size * 8, letter.points[loop].position, lastPosition, offset, scaling, color);
                    contourEnd = letter.contourEnds.size() > (contourIndex += 1) ? letter.contourEnds[contourIndex] : -1;
                    loop = i%pointSize;
                    winders.resize(winders.size()+1);
                }
                else
                {
                    drawSegment(winders, scale, size * 8, position, lastPosition, offset, scaling, color);
                }
                lastPosition = position;

                if(!letter.points[(i+1) % pointSize].onCurve)
                {
                    size_t length = 0;

                    std::vector<Vector2> points;
                    points.push_back(letter.points[i].position);
                    while(!letter.points[(++i) % pointSize].onCurve)
                    {
                        Vector2I point = letter.points[i % pointSize].position;
                        length += (points[points.size()-1] - point).length();
                        points.push_back(point);
                    }
                    points.push_back(letter.points[(i == contourEnd+1) ? loop : i%pointSize].position);
                    length += (points[points.size()-1] - points[0]).length();
                    i--;

                    renderBezier(winders, scale, size*8, points, offset, scaling, lastPosition, color, 10.0f/size);
                }
            }
            drawSegment(winders, scale, size*8, letter.points[loop].position, lastPosition, offset, scaling, color);
        }

        std::vector<size_t> contour;
        int contourSize;
        for(int w=0; w<winders.size(); w++)
        {
            contour = winders[w].contour;
            contourSize = contour.size();

            bool toggle = true, buffer = false;
            int direction = 0, padding = 0;

            for(int idx = 0; contourSize && idx < contourSize+std::abs(padding); idx++)
            {
                int loop = ((idx%contourSize)) % 255 + 1;
                size_t placement = contour[idx%contourSize], back = contour[(idx+contourSize-1)%contourSize], forward = contour[(idx+1)%contourSize];
                bool vertical = ((placement - back) == size*8 || (back - placement) == size*8);

                if(padding <= 0)
                    padding = std::max(padding-1, -contourSize);

                if(vertical && !render[placement].g)
                {
                    padding = -padding;
                    toggle = false;
                }
                else
                {
                    toggle = true;
                    direction = back%(size*8) - placement%(size*8);
                }

                int tempDirection = placement%(size*8) - forward%(size*8);
                if(direction != 0 && tempDirection != 0 && direction != tempDirection)
                {
                    direction = 0;
                    toggle = !toggle;
                }

                render[placement] = Color8(255, toggle*loop, 255);
            }
        }
        
        for(int x = size*index.x; x < size*(index.x+1); x++)
        {
            bool toggle = false;
            for(int y = size*index.y; y < size*(index.y+1); y++)
            {
                size_t placement = size*8*y+x;
                if(render[placement].g > 0)
                {
                    toggle = !toggle;
                }
                if(toggle || render[placement].r)
                {
                    render[placement] = Color8(255);
                }
            }
        }

        index.x = (index.x+1)%8;
        if(!index.x)
            index.y--;
    }

    return texture::loadTo(render, size * 8, size * 8, texture::RGBA, texture::PNG, texture::POINT).texture;
}

Text::Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__, const Color& color__, const Shader& shader__, void(*run__)(entity, const Text&, const Camera&, const Transform&)) : font(font__), text(text__), color(color__), scale(scale__), alignment(alignment__), shader(shader__), run(run__)
{
    spacing = -0.025f;
    linePadding = 0.05f;
    newLineSetting = text::WORD;

    reinit();
}
void Text::initialize(const Rect& bounds)
{
    struct LineNode
    {
        int start, end;
        Vector2 position;
    };
    std::vector<LineNode> nodes;

    Vector2 position = 0, boundsScale = bounds.adjustedScale();
    std::vector<float> text0, text1;
    std::vector<Vector3> vectors0, vectors1;

    int start = 0, wordEnd;
    float scaling = (float)1 / font.unitsPerEm;
    float lineSpace = 0, modifier = (scale*5) / window::height(), tabWidth = 4*(font.characters[' '].lsb + font.characters[' '].rsb) * scaling, addon = 0, wordAddon;
    for(int i=0; i<text.size(); i++)
    {
        CharacterTTF letter = letterData(text[i]);
        addon = (letter.scale.x * 0.5f + letter.lsb + letter.rsb) * scaling + spacing;
        wordAddon += addon;
        switch(text[i])
        {
            case '\n':
                position.x = (boundsScale.x - lineSpace*modifier) / modifier * 0.5f * alignment.horizontal;
                nodes.push_back({start, wordEnd = i, position});
                start = i;
                position.y -= font.maxScale.y * 0.5f * scaling;
                wordAddon = lineSpace = 0;
            continue;
            case '\t':
                addon = tabWidth;
            case ' ':
                wordAddon = 0;
                wordEnd = i;
            break;
        }
        lineSpace += addon;
        if(lineSpace*modifier >= boundsScale.x)
        {
            float finalAddon = (newLineSetting == text::WORD && wordAddon != lineSpace) ? wordAddon : addon;
            int finalEnd = (newLineSetting == text::WORD && wordAddon != lineSpace) ? wordEnd : i;

            position.x = (boundsScale.x - (lineSpace-finalAddon)*modifier) / modifier * 0.5f * alignment.horizontal;
            nodes.push_back({start, finalEnd, position});
            start = finalEnd;
            position.y -= font.maxScale.y * 0.5f * scaling;
            wordAddon = lineSpace = finalAddon;
        }
    }
    
    position.x = (boundsScale.x - lineSpace*modifier) / modifier * 0.5f * alignment.horizontal;
    nodes.push_back({start, (int)text.length(), position});

    float height = position.y;
    for(auto& node : nodes)
    {
        node.position.y -= ((boundsScale.y + height*modifier) / modifier * 0.5f - font.maxScale.y * scaling * 0.25f) * (2-alignment.vertical);
        for(int j=node.start; j<node.end; j++)
        {
            if(j != node.start || (text[j] != ' ' && text[j] != '\t'))
                getUVs(text[j], scaling, node.position, text0, text1, vectors0, vectors1);
        }
    }

    result0 = Mesh(vectors0, text0, Vector3(1, 1, 0));
    result1 = Mesh(vectors1, text1, Vector3(1, 1, 0));

    result0.generate();
    if(result0.vertices.size())
        result0.refresh();
    
    result1.generate();
    if(result1.vertices.size())
        result1.refresh();
}
void Text::refresh(const Rect& bounds)
{
    struct LineNode
    {
        int start, end;
        Vector2 position;
    };
    std::vector<LineNode> nodes;

    Vector2 position = 0, rawScale = bounds.adjustedScale(), boundsScale = rawScale * window::aspectRatioInv();
    std::vector<float> text0, text1;
    std::vector<Vector3> vectors0, vectors1;

    int start = 0, wordEnd;
    float scaling = (float)1 / font.unitsPerEm;
    float lineSpace = 0, modifier = (scale*5) / window::height(), tabWidth = 4*(font.characters[' '].lsb + font.characters[' '].rsb) * scaling, addon = 0, wordAddon;

    float aspectOffset = (rawScale.x - lineSpace*modifier) / modifier * 0.5f * (1-window::aspectRatioInv());
    for(int i=0; i<text.size(); i++)
    {
        CharacterTTF letter = letterData(text[i]);
        addon = (letter.scale.x * 0.5f + letter.lsb + letter.rsb) * scaling + spacing;
        wordAddon += addon;
        switch(text[i])
        {
            case '\n':
                position.x = (boundsScale.x - lineSpace*modifier) / modifier * 0.5f * alignment.horizontal + aspectOffset;
                nodes.push_back({start, wordEnd = i, position});
                start = i;
                position.y -= font.maxScale.y * 0.5f * scaling;
                wordAddon = lineSpace = 0;
            continue;
            case '\t':
                addon = tabWidth;
            case ' ':
                wordAddon = 0;
                wordEnd = i;
            break;
        }
        lineSpace += addon;
        if(lineSpace*modifier >= boundsScale.x)
        {
            float finalAddon = (newLineSetting == text::WORD && wordAddon != lineSpace) ? wordAddon : addon;
            int finalEnd = (newLineSetting == text::WORD && wordAddon != lineSpace) ? wordEnd : i;

            position.x = (boundsScale.x - (lineSpace-finalAddon)*modifier) / modifier * 0.5f * alignment.horizontal + aspectOffset;
            nodes.push_back({start, finalEnd, position});
            start = finalEnd;
            position.y -= font.maxScale.y * 0.5f * scaling;
            wordAddon = lineSpace = finalAddon;
        }
    }
    
    position.x = (boundsScale.x - lineSpace*modifier) / modifier * 0.5f * alignment.horizontal + aspectOffset;
    nodes.push_back({start, (int)text.length(), position});

    float height = position.y;
    float aspectYOffset = ((boundsScale.y + height*modifier) / modifier * 0.5f - font.maxScale.y * scaling * 0.25f) * (1-window::aspectRatio());
    for(auto& node : nodes)
    {
        node.position.y -= ((boundsScale.y + height*modifier) / modifier * 0.5f - font.maxScale.y * scaling * 0.25f) * (2-alignment.vertical) + aspectYOffset;
        for(int j=node.start; j<node.end; j++)
        {
            if(j != node.start || (text[j] != ' ' && text[j] != '\t'))
                getUVs(text[j], scaling, node.position, text0, text1, vectors0, vectors1);
        }
    }

    result0.reinit(vectors0, text0, Vector3(1, 1, 0));
    result1.reinit(vectors1, text1, Vector3(1, 1, 0));

    if(result0.vertices.size())
    {
        result0.refresh();
    }
    if(result1.vertices.size())
    {
        result1.refresh();
    }
}
void Text::reinit()
{
    // glDeleteTextures(1, &texture0);
    // glDeleteTextures(1, &texture1);

    texture0 = renderCharacterSet(font, 0, scale);
    texture1 = renderCharacterSet(font, 64, scale);
}
CharacterTTF Text::letterData(char index)
{
    switch(index)
    {
        default: return font.characters[index];
    }
}
void Text::getUVs(char index, float scaling, Vector2& position, std::vector<float>& result0, std::vector<float>& result1, std::vector<Vector3>& positions0, std::vector<Vector3>& positions1)
{
    float tabWidth = 4*(font.characters[' '].lsb + font.characters[' '].rsb) * scaling;
    CharacterTTF data = letterData(index);
    switch(index)
    {
        case '\t':
            position.x += tabWidth;
        break;
    }

    bool first = !(index / 64);
    int trueIndex = (index % 64);

    Vector2 scaled = {(trueIndex % 8)/8.0f, 1 - (trueIndex / 8 + 1)/8.0f};
    std::vector<float> addon =
    {
        scaled.x + 0.125f, scaled.y,
        scaled.x, scaled.y + 0.125f,
        scaled.x, scaled.y,

        scaled.x + 0.125f, scaled.y + 0.125f,
        scaled.x, scaled.y + 0.125f,
        scaled.x + 0.125f, scaled.y,
    };

    position.x += data.lsb * scaling;
    Vector2 size = data.scale;
    Vector2 offset = (Vector2(-size.x*0.5f, size.y*0.5f - font.maxScale.y)) * scaling * 0.5f + Vector2(0, linePadding);
    std::vector<Vector3> vectors =
    {
        Vector3(position.x+0.5f - offset.x, position.y-0.5f + offset.y, 0),
        Vector3(position.x-0.5f - offset.x, position.y+0.5f + offset.y, 0),
        Vector3(position.x-0.5f - offset.x, position.y-0.5f + offset.y, 0),
        
        Vector3(position.x+0.5f - offset.x, position.y+0.5f + offset.y, 0),
        Vector3(position.x-0.5f - offset.x, position.y+0.5f + offset.y, 0),
        Vector3(position.x+0.5f - offset.x, position.y-0.5f + offset.y, 0),
    };
    position.x += (size.x * 0.5f + data.rsb) * scaling + spacing;
    
    if(first)
    {
        result0.insert(result0.end(), addon.begin(), addon.end());
        positions0.insert(positions0.end(), vectors.begin(), vectors.end());
    }
    else
    {
        result1.insert(result1.end(), addon.begin(), addon.end());
        positions1.insert(positions1.end(), vectors.begin(), vectors.end());
    }
}
void Text::renderMesh(const Rect& bounds, std::vector<float>& result0, std::vector<float>& result1, std::vector<Vector3>& positions0, std::vector<Vector3>& positions1)
{

}

void Screen::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Screen::initialize(const DirectionalLight& dirLight__, const Shader& screenShader__, uint32_t width, uint32_t height)
{
    dirLight = dirLight__;
    gamma = 2.2f;

    int maxSamples = getMaximumSamples();

    frameBuffer.initialize();
    subBuffer.initialize();
    depthBuffer.initialize();

    frameBuffer.addTexture("multiTexture", width, height, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, 0, GL_REPEAT, std::min(4, maxSamples));
    subBuffer.addTexture("texture", width, height, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, GL_REPEAT, 0);
    
    frameBuffer.addRenderBuffer("renderBuffer", width, height, std::min(4, maxSamples));

    shader = screenShader__;
    shader.use();
    shader.setInt("screenTexture", 0);

    int complete;
    if((complete = frameBuffer.complete()) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR :: " << complete << " :: Screen subbuffer is not complete." << std::endl;

    if((complete = subBuffer.complete()) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR :: " << complete << " :: Screen subbuffer is not complete." << std::endl;

}
void Screen::remove()
{
    frameBuffer.remove();
    subBuffer.remove();
    depthBuffer.remove();
    shader.remove();
}
void Screen::refreshResolution(uint32_t width, uint32_t height)
{
    frameBuffer.refresh(g_windows[currentWindow].width, g_windows[currentWindow].height, defaultColor.a == 1);
    subBuffer.refresh(g_windows[currentWindow].width, g_windows[currentWindow].height, defaultColor.a == 1);
    resolution = {window::width(), window::height()};
}
void Screen::store()
{
    glViewport(0, 0, g_windows[currentWindow].width, g_windows[currentWindow].height);
    frameBuffer.bind(GL_FRAMEBUFFER);
    glEnable(GL_DEPTH_TEST);
}
void Screen::draw()
{
    frameBuffer.bind(GL_READ_FRAMEBUFFER);
    subBuffer.bind(GL_DRAW_FRAMEBUFFER);
    glBlitFramebuffer(0, 0, g_windows[currentWindow].width, g_windows[currentWindow].height, 0, 0, g_windows[currentWindow].width, g_windows[currentWindow].height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    frameBuffer.unbind(GL_FRAMEBUFFER);
    glDisable(GL_DEPTH_TEST);
    glClearColor(defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    shader.use();
    shader.setFloat("gamma", gamma);
    quad.draw(subBuffer.getTexture("texture").data);

    frameBuffer.unbind(GL_READ_FRAMEBUFFER);
    subBuffer.unbind(GL_DRAW_FRAMEBUFFER);
}
int Screen::getMaximumSamples()
{
    int maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    return maxSamples;
}

void Mesh::draw(const uint32_t texture) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, g_windows[currentWindow].screen.depthBuffer.getTexture("texture").data);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void shader::simple(Shader& shader, const Transform& transform, const Camera& camera, const Transform& cameraTransform, const SimpleShader& mat)
{    
    shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
    shader.setMat4("view", camera.view.matrix, true);
    shader.setMat4("projection", camera.projection.matrix, true);
    shader.setVec3("scale", transform.scale);
    shader.setVec4("objColor", mat.color);
    shader.setVec2("offset", mat.offset);
    shader.setVec2("uvScale", mat.scale);
    shader.setBool("flip", mat.flip);
}
void shader::ui(entity e, const Sprite& sprite, const Camera& camera = Camera(), const Transform& cameraTransform = Transform())
{
    Rect& transform = object::getComponent<Rect>(e);
    Shader shader = sprite.shader;
    shader.use();
    
    shader.setFloat("aspect", transform.useAspect() ? window::aspectRatio():1);
    shader.setVec2("position", transform.relativePosition());
    shader.setVec2("scale", transform.adjustedScale());
    shader.setMat4("model", (mat4x4(1) * (mat4x4)transform.rotation).matrix, false);
    shader.setVec4("objColor", sprite.color);
}
void shader::ui(entity e, const Text& text, const Camera& camera = Camera(), const Transform& cameraTransform = Transform())
{
    Rect& transform = object::getComponent<Rect>(e);
    Shader shader = text.shader;
    shader.use();
    
    float scaleModifier = (text.getScale()*5) / window::height();
    shader.setFloat("aspect", window::aspectRatio());
    shader.setVec2("position", transform.relativePosition() + transform.adjustedScale() * Vector2(-window::aspectRatio(), 1) * 0.5f);
    shader.setVec2("scale", scaleModifier);
    shader.setMat4("model", (mat4x4(1) * (mat4x4)transform.rotation).matrix, false);
    shader.setVec4("objColor", text.getColor());
}
void shader::advanced(Shader& shader, const Transform& transform, const Camera& camera, const Transform& cameraTransform, const AdvancedShader& mat)
{
    DirectionalLight& light = window::lighting();
    
    shader.setMat4("model", (mat4x4(1).rotated(transform.rotation).translated(transform.position)).matrix, true);
    shader.setMat4("view", camera.view.matrix, true);
    shader.setMat4("projection", camera.projection.matrix, true);
    shader.setVec3("scale", transform.scale);
    
    shader.setVec3("dirLight.direction", light.direction);
    shader.setVec4("dirLight.color", light.color);
    shader.setFloat("dirLight.strength", light.strength);

    shader.setVec3("viewPos", cameraTransform.position);
    shader.setVec4("objColor", mat.color);

    shader.setFloat("material.shininess", mat.shine);
    shader.setFloat("material.ambientStrength", mat.ambient);
    shader.setFloat("material.diffuseStrength", mat.diffuse);
    shader.setFloat("material.specularStrength", mat.specular);
}

uint32_t glInputToKeyCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_KEY_SPACE: return key::SPACE;
        case GLFW_KEY_APOSTROPHE: return key::APOSTROPHE;
        case GLFW_KEY_COMMA: return key::COMMA;
        case GLFW_KEY_MINUS : return key::MINUS;
        case GLFW_KEY_PERIOD: return key::PERIOD;
        case GLFW_KEY_SLASH: return key::SLASH;
        case GLFW_KEY_0: return key::ZERO;
        case GLFW_KEY_1: return key::ONE;
        case GLFW_KEY_2: return key::TWO;
        case GLFW_KEY_3: return key::THREE;
        case GLFW_KEY_4: return key::FOUR;
        case GLFW_KEY_5: return key::FIVE;
        case GLFW_KEY_6: return key::SIX;
        case GLFW_KEY_7: return key::SEVEN;
        case GLFW_KEY_8: return key::EIGHT;
        case GLFW_KEY_9: return key::NINE;
        case GLFW_KEY_SEMICOLON: return key::SEMICOLON;
        case GLFW_KEY_EQUAL: return key::EQUAL;
        case GLFW_KEY_A: return key::A;
        case GLFW_KEY_B: return key::B;
        case GLFW_KEY_C: return key::C;
        case GLFW_KEY_D: return key::D;
        case GLFW_KEY_E: return key::E;
        case GLFW_KEY_F: return key::F;
        case GLFW_KEY_G: return key::G;
        case GLFW_KEY_H: return key::H;
        case GLFW_KEY_I: return key::I;
        case GLFW_KEY_J: return key::J;
        case GLFW_KEY_K: return key::K;
        case GLFW_KEY_L: return key::L;
        case GLFW_KEY_M: return key::M;
        case GLFW_KEY_N: return key::N;
        case GLFW_KEY_O: return key::O;
        case GLFW_KEY_P: return key::P;
        case GLFW_KEY_Q: return key::Q;
        case GLFW_KEY_R: return key::R;
        case GLFW_KEY_S: return key::S;
        case GLFW_KEY_T: return key::T;
        case GLFW_KEY_U: return key::U;
        case GLFW_KEY_V: return key::V;
        case GLFW_KEY_W: return key::W;
        case GLFW_KEY_X: return key::X;
        case GLFW_KEY_Y: return key::Y;
        case GLFW_KEY_Z: return key::Z;
        case GLFW_KEY_LEFT_BRACKET: return key::LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH: return key::BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET: return key::RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT: return key::GRAVE;
        case GLFW_KEY_ESCAPE: return key::ESCAPE;
        case GLFW_KEY_ENTER: return key::ENTER;
        case GLFW_KEY_TAB: return key::TAB;
        case GLFW_KEY_BACKSPACE: return key::BACKSPACE;
        case GLFW_KEY_INSERT: return key::INSERT;
        case GLFW_KEY_DELETE: return key::DEL;
        case GLFW_KEY_RIGHT: return key::RIGHT;
        case GLFW_KEY_LEFT: return key::LEFT;
        case GLFW_KEY_DOWN: return key::DOWN;
        case GLFW_KEY_UP: return key::UP;
        case GLFW_KEY_PAGE_UP: return key::PAGE_UP;
        case GLFW_KEY_PAGE_DOWN: return key::PAGE_DOWN;
        case GLFW_KEY_HOME: return key::HOME;
        case GLFW_KEY_END: return key::END;
        case GLFW_KEY_CAPS_LOCK: return key::CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK: return key::SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK: return key::NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN: return key::PRINT_SCREEN;
        case GLFW_KEY_PAUSE: return key::PAUSE;
        case GLFW_KEY_F1: return key::F1;
        case GLFW_KEY_F2: return key::F2;
        case GLFW_KEY_F3: return key::F3;
        case GLFW_KEY_F4: return key::F4;
        case GLFW_KEY_F5: return key::F5;
        case GLFW_KEY_F6: return key::F6;
        case GLFW_KEY_F7: return key::F7;
        case GLFW_KEY_F8: return key::F8;
        case GLFW_KEY_F9: return key::F9;
        case GLFW_KEY_F10: return key::F10;
        case GLFW_KEY_F11: return key::F11;
        case GLFW_KEY_F12: return key::F12;
        case GLFW_KEY_F13: return key::F13;
        case GLFW_KEY_F14: return key::F14;
        case GLFW_KEY_F15: return key::F15;
        case GLFW_KEY_F16: return key::F16;
        case GLFW_KEY_F17: return key::F17;
        case GLFW_KEY_F18: return key::F18;
        case GLFW_KEY_F19: return key::F19;
        case GLFW_KEY_F20: return key::F20;
        case GLFW_KEY_F21: return key::F21;
        case GLFW_KEY_F22: return key::F22;
        case GLFW_KEY_F23: return key::F23;
        case GLFW_KEY_F24: return key::F24;
        case GLFW_KEY_F25: return key::F25;
        case GLFW_KEY_KP_0: return key::K_0;
        case GLFW_KEY_KP_1: return key::K_1;
        case GLFW_KEY_KP_2: return key::K_2;
        case GLFW_KEY_KP_3: return key::K_3;
        case GLFW_KEY_KP_4: return key::K_4;
        case GLFW_KEY_KP_5: return key::K_5;
        case GLFW_KEY_KP_6: return key::K_6;
        case GLFW_KEY_KP_7: return key::K_7;
        case GLFW_KEY_KP_8: return key::K_8;
        case GLFW_KEY_KP_9: return key::K_9;
        case GLFW_KEY_KP_DECIMAL: return key::K_DECIMAL;
        case GLFW_KEY_KP_DIVIDE: return key::K_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY: return key::K_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT: return key::K_SUBTRACT;
        case GLFW_KEY_KP_ADD: return key::K_ADD;
        case GLFW_KEY_KP_ENTER: return key::K_ENTER;
        case GLFW_KEY_KP_EQUAL: return key::K_EQUAL;
        case GLFW_KEY_LEFT_SHIFT: return key::LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL: return key::LEFT_CTRL;
        case GLFW_KEY_LEFT_ALT: return key::LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER: return key::LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT: return key::RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL: return key::RIGHT_CTRL;
        case GLFW_KEY_RIGHT_ALT: return key::RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER: return key::RIGHT_SUPER;
        case GLFW_KEY_WORLD_1: return key::GLOBAL_0;
        case GLFW_KEY_WORLD_2: return key::GLOBAL_1;
        case GLFW_KEY_MENU: return key::MENU;
    }
    return key::UNKNOWN;
}
uint32_t glInputToButtonCode(uint32_t input)
{
    switch(input)
    {
        case GLFW_MOUSE_BUTTON_1: return mouse::BUTTON_0;
        case GLFW_MOUSE_BUTTON_2: return mouse::BUTTON_1;
        case GLFW_MOUSE_BUTTON_3: return mouse::BUTTON_2;
        case GLFW_MOUSE_BUTTON_4: return mouse::BUTTON_3;
        case GLFW_MOUSE_BUTTON_5: return mouse::BUTTON_4;
        case GLFW_MOUSE_BUTTON_6: return mouse::BUTTON_5;
        case GLFW_MOUSE_BUTTON_7: return mouse::BUTTON_6;
        case GLFW_MOUSE_BUTTON_8: return mouse::BUTTON_7;
    }
    return mouse::UNKNOWN;
}

void error_callback(int error, const char* msg)
{
	std::string s;
	s = " [" + std::to_string(error) + "] " + msg + '\n';
	std::cout << s << std::endl;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    window::enableCapslock((bool)(mods & GLFW_MOD_CAPS_LOCK));
    window::enableNumpad((bool)(mods & GLFW_MOD_NUM_LOCK));

    if(action != GLFW_RELEASE)
    {
        g_keyboard.heldKey = glInputToKeyCode(key);
    }

    if(action == GLFW_REPEAT)
    {
        return;
    }

    for(GLenum k = 32; k<=GLFW_KEY_LAST; k++)
    {
        g_keyboard.parse(glInputToKeyCode(k), glfwGetKey(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_keyboard.parse(glInputToKeyCode(key), false);
    }

    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_F && (mods & GLFW_MOD_CONTROL) && !g_windows[currentWindow].screen.fullscreen)
        {
            if(window::maximized())
            {
                glfwRestoreWindow(window);
            }
            else
            {
                glfwMaximizeWindow(window);
            }
        }
        else if(key == GLFW_KEY_F11)
        {
            window::fullscreen(!g_windows[currentWindow].screen.fullscreen);
        }
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(action == GLFW_REPEAT)
        return;

    for(GLenum k = 0; k <= GLFW_MOUSE_BUTTON_LAST; k++)
    {
        g_mouse.parse(glInputToButtonCode(k), glfwGetMouseButton(window, k));
    }

    if(action == GLFW_RELEASE)
    {
        g_mouse.parse(glInputToButtonCode(button), false);
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    g_windows[currentWindow].cursorPosition = {(int)xpos, (int)ypos};
}
void framebuffer_size_callback(GLFWwindow *window, int32_t width, int32_t height)
{
    if(width && height)
    {
        g_windows[currentWindow].width = width;
        g_windows[currentWindow].height = height;
        g_windows[currentWindow].screen.refreshResolution(width, height);
    }

    glViewport(0, 0, width, height);
}

Time::Time()
{
    if(!advanceKey)
    {
        framerates = {60, 60, 60, 60, 60, 60, 60, 60, 60, 60};
        advanceKey = key::BACKSLASH;
    }
}
void Time::update()
{
    runtime = glfwGetTime();
    lastDeltaTime = deltaTime;
    deltaTime = std::min(runtime - lastFrame, 0.1);
    lastFrame = runtime;
    timer += deltaTime;
    framerates[framerateIndex = (framerateIndex+1)%10] = (1/deltaTime);
}
void Time::beginTimer()
{
    float currentFrame = glfwGetTime();
    lastFrame = currentFrame;
}

Window::Window(std::string name, uint32_t width__, uint32_t height__)
{
    width = width__;
    height = height__;
    screen.defaultColor = color::RED;

    data = glfwCreateWindow(800, 600, "default", NULL, NULL);
    if(data == NULL)
    {
        std::cout << "ERROR :: Failed to create GLFW window" << std::endl;
        return;
    }

    glfwMakeContextCurrent((GLFWwindow *)data);

    glfwShowWindow((GLFWwindow *)data);
    glfwSetWindowPos((GLFWwindow *)data, 0, 25);
    glfwSetFramebufferSizeCallback((GLFWwindow *)data, framebuffer_size_callback);
    glfwSetKeyCallback((GLFWwindow *)data, key_callback);
    glfwSetCursorPosCallback((GLFWwindow *)data, mouse_callback);
    glfwSetMouseButtonCallback((GLFWwindow *)data, mouse_button_callback);
    glfwSetInputMode((GLFWwindow *)data, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

    configureGLAD();
    glViewport(0, 0, width, height);
    
    double xPos, yPos;
    glfwGetCursorPos((GLFWwindow *)data, &xPos, &yPos);
    cursorPosition = {(int)xPos, (int)yPos};

    stbi_set_flip_vertically_on_load(true);
    screen.initialize(DirectionalLight(vec3::forward, color::WHITE), Shader("screen_vertex", "screen_frag"), width, height);

    
}
void Window::configureGLAD()
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR :: Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        data = NULL;
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    active = true;
}

bool key::pressed(int32_t id)
{
    return g_keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyCode id)
{
    return g_keyboard.inputs[id].pressed;
}
bool key::pressed(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].pressed)
            return true;
    }
    return false;
}
bool key::held(int32_t id)
{
    return g_keyboard.inputs[id].held;
}
bool key::held(key::KeyCode id)
{
    return g_keyboard.inputs[id].held;
}
bool key::held(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].held)
        {
            return true;
        }
    }
    return false;
}
bool key::released(int32_t id)
{
    return g_keyboard.inputs[id].released;
}
bool key::released(key::KeyCode id)
{
    return g_keyboard.inputs[id].released;
}
bool key::released(key::KeyArray ids)
{
    for(const KeyCode &id : ids)
    {
        if(g_keyboard.inputs[id].released)
            return true;
    }
    return false;
}
char key::typed()
{
    KeyCode key = (KeyCode)g_keyboard.heldKey;
    if(key::held({key::LEFT_CTRL, key::RIGHT_CTRL}) || !ascii_default(key))
    {
        return '\0';
    }

    key = (KeyCode)std::tolower(key);
    if(key::held({key::LEFT_SHIFT, key::RIGHT_SHIFT}))
    {
        if(key >= 'a' && key <='z')
        {
            return window::capslockEnabled() ? key : std::toupper(key);
        }
        
        if(!window::capslockEnabled())
        {
            switch(key)
            {
                case '1': return '!';
                case '2': return '@';
                case '3': return '#';
                case '4': return '$';
                case '5': return '%';
                case '6': return '^';
                case '7': return '&';
                case '8': return '*';
                case '9': return '(';
                case '0': return ')';
                case '-': return '_';
                case '=': return '+';
                case '[': return '{';
                case ']': return '}';
                case '\\': return '|';
                case ';': return ':';
                case '\'': return '"';
                case ',': return '<';
                case '.': return '>';
                case '/': return '?';
                case '`': return '~';
            }
        }
    }
    
    if(window::numpadEnabled())
    {
        switch(key)
        {
            case key::K_0: return '0';
            case key::K_1: return '1';
            case key::K_2: return '2';
            case key::K_3: return '3';
            case key::K_4: return '4';
            case key::K_5: return '5';
            case key::K_6: return '6';
            case key::K_7: return '7';
            case key::K_8: return '8';
            case key::K_9: return '9';
            case key::K_DECIMAL: return '.';
        }
    }
    else if(window::capslockEnabled())
    {
        return std::toupper(key);
    }
    
    if((key >= K_0 && key <= K_9) || key == K_DECIMAL)
        return '\0';
    switch(key)
    {
        case key::K_ADD: return '+';
        case key::K_SUBTRACT: return '-';
        case key::K_DIVIDE: return '/';
        case key::K_MULTIPLY: return '*';
        case key::K_ENTER: return '\n';
        case key::K_EQUAL: return '=';
    }
    return key;
}
bool key::ascii_default(KeyCode key)
{
    return (key >= key::BACKSPACE && key <= key::K_9) || key == key::SPACE || key == key::APOSTROPHE || (key >= key::COMMA && key <= key::NINE)
        || (key >= key::SEMICOLON && key <= key::EQUAL) || (key >= key::A && key <= key::RIGHT_BRACKET) || key == key::GRAVE;
}

bool mouse::pressed(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].pressed;
}
bool mouse::held(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].held;
}
bool mouse::released(mouse::ButtonCode id)
{
    return g_mouse.inputs[id].held;
}

void *window::handle()
{
    return g_windows[currentWindow].data;
}
bool window::active()
{
    return g_windows[currentWindow].active;
}
bool window::capslockEnabled()
{
    return g_windows[currentWindow].capslock;
}
bool window::closing()
{
    return glfwWindowShouldClose((GLFWwindow *)g_windows[currentWindow].data);
}
bool window::decorated()
{
    return glfwGetWindowAttrib((GLFWwindow *)g_windows[currentWindow].data, GLFW_DECORATED);
}
bool window::fullscreened()
{
    return g_windows[currentWindow].screen.fullscreen;
}
bool window::maximized()
{
    return glfwGetWindowAttrib((GLFWwindow *)g_windows[currentWindow].data, GLFW_MAXIMIZED);
}
bool window::numpadEnabled()
{
    return g_windows[currentWindow].numpad;
}
bool window::resolutionUpdated()
{
    return g_windows[currentWindow].screen.resolutionUpdated;
}
bool window::throwError()
{
    uint32_t error = glGetError();
    std::string errorMessage;
    switch(error)
    {
        case 0:
            errorMessage = "SUCCESS (GL)";
        break;
        case 1280:
            errorMessage = "ERROR :: 1280 :: Invalid enumeration parameter.";
        break;
        case 1281:
            errorMessage = "ERROR :: 1281 :: Invalid value parameter.";
        break;
        case 1282:
            errorMessage = "ERROR :: 1282 :: Invalid command state for given parameter.";
        break;
        case 1283:
            errorMessage = "ERROR :: 1283 :: Stack overflow.";
        break;
        case 1284:
            errorMessage = "ERROR :: 1284 :: Stack underflow.";
        break;
        case 1285:
            errorMessage = "ERROR :: 1285 :: Memory could not be allocated.";
        break;
        case 1286:
            errorMessage = "ERROR :: 1286 :: Framebuffer incomplete.";
        break;
    }
    std::cout << errorMessage << '\n';
    return error;
}
bool window::throwAudioError()
{
    return 0;
}
bool window::vsyncEnabled()
{
    return g_windows[currentWindow].vsyncEnabled;
}
void window::centerWindow()
{
    setPosition(monitorCenter() - Vector2I(0.5*g_windows[currentWindow].width, 0.5*g_windows[currentWindow].height));
}
void window::clearScreen(const Color& color)
{
    g_windows[currentWindow].screen.clear(color);
}
void window::close()
{
    glfwSetWindowShouldClose((GLFWwindow *)g_windows[currentWindow].data, true);
}
void window::enableCapslock(bool enable)
{
    g_windows[currentWindow].capslock = enable;
}
void window::enableDecoration(bool enable)
{
    glfwSetWindowAttrib((GLFWwindow *)g_windows[currentWindow].data, GLFW_DECORATED, enable);
}
void window::enableNumpad(bool enable)
{
    g_windows[currentWindow].numpad = enable;
}
void window::enableVSync(bool enable)
{
    glfwSwapInterval(g_windows[currentWindow].vsyncEnabled = enable);
}
void window::fullscreen(bool enable)
{
    if(enable && !g_windows[currentWindow].screen.fullscreen)
    {
        g_windows[currentWindow].screen.fullscreen = true;
        glfwGetWindowPos((GLFWwindow *)g_windows[currentWindow].data, &g_windows[currentWindow].screen.lastPosition.x, &g_windows[currentWindow].screen.lastPosition.y);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor((GLFWwindow *)g_windows[currentWindow].data, monitor, 0, 0, mode->width, mode->height, g_windows[currentWindow].vsyncEnabled ? mode->refreshRate : GLFW_DONT_CARE);
    }
    else if(!enable && g_windows[currentWindow].screen.fullscreen)
    {
        g_windows[currentWindow].screen.fullscreen = false;
        glfwSetWindowMonitor((GLFWwindow *)g_windows[currentWindow].data, nullptr, 0, 0, g_windows[currentWindow].screen.resolution.x, g_windows[currentWindow].screen.resolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_windows[currentWindow].data, g_windows[currentWindow].screen.lastPosition.x, g_windows[currentWindow].screen.lastPosition.y);
    }
}
float window::gamma()
{
    return g_windows[currentWindow].screen.gamma;
}
void window::hideCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)g_windows[currentWindow].data, GLFW_CURSOR, enable ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}
void window::lockCursor(bool enable)
{
    glfwSetInputMode((GLFWwindow *)g_windows[currentWindow].data, GLFW_CURSOR, enable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void window::maximize()
{
    if(g_windows[currentWindow].screen.fullscreen)
    {
        glfwSetWindowMonitor((GLFWwindow *)g_windows[currentWindow].data, nullptr, 0, 0, g_windows[currentWindow].screen.resolution.x, g_windows[currentWindow].screen.resolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_windows[currentWindow].data, g_windows[currentWindow].screen.lastPosition.x, g_windows[currentWindow].screen.lastPosition.y);
        g_windows[currentWindow].screen.fullscreen = false;
    }

    glfwMaximizeWindow((GLFWwindow *)g_windows[currentWindow].data);
}
void window::minimize()
{
    if(g_windows[currentWindow].screen.fullscreen)
    {
        glfwSetWindowMonitor((GLFWwindow *)g_windows[currentWindow].data, nullptr, 0, 0, g_windows[currentWindow].screen.resolution.x, g_windows[currentWindow].screen.resolution.y, GLFW_DONT_CARE);
        glfwSetWindowPos((GLFWwindow *)g_windows[currentWindow].data, g_windows[currentWindow].screen.lastPosition.x, g_windows[currentWindow].screen.lastPosition.y);
        g_windows[currentWindow].screen.fullscreen = false;
    }

    glfwRestoreWindow((GLFWwindow *)g_windows[currentWindow].data);
}
void window::poll()
{
    glfwPollEvents();
}
void window::refresh()
{
    glfwSwapBuffers((GLFWwindow *)g_windows[currentWindow].data);
}
void window::remove()
{
    g_windows[currentWindow].screen.remove();
}
void window::setActiveWindow(uint32_t index)
{
    currentWindow = index;
}
void window::setCamera(uint32_t newCamera)
{
    g_windows[currentWindow].screen.camera = newCamera;
}
void window::setCursor(const Vector2& position)
{
    glfwSetCursorPos((GLFWwindow *)g_windows[currentWindow].data, position.x, position.y);
}
void window::setDefaultBackgroundColor(const Color &color)
{
    g_windows[currentWindow].screen.defaultColor = color;
    g_windows[currentWindow].screen.refreshResolution(g_windows[currentWindow].width, g_windows[currentWindow].height);
}
void window::setGamma(float value)
{
    g_windows[currentWindow].screen.gamma = value;
}
void window::setIcon(const char *path)
{
    GLFWimage images[1];

    stbi_set_flip_vertically_on_load(false);
    images[0].pixels = stbi_load((source::root() + std::string("resources/images/") + path).c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon((GLFWwindow *)g_windows[currentWindow].data, 1, images); 
    stbi_image_free(images[0].pixels);
    stbi_set_flip_vertically_on_load(true);
}
void window::setOpacity(float opacity)
{
    glfwSetWindowOpacity((GLFWwindow *)g_windows[currentWindow].data, opacity);
}
void window::setPosition(const Vector2I& position)
{
    glfwSetWindowPos((GLFWwindow *)g_windows[currentWindow].data, position.x, position.y);
}
void window::setSize(int width, int height)
{
    glfwSetWindowSize((GLFWwindow *)g_windows[currentWindow].data, width, height);
}
void window::setTitle(const char *title)
{
    glfwSetWindowTitle((GLFWwindow *)g_windows[currentWindow].data, title);
}
void window::storeRender()
{
    g_windows[currentWindow].screen.store();
}
void window::drawRender()
{
    g_windows[currentWindow].screen.draw();
}
void window::terminate()
{
    glfwTerminate();
}
void window::updatedResolution(bool updated)
{
    g_windows[currentWindow].screen.resolutionUpdated = updated;
}
float window::aspectRatioInv()
{
    return (float)g_windows[currentWindow].width/(float)g_windows[currentWindow].height;
}
float window::aspectRatio()
{
    return (float)g_windows[currentWindow].height/(float)g_windows[currentWindow].width;
}
float window::getOpacity()
{
    return glfwGetWindowOpacity((GLFWwindow *)g_windows[currentWindow].data);
}
float window::height()
{
    return g_windows[currentWindow].height;
}
float window::width()
{
    return g_windows[currentWindow].width;
}
Vector2 window::cursorUniformScreenPosition()
{
    return {((float)g_windows[currentWindow].cursorPosition.x/(float)g_windows[currentWindow].width)*2 - 1, -((float)g_windows[currentWindow].cursorPosition.y/(float)g_windows[currentWindow].height)*2 + 1};
}
Vector2 window::cursorScreenPosition()
{
    return cursorUniformScreenPosition() * Vector2{aspectRatioInv(), 1};
}
Vector2I window::center()
{
    return Vector2I(monitorCenter() - Vector2I(0.5*g_windows[currentWindow].width, 0.5*g_windows[currentWindow].height));
}
Vector2I window::resolution()
{
    return Vector2I(g_windows[currentWindow].width, g_windows[currentWindow].height);
}
Vector2I window::monitorCenter()
{
    Vector2I result;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &result.x, &result.y);
    return result * 2;
}
int32_t window::getMaximumSamples()
{
    return g_windows[currentWindow].screen.getMaximumSamples();
}
uint32_t window::camera()
{
    return g_windows[currentWindow].screen.camera;
}
DirectionalLight& window::lighting()
{
    return g_windows[currentWindow].screen.dirLight;
}
DirectionalLight& window::setLighting(const DirectionalLight& light)
{
    return(g_windows[currentWindow].screen.dirLight = light);
}

void initializeECS()
{  
    auto& pointlights = object::createSystem<PointLightManager, PointLight, Transform>({}, 3);
    pointlights.setFunction(object::fn::UPDATE, []
    (object::ecs &container, object::ecs::system &system)
    {
        std::vector<entity>& entities = container.entities<PointLightManager>();

        Shader& shader = shader::get("object_shader");
        shader.use();
        shader.setInt("totalPointLights", entities.size());

        uint32_t iterator = 0;
        for(entity e : entities)
        {
            Transform& transform = container.getComponent<Transform>(e);
            PointLight& light = container.getComponent<PointLight>(e);

            std::string name = "pointLights[" + std::to_string(iterator) + "]";
            shader.setVec3(name + ".position", transform.position);
            shader.setVec4(name + ".color", light.color);
            shader.setFloat(name + ".strength", light.strength);
            
            shader.setFloat(name + ".constant", light.constant);
            shader.setFloat(name + ".linear", light.linear);
            shader.setFloat(name + ".quadratic", light.quadratic);
            iterator++;
        }
    });

    auto& spotlights = object::createSystem<SpotLightManager, SpotLight, Transform>({}, 3);
    spotlights.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system)
    {
        const std::vector<entity>& entities = container.entities<SpotLightManager>();
        Shader& shader = shader::get("object_shader");
        shader.use();
        shader.setInt("totalSpotLights", entities.size());

        uint32_t iterator = 0;
        for(entity e : entities)
        {
            Transform& transform = container.getComponent<Transform>(e);
            SpotLight& light = container.getComponent<SpotLight>(e);

            std::string name = "spotLights[" + std::to_string(iterator) + "]";
            shader.setVec3(name + ".position", transform.position);
            shader.setVec3(name + ".direction", light.direction);
            shader.setVec4(name + ".color", light.color);
            shader.setFloat(name + ".strength", light.strength);
            
            shader.setFloat(name + ".constant", light.constant);
            shader.setFloat(name + ".linear", light.linear);
            shader.setFloat(name + ".quadratic", light.quadratic);

            shader.setFloat(name + ".cutOff", light.cutoff);
            shader.setFloat(name + ".outerCutOff", light.outerCutOff);
            iterator++;
        }
    });

    auto& physics = object::createSystem<PhysicsManager, Physics2D, Transform>({}, 6);
    physics.setFunction(object::fn::START, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<PhysicsManager>())
        {
            Physics2D& physics = container.getComponent<Physics2D>(e);
            physics.delta = physics.lastDelta = container.getComponent<Transform>(e).position;
            physics.time.begin();
        }
    });
    physics.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<PhysicsManager>())
        {
            Vector3& position = container.getComponent<Transform>(e).position;
            Physics2D& physics = container.getComponent<Physics2D>(e);

            physics.time.update(physics.maxDeltaTime);
            float time = physics.time.interval;

            while(physics.time.timer > physics.time.interval)
            {
                Vector3 acceleration = physics.acceleration();
                Vector3 deltaStep = vec3::roundTo(physics.lastDelta - physics.delta, 5);
                if((physics.collisions[physics::LEFT] && deltaStep.x >= 0) || (physics.collisions[physics::RIGHT] && deltaStep.x <= 0))
                {
                    physics.delta.x = position.x;
                }
                if((physics.collisions[physics::DOWN] && deltaStep.y >= 0) || (physics.collisions[physics::UP] && deltaStep.y <= 0))
                {
                    physics.delta.y = position.y;
                }
                
                physics.lastDelta = physics.delta;

                if(physics.dragDirection && (physics.collisions[physics::UP] || physics.collisions[physics::DOWN]) && math::sign0(physics.force.x) != math::sign0(physics.velocity.x))
                {
                    physics.velocity.x = physics.velocity.x / (1 + physics.drag.x * time);
                }
                else if(!physics.dragDirection && (physics.collisions[physics::RIGHT] || physics.collisions[physics::LEFT]) && math::sign0(physics.force.y) != math::sign0(physics.velocity.y))
                {
                    physics.velocity.y = physics.velocity.y / (1 + physics.drag.y * time);
                }

                if(physics.terminal != 0)
                {
                    physics.velocity = vec3::min(vec3::abs(physics.velocity + acceleration * time), physics.terminal) * vec3::sign(physics.velocity + acceleration * time);
                }
                else
                {
                    physics.velocity += acceleration * time;
                }
                
                physics.velocity += physics.impulse;

                physics.delta += Vector3(physics.velocity * time + acceleration * 0.5f * time * time);
                physics.time.reset();
                physics.resetImpulse();
            }

            position = vec3::lerp(physics.lastDelta, physics.delta, physics.time.timer / physics.time.interval);
            physics.resetForce();
        }
    });

    auto& collisions = object::createSystem<CollisionManager, BoxCollider, Transform>({}, 15);
    object::setInsertion<CollisionManager>([]
    (entity e, std::vector<entity>& entities, std::vector<size_t>& maps)
    {
        object::defaultInsertion(e, entities, maps);
    
        BoxCollider& collider = object::getComponent<BoxCollider>(e);
        collider.storedPosition = object::getComponent<Transform>(e).position + collider.offset;
    });
    collisions.setFunction(object::fn::RENDER, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<CollisionManager>())
        {
            BoxCollider& collider = container.getComponent<BoxCollider>(e);
            Transform& transform = container.getComponent<Transform>(e);
            collider.storedPosition = transform.position + collider.offset;
        }
    });

    auto& aabb = object::createSystem<AABBHandler, AABB, BoxCollider, Physics2D, Transform>({}, 9);
    aabb.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<AABBHandler>())
        {
            BoxCollider& collider = container.getComponent<BoxCollider>(e);
            Transform& transform = container.getComponent<Transform>(e);
            bool triggered = false;
            if(collider.mobile)
            {
                Vector3 position = transform.position + collider.offset;
                Vector3 boxDim = (transform.scale * collider.scale + vec3::abs(position - collider.storedPosition)) * 0.5f;
    
                for(entity compare : container.entities<CollisionManager>())
                {
                    if(compare == e)
                        continue;
    
                    BoxCollider& collider2 = container.getComponent<BoxCollider>(compare);
                    Transform& transform2 = container.getComponent<Transform>(compare);
    
                    Vector3 position2 = transform2.position + collider2.offset;
                    Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;
    
                    int precision = 6;
                    Vector3 positive = vec3::roundTo(position + boxDim, precision), negative = vec3::roundTo(position - boxDim, precision);
                    Vector3 positive2 = vec3::roundTo(position2 + boxDim2, precision), negative2 = vec3::roundTo(position2 - boxDim2, precision);
                    if(positive.x >= negative2.x && negative.x <= positive2.x && positive.y >= negative2.y && negative.y <= positive2.y && positive.z >= negative2.z && negative.z <= positive2.z)
                    {                     
                        bool edge = (positive.x == negative2.x || negative.x == positive2.x || positive.y == negative2.y || negative.y == positive2.y);
                        physics::collisionTrigger(e, compare, edge, triggered);
                        // collider2.trigger(compare, e, edge, triggered);
                        collider.enter = collider2.enter = triggered = true;
                    }
                }
                if(!triggered)
                {
                    physics::collisionMiss(e);
                    collider.enter = false;
                }
            }
            collider.storedPosition = transform.position + collider.offset;
        }
    });

    auto& aabb2D = object::createSystem<AABB2DHandler, AABB2D, BoxCollider, Physics2D, Transform>({}, 9);
    aabb2D.setFunction(object::fn::UPDATE, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<AABB2DHandler>())
        {
            BoxCollider& collider = container.getComponent<BoxCollider>(e);
            Transform& transform = container.getComponent<Transform>(e);

            bool triggered = false;
            Vector3 position = transform.position + collider.offset;
            Vector3 boxDim = (transform.scale * collider.scale + vec3::abs(position - collider.storedPosition)) * 0.5f;
    
            for(entity compare : container.entities<CollisionManager>())
            {
                if(compare == e)
                    continue;

                BoxCollider& collider2 = container.getComponent<BoxCollider>(compare);
                Transform& transform2 = container.getComponent<Transform>(compare);

                Vector3 position2 = transform2.position + collider2.offset;
                Vector3 boxDim2 = transform2.scale * collider2.scale * 0.5f;

                int precision = 6;
                Vector3 positive = vec3::roundTo(position + boxDim, precision), negative = vec3::roundTo(position - boxDim, precision);
                Vector3 positive2 = vec3::roundTo(position2 + boxDim2, precision), negative2 = vec3::roundTo(position2 - boxDim2, precision);
                if(positive.x >= negative2.x && negative.x <= positive2.x && positive.y >= negative2.y && negative.y <= positive2.y)
                {          
                    bool edge = (positive.x == negative2.x || negative.x == positive2.x || positive.y == negative2.y || negative.y == positive2.y);
                    physics::collisionTrigger(e, compare, edge, triggered);
                    // collider2.trigger(compare, e, edge, triggered);
                    collider.enter = collider2.enter = triggered = true;
                }
            }
            if(!triggered)
            {
                physics::collisionMiss(e);
                collider.enter = false;
            }
        }
    });

    auto& cameras = object::createSystem<CameraManager, Camera, Transform>({}, 18);
    void (*cameraFunction)(object::ecs &, object::ecs::system &) = []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<CameraManager>())
        {
            Camera& camera = container.getComponent<Camera>(e);
            camera.view = mat4::lookAt(container.getComponent<Transform>(e).position, -camera.front, vec3::up);

            if(window::resolutionUpdated())
            {
                camera.projection = mat4::inter(math::radians(45.0f), 2.5f, window::aspectRatioInv(), camera.nearDistance, camera.farDistance, 1);
            }
        }
    };
    cameras.setFunction(object::fn::LOAD, cameraFunction);
    cameras.setFunction(object::fn::LATE_UPDATE, cameraFunction);

    auto& billboards = object::createSystem<BillboardManager, Billboard, Transform>({}, 21);
    billboards.setFunction(object::fn::LATE_UPDATE, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<BillboardManager>())
        {
            Billboard& billboard = container.getComponent<Billboard>(e);
            Transform& transform = container.getComponent<Transform>(e);
            transform.rotation = Quaternion(mat4::lookAt(transform.position, ((container.getComponent<Transform>(billboard.target).position - transform.position) * billboard.limit).normalized(), vec3::up)).inverted();
        }
    });

    // auto& animations = object::createSystem<AnimationManager, Animator, Model>({}, 24);
    // animations.setFunction(object::fn::FIXED_UPDATE, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     for(Entity entity : container.entities<AnimationManager>())
    //     {
    //         Animator& animator = container.getComponent<Animator>(entity);
    //         Animation& animation = animator.transition().transition();
    //         container.getComponent<Model>(entity).texture.texture = animation.frames[animation.step()].texture;
    //     }
    // });

    auto& meshes = object::createSystem<MeshManager, MeshAddon, Transform, Model>({}, 27);
    meshes.setFunction(object::fn::START, []
    (object::ecs & container, object::ecs::system &system)
    {
        for(entity e : container.entities<MeshManager>())
        {
            Model model = container.getComponent<Model>(e);
            MeshAddon addon = container.getComponent<MeshAddon>(e);
            model.append(container.getComponent<Transform>(e), addon.additions);
            object::setComponent<Model>(e, model);
        }
    });

    // auto& uis = object::createSystem<UIManager, Rect, Sprite>({}, 30);
    // uis.setFunction(object::fn::START, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     for(Entity entity : container.entities<UIManager>())
    //     {
    //         container.getComponent<Sprite>(entity).refresh();
    //     }
    // });
    // uis.setFunction(object::fn::RENDER, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     Camera& camera = container.getComponent<Camera>(window::camera());
    //     Transform& cameraTransform = container.getComponent<Transform>(window::camera());
    //     uint32_t shaderID = -1;
    //
    //     glDisable(GL_DEPTH_TEST);
    //     for(Entity entity : container.entities<UIManager>())
    //     {
    //         shaderID = container.getComponent<Sprite>(entity).render(entity, shaderID, camera, cameraTransform);
    //     }
    //     glEnable(GL_DEPTH_TEST);
    // });

    auto& graphics = object::createSystem<GraphicsManager, Transform, Model>({}, 33);
    // object::setInsertion<GraphicsManager>([]
    // (Entity entity, std::vector<Entity>& entities, std::vector<size_t>& map)
    // {
    //     if(std::find(entities.begin(), entities.end(), entity) == entities.end())
    //     {
    //         entities.push_back(entity);
    //         std::sort(entities.begin(), entities.end(), []
    //         (Entity one, Entity two)
    //         {
    //             Vector3 cameraPosition = object::getComponent<Transform>(window::camera()).position;
    //
    //             Model modelOne = object::getComponent<Model>(one);
    //             Model modelTwo = object::getComponent<Model>(two);
    //            
    //             return (modelOne.getSorting() < modelTwo.getSorting());
    //         });
    //
    //         map = std::vector<size_t>(map.size(), -1);
    //         for(int i=0; i<entities.size(); i++)
    //         {
    //             map[entities[i]] = i;
    //         }
    //     }
    // });
    graphics.setFunction(object::fn::START, []
    (object::ecs &container, object::ecs::system &system)
    {
        for(entity e : container.entities<GraphicsManager>())
        {
            container.getComponent<Model>(e).refresh();
        }
    });

    auto& simpleRendering = object::createSystem<SimpleRenderer, Transform, Model, SimpleShader>({}, 33);
    simpleRendering.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system)
    {
        Camera& camera = container.getComponent<Camera>(window::camera());
        Transform& cameraTransform = container.getComponent<Transform>(window::camera());

        Shader& shader = shader::get("simple_shader");
        shader.use();

        for(entity e : container.entities<SimpleRenderer>())
        {
            shader::simple(shader, object::getComponent<Transform>(e), camera, cameraTransform, object::getComponent<SimpleShader>(e));
            container.getComponent<Model>(e).render();
        }
    });

    auto& advancedRendering = object::createSystem<AdvancedRenderer, Transform, Model, AdvancedShader>({}, 33);
    advancedRendering.setFunction(object::fn::RENDER, []
    (object::ecs &container, object::ecs::system &system)
    {
        Camera& camera = container.getComponent<Camera>(window::camera());
        Transform& cameraTransform = container.getComponent<Transform>(window::camera());

        Shader& shader = shader::get("object_shader");
        shader.use();

        for(entity e : container.entities<AdvancedRenderer>())
        {
            shader::advanced(shader, object::getComponent<Transform>(e), camera, cameraTransform, container.getComponent<AdvancedShader>(e));
            container.getComponent<Model>(e).render();
        }
    });

    // auto& buttons = object::createSystem<ButtonManager, Button, Rect>({}, 36);
    // buttons.setFunction(object::fn::UPDATE, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     for(Entity entity : container.entities<ButtonManager>())
    //     {
    //         Rect& rect = container.getComponent<Rect>(entity);
    //         Button& button = container.getComponent<Button>(entity);
    //         if(mouse::pressed(mouse::LEFT) && rect.contains(window::cursorScreenPosition()))
    //         {
    //             button.trigger(entity);
    //         }
    //         else
    //         {
    //             for(auto key : button.keyInputs)
    //             {
    //                 if(key::pressed((key::KeyCode)key))
    //                 {
    //                     button.trigger(entity);
    //                 }
    //             }
    //
    //             for(auto click : button.buttonInputs)
    //             {
    //                 if(mouse::pressed((mouse::ButtonCode)click))
    //                 {
    //                     button.trigger(entity);
    //                 }
    //             }
    //         }
    //     }
    // });

    // auto& texts = object::createSystem<TextManager, Text, Rect>({}, 36);
    // texts.setFunction(object::fn::START, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     for(Entity entity : container.entities<TextManager>())
    //     {
    //         container.getComponent<Text>(entity).initialize(container.getComponent<Rect>(entity));
    //     }
    // });
    // texts.setFunction(object::fn::RENDER, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     Camera& camera = container.getComponent<Camera>(window::camera());
    //     Transform& cameraTransform = container.getComponent<Transform>(window::camera());
    //
    //     glDisable(GL_DEPTH_TEST);
    //     for(Entity entity : container.entities<TextManager>())
    //     {
    //         Text& text = container.getComponent<Text>(entity);
    //         if(text.update || window::resolutionUpdated())
    //         {
    //             text.refresh(container.getComponent<Rect>(entity));
    //             text.update = false;
    //         }
    //
    //         text.render(entity, camera, cameraTransform);
    //     }
    //     glEnable(GL_DEPTH_TEST);
    // });
    // texts.setFunction(object::fn::DESTROY, []
    // (object::ecs & container, object::ecs::system &system)
    // {
    //     for(Entity entity : container.entities<TextManager>())
    //     {
    //         container.getComponent<Text>(entity).destroy();
    //     }
    // });
}

ProjectManager::ProjectManager()
{
    if(g_windows.size() > 0)
        return;
         
    if(!glfwInit())
    {
        std::cout << "ERROR :: GLFW could not be initialized." << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwSetErrorCallback(error_callback);

    g_windows.push_back(Window("default", 800, 600));

    texture::load("default", {Color8(255, 255, 255, 255)}, 1, 1, texture::RGBA, texture::PNG);
    mesh::load("square", shape::square());
    mesh::load("cube", shape::cube());
    mesh::load("sphere", shape::sphere(1));

    file::loadFilesInDirectory(source::root() + source::font(), ttf::load);

    std::vector<std::string> shaderConfig = file::loadFileToStringVector(source::config() + "shader.config");
    for(const auto& line : shaderConfig)
    {
        int index = line.find(':');
        if(index != std::string::npos)
        {
            int comma = line.find(',');
            shader::load(line.substr(0, index), Shader(line.substr(index+2, comma - index-2), line.substr(comma+2)));
        }
    }

    shader::get("object_shader").use();
    shader::get("object_shader").setInt("material.diffuse", 0);
    
    shader::get("simple_shader").use();
    shader::get("simple_shader").setInt("material.texture", 0);
    
    shader::get("ui_shader").use();
    shader::get("ui_shader").setInt("text", 0);
    
    (g_windows[currentWindow].screen.quad = mesh::get("square")).refresh();

    g_keyboard.initialize(key::LAST);
    g_mouse.initialize(mouse::LAST);

    object::setFunctionDefinitions({&object::fn::LOAD, &object::fn::START, &object::fn::UPDATE, &object::fn::LATE_UPDATE, &object::fn::FIXED_UPDATE, &object::fn::RENDER, &object::fn::DESTROY});
    initializeECS();
}