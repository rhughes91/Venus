#include "ui.h"

bool Rect::contains(const Vector2& vec, const Vector2& dim) const
{
    Vector2 pos = relativePosition(dim);
    Vector2 target = vec - Vector2((dim.x - dim.y)/dim.y * pos.x, 0);
    float angle = rotation.euler().x*2 - M_PI;

    Vector2 adjusted = adjustedScale(dim);
    Vector2 ul = pos - adjusted * 0.5f, br = pos + adjusted * 0.5f;
    return math::quadPointIntersect(Quad(vec2::rotatedAround(ul, pos, angle), vec2::rotatedAround(Vector2(br.x, ul.y), pos, angle), vec2::rotatedAround(br, pos, angle), vec2::rotatedAround(Vector2(ul.x, br.y), pos, angle)), target);
}
Vector2 Rect::adjustedScale(const Vector2& dim) const
{
    return scaled() ? scale * 0.1f : (scale*5) / dim.y;
}
Vector2 Rect::relativePosition(const Vector2& dim) const
{
    Vector2 origin = relativeOrigin();
    return (position + origin - adjustedScale(dim) * origin * Vector2(dim.y/dim.x, 1) * 0.5f);
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

    return Texture::loadTo(render, size * 8, size * 8, Texture::RGBA, Texture::PNG, Texture::POINT).texture;
}

Text::Text(const Font& font__, const std::string& text__, float scale__, const Alignment& alignment__, const Shader& shader__, const Color& color__) : font(font__), text(text__), color(color__), scale(scale__), alignment(alignment__), textShader(shader__)
{
    spacing = -0.025f;
    linePadding = 0.05f;
    newLineSetting = text::WORD;

    reinit();
}
void Text::initialize(const Rect& bounds, const Vector2& dim)
{
    struct LineNode
    {
        int start, end;
        Vector2 position;
    };
    std::vector<LineNode> nodes;

    Vector2 position = 0, boundsScale = bounds.adjustedScale(dim);
    std::vector<float> text0, text1;
    std::vector<Vector3> vectors0, vectors1;

    int start = 0, wordEnd;
    float scaling = (float)1 / font.unitsPerEm;
    float lineSpace = 0, modifier = (scale*5) / dim.y, tabWidth = 4*(font.characters[' '].lsb + font.characters[' '].rsb) * scaling, addon = 0, wordAddon;
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
void Text::refresh(const Rect& bounds, const Vector2& dim)
{
    struct LineNode
    {
        int start, end;
        Vector2 position;
    };
    std::vector<LineNode> nodes;

    float aspectRatioInv = dim.x/dim.y;

    Vector2 position = 0, rawScale = bounds.adjustedScale(dim), boundsScale = rawScale * aspectRatioInv;
    std::vector<float> text0, text1;
    std::vector<Vector3> vectors0, vectors1;

    int start = 0, wordEnd;
    float scaling = (float)1 / font.unitsPerEm;
    float lineSpace = 0, modifier = (scale*5) / dim.y, tabWidth = 4*(font.characters[' '].lsb + font.characters[' '].rsb) * scaling, addon = 0, wordAddon;

    float aspectOffset = (rawScale.x - lineSpace*modifier) / modifier * 0.5f * (1-aspectRatioInv);
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
    float aspectYOffset = ((boundsScale.y + height*modifier) / modifier * 0.5f - font.maxScale.y * scaling * 0.25f) * (1-(1/aspectRatioInv));
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