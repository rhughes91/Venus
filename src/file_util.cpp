#define NOMINMAX

#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <windows.h>

#include "audio.h"
#include "graphics.h"
#include "ui.h"

#include "file_util.h"

std::string file::loadFileToString(const std::string &fileName)
{
    std::string content = "", line;
    std::ifstream myFile;

    myFile.open(Source::root()+fileName);
    if(myFile.is_open())
    {
        while (std::getline(myFile, line))
        {
            content += line + '\n';
        }
        myFile.close();
    }
    else
    {
        std::cout << "ERROR :: " << Source::root()+fileName << " could not be opened." << std::endl;
    }
    return content;
}

std::vector<std::string> file::loadFileToStringVector(const std::string &fileName)
{
    std::vector<std::string> content;
    std::string line;
    std::ifstream myFile;

    myFile.open(Source::root()+fileName);
    if(myFile.is_open())
    {
        while (std::getline(myFile, line))
        {
            content.push_back(line);
        }
        myFile.close();
    }
    else
    {
        std::cout << "ERROR :: " << fileName << " could not be opened." << std::endl;
    }
    return content;
}

Mesh file::loadObjFile(const std::string &fileName)
{
    std::vector<Vertex> vertices;
    int tri[3][3];

    std::vector<Vector3> inVertices, inNormals;
    std::vector<Vector2> inUVs;

    Vector3 dimensions;
    Vector3 bufferVert, bufferNormal;
    Vector2 bufferUV;

    Vector3 average = 0;
    
    std::string line;
    std::ifstream myFile;
    myFile.open(Source::root() + Source::model() + fileName);
    if(myFile.is_open())
    {
        float minX, maxX, minY, maxY, minZ, maxZ;
        bool first = true;
        while (std::getline(myFile, line))
        {
            if(line.rfind("vt", 0) == 0)
            {
                sscanf((line.substr(3)).c_str(), "%f %f", &bufferUV.x, &bufferUV.y);
                inUVs.push_back(bufferUV);
            }
            else if(line.rfind("vn", 0) == 0)
            {
                sscanf((line.substr(3)).c_str(), "%f %f %f", &bufferNormal.x, &bufferNormal.y, &bufferNormal.z);
                inNormals.push_back(bufferNormal);
            }
            else if(line.rfind("v", 0) == 0)
            {
                sscanf((line.substr(2)).c_str(), "%f %f %f", &bufferVert.x, &bufferVert.y, &bufferVert.z);
                if(first)
                {
                    minX = maxX = bufferVert.x;
                    minY = maxY = bufferVert.y;
                    minZ = maxZ = bufferVert.z;
                    first = false;
                }
                else
                {
                    minX = std::min(minX, bufferVert.x);
                    minY = std::min(minY, bufferVert.y);
                    minZ = std::min(minZ, bufferVert.z);

                    maxX = std::max(maxX, bufferVert.x);
                    maxY = std::max(maxY, bufferVert.y);
                    maxZ = std::max(maxZ, bufferVert.z);
                }

                average = (inVertices.size() * average + bufferVert) / (inVertices.size()+1);
                inVertices.push_back(bufferVert);
            }
            else if(line.rfind("f", 0) == 0)
            {
                sscanf((line.substr(1)).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", &tri[0][0], &tri[0][1], &tri[0][2], &tri[1][0], &tri[1][1], &tri[1][2], &tri[2][0], &tri[2][1], &tri[2][2]);
                vertices.push_back(Vertex{inVertices[tri[0][0]-1], inNormals[tri[0][2]-1], inUVs[tri[0][1]-1]});
                vertices.push_back(Vertex{inVertices[tri[1][0]-1], inNormals[tri[1][2]-1], inUVs[tri[1][1]-1]});
                vertices.push_back(Vertex{inVertices[tri[2][0]-1], inNormals[tri[2][2]-1], inUVs[tri[2][1]-1]});
            }
        }
        float maxXZ = std::max(maxX-minX, maxZ-minZ);
        dimensions = Vector3(maxXZ, maxY-minY, maxXZ);
        myFile.close();
    }
    else
    {
        std::cout << "ERROR :: " << fileName << " could not be opened." << std::endl;
    }
    return Mesh(vertices, dimensions, average);
}

void file::loadFilesInDirectory(const std::string &fileName, void(*load)(const std::string&))
{
    for (const auto& entry : std::filesystem::directory_iterator(fileName))
    {
        load(entry.path().string());
    }
}


void Font::load(const std::string &fileName)
{
    int index = fileName.find_last_of('/')+1;
    loadedFonts[fileName.substr(index, fileName.find_last_of('.')-index) + ".ttf"] = file::loadTTF(fileName);
}
Font& Font::get(const std::string &fileName)
{
    if(!loadedFonts.count(fileName))
        std::cout << "ERROR :: TTF file " << fileName << " could not be found." << std::endl;
    return loadedFonts[fileName];
}

void Source::initialize()
{
    projSource = getCurrentDirectoryName();
    bool cmakeBuild = false;
    for (const auto & entry : std::filesystem::directory_iterator(projSource))
        if(entry.path().filename() == "cmake")
            cmakeBuild = true;

    // if(cmakeBuild)
    {
        std::vector<std::string> pathConfig = file::loadFileToStringVector("path.config");
        for(const auto& line : pathConfig)
        {
            int index = -1;
            if((index = line.find(':')) != std::string::npos)
            {
                std::string setting = line.substr(0, index);
                
                if(setting == "source")
                {
                    projSource += line.substr(index+2);
                }
                else if(setting == "audio")
                {
                    audioSource = line.substr(index+2) + "/";
                }
                else if(setting == "data")
                {
                    configSource = line.substr(index+2) + "/";
                }
                else if(setting == "texture")
                {
                    textureSource = line.substr(index+2) + "/";
                }
                else if(setting == "font")
                {
                    fontSource = line.substr(index+2) + "/";
                }
                else if(setting == "model")
                {
                    modelSource = line.substr(index+2) + "/";
                }
                else if(setting == "shader")
                {
                    shaderSource = line.substr(index+2) + "/";
                }
            }
        }
    }
}
std::string Source::getCurrentDirectoryName()
{
    TCHAR buffer[260] = { 0 };
    GetModuleFileName(NULL, buffer, 256);

    std::string source = std::string(buffer);
    int size = source.size();
    for(int i=0; i<size; i++)
    {
        if(source[i] == '\\')
        {
            source[i] = '/';
        }
    }

    return source.substr(0, source.find_last_of("/")) + "/";
}


struct BinaryFile
{
    std::ifstream file;
    BinaryFile(const std::string& path)
    {
        file.open(path, std::ios_base::binary);
    }

    void close()
    {
        file.close();
    }
    void seek(int32_t position)
    {
        file.seekg(position);
    }
    int32_t position()
    {
        return file.tellg();
    }

    bool open()
    {
        return file.is_open();
    }

    uint8_t toUInt8()
    {
        char c;
        file.get(c);
        return c;
    }
    uint8_t toUInt8_LE()
    {
        return toUInt8();
    }
    uint16_t toUInt16()
    {
        return ((toUInt8() << 8) | toUInt8());
    }
    uint16_t toUInt16_LE()
    {
        uint8_t one = toUInt8();
        uint8_t two = toUInt8();

        return ((two << 8) | one);
    }
    int16_t toInt16()
    {
        uint16_t number = toUInt16();
        if(number & 0x8000)
            number -= 1 << 16;
        return number;
    }
    int16_t toFWord()
    {
        return toInt16();
    }
    int16_t toUFWord()
    {
        return toUInt16();
    }
    int32_t toInt32()
    {
        return (toUInt8() << 24) | (toUInt8() << 16) | (toUInt8() << 8) | toUInt8();
    }
    uint32_t toUInt32()
    {
        return toInt32();
    }
    uint32_t toUInt32_LE()
    {
        uint8_t one = toUInt8();
        uint8_t two = toUInt8();
        uint8_t three = toUInt8();
        uint8_t four = toUInt8();
        return (four << 24) | (three << 16) | (two << 8) | one;
    }
    uint64_t toUInt64()
    {
        uint64_t result;
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        result = (result << 8) | toUInt8();
        return result;
    }
    uint64_t toUInt64_LE()
    {
        uint8_t one = toUInt8();
        uint8_t two = toUInt8();
        uint8_t three = toUInt8();
        uint8_t four = toUInt8();
        uint8_t five = toUInt8();
        uint8_t six = toUInt8();
        uint8_t seven = toUInt8();
        uint8_t eight = toUInt8();

        uint64_t result;
        result = (result << 8) | eight;
        result = (result << 8) | seven;
        result = (result << 8) | six;
        result = (result << 8) | five;
        result = (result << 8) | four;
        result = (result << 8) | three;
        result = (result << 8) | two;
        result = (result << 8) | one;
        return result;
    }
    std::string toString(int length)
    {
        std::string result;
        for (int i = 0; i < length; i++)
        {
            result += char(toUInt8());
        }
        return result;
    }
    float toFixed()
    {
        return toInt32() / (1 << 16);
    }
    uint32_t toDate()
    {
        uint32_t macTime = toUInt32() * 0x100000000 + toUInt32();
        return macTime * 1000;
    }
};

struct Glyph
{
    int16_t numberOfContours, xMin, yMin, xMax, yMax;
    std::vector<uint16_t> contourEnds;
    std::vector<Point> points;

    uint16_t offset;
};
struct Metric
{
    uint16_t advanceWidth;
    int16_t leftSideBearing;
};
struct EncodingRecord
{
    uint16_t platformID, encodingID;
    uint32_t offset;
};

struct Header
{
    uint16_t majorVersion, minorVersion;
    float fontRevision;
    uint32_t checksumAdjustment, magicNumber;
    uint16_t flags, unitsPerEm;
    uint32_t created, modified;
    int16_t xMin, yMin, xMax, yMax;
    uint16_t macStyle, lowestRecPPEM, fontDirectionHint, indexToLocFormat, glyphDataFormat;

    Header() {}
    Header(BinaryFile& file, uint32_t offset)
    {
        file.seek(offset);
        majorVersion = file.toUInt16();
        minorVersion = file.toUInt16();
        fontRevision = file.toFixed();
        checksumAdjustment = file.toUInt32();
        magicNumber = file.toUInt32();
        flags = file.toUInt16();
        unitsPerEm = file.toUInt16();
        created = file.toDate();
        modified = file.toDate();
        xMin = file.toFWord();
        yMin = file.toFWord();
        xMax = file.toFWord();
        yMax = file.toFWord();
        macStyle = file.toUInt16();
        lowestRecPPEM = file.toUInt16();
        fontDirectionHint = file.toInt16();
        indexToLocFormat = file.toInt16();
        glyphDataFormat = file.toInt16();
    }
};
struct MaxProfile
{
    float maxpVersion;
    uint16_t numGlyphs, maxPoints, maxContours, maxCompositePoints, maxCompositeContours, maxZones, maxTwilightPoints, maxStorage, maxFunctionDefs, maxInstructionDefs, maxStackElements, maxSizeOfInstructions, maxComponentElements, maxComponentDepth;

    MaxProfile() {}
    MaxProfile(BinaryFile& file, uint32_t index)
    {
        file.seek(index);
        maxpVersion = file.toFixed();
        numGlyphs = file.toUInt16();
        maxPoints = file.toUInt16();
        maxContours = file.toUInt16();
        maxCompositePoints = file.toUInt16();
        maxCompositeContours = file.toUInt16();
        maxZones = file.toUInt16();
        maxTwilightPoints = file.toUInt16();
        maxStorage = file.toUInt16();
        maxFunctionDefs = file.toUInt16();
        maxInstructionDefs = file.toUInt16();
        maxStackElements = file.toUInt16();
        maxSizeOfInstructions = file.toUInt16();
        maxComponentElements = file.toUInt16();
        maxComponentDepth = file.toUInt16();
    }

};
struct HorizontalHeader
{
    uint16_t majorVersion, minorVersion;
    int16_t ascent, descent, lineGap;
    uint16_t advanceWidthMax;
    int16_t minLeftSideBearing, minRightSideBearing, xMaxExtent, caretSlopeRise, caretSlopeRun, caretOffset, metricDataFormat;
    uint16_t numOfLongHorMetrics;

    HorizontalHeader() {}
    HorizontalHeader(BinaryFile& file, uint32_t index)
    {
        file.seek(index);
        majorVersion = file.toUInt16();
        minorVersion = file.toUInt16();
        ascent = file.toFWord();
        descent = file.toFWord();
        lineGap = file.toFWord();
        advanceWidthMax = file.toUFWord();
        minLeftSideBearing = file.toFWord();
        minRightSideBearing = file.toFWord();
        xMaxExtent = file.toFWord();
        caretSlopeRise = file.toInt16();
        caretSlopeRun = file.toInt16();
        caretOffset = file.toInt16();

        file.toInt16();file.toInt16();file.toInt16();file.toInt16(); // skip 4 reserved places

        metricDataFormat = file.toInt16();
        numOfLongHorMetrics = file.toUInt16();
    }

};
struct HorizontalMetrics
{
    std::vector<Metric> hMetrics;
    std::vector<int16_t> leftSideBearing;

    HorizontalMetrics() {}
    HorizontalMetrics(BinaryFile& file, uint32_t index, uint16_t numGlyphs, uint16_t numOfLongHorMetrics)
    {
        file.seek(index);
        for (int i = 0; i < numOfLongHorMetrics; i++) {
            hMetrics.push_back
            ({
                file.toUInt16(), file.toInt16()
            });
        }
        for (int i = 0; i < numGlyphs - numOfLongHorMetrics; i++)
        {
            leftSideBearing.push_back(file.toFWord());
        }
    }
};
struct IndexLocations
{
    std::vector<uint32_t> locations;

    IndexLocations() {}
    IndexLocations(BinaryFile& file, uint32_t index, uint16_t indexToLocFormat, uint16_t numGlyphs)
    {
        if(indexToLocFormat == 1)
        {
            for(int i = 0; i < numGlyphs + 1; i++)
            {
                file.seek(index + i*4);
                locations.push_back(file.toUInt32());
            }
        }
        else
        {
            for(int i = 0; i < numGlyphs + 1; i++)
            {
                file.seek(index + i*2);
                locations.push_back(file.toUInt16()*2);
            }
        }
    }

};
struct GlyphData
{
    std::vector<Glyph> glyphs;

    GlyphData() {}
    GlyphData(BinaryFile& file, uint32_t index, const std::vector<uint32_t>& locations, const Vector2& scale)
    {
        for(int i = 0; i < locations.size() - 1; i++)
        {
            uint32_t locaOffset = locations[i];

            file.seek(index + locaOffset);

            Glyph glyph = {file.toInt16(), file.toInt16(), file.toInt16(), file.toInt16(), file.toInt16()};
            if(glyph.numberOfContours > 0)
            {
                readSimpleGlyph(file, glyph, scale);
            }
            glyphs.push_back(glyph);
        }
    }

    private:
        void readSimpleGlyph(BinaryFile& file, Glyph& glyph, const Vector2& scale)
        {
            uint8_t ON_CURVE = 1, X_IS_BYTE = 2, Y_IS_BYTE = 4, REPEAT = 8 , X_DELTA = 16, Y_DELTA = 32;

            uint16_t numPoints = 0;
            for(int i=0; i<glyph.numberOfContours; i++)
            {
                uint16_t contourEnd = file.toUInt16();
                glyph.contourEnds.push_back(contourEnd);

                if(contourEnd > numPoints)
                {
                    numPoints = contourEnd+1;
                }
            }

            file.seek(file.toUInt16() + file.position());

            if(glyph.numberOfContours == 0)
                return;

            std::vector<uint8_t> flags;
            for(int i=0; i<numPoints; i++)
            {
                uint8_t flag = file.toUInt8();
                flags.push_back(flag);
                glyph.points.push_back(Point((flag & ON_CURVE) > 0));
                if(flag & REPEAT)
                {
                    uint8_t repeatCount = file.toUInt8();
                    i += repeatCount;
                    while(repeatCount--)
                    {
                        flags.push_back(flag);
                        glyph.points.push_back(Point((flag & ON_CURVE) > 0));
                    }
                }
            }

            int32_t value = 0;
            for(int i=0; i<numPoints; i++)
            {
                if(flags[i] & X_IS_BYTE)
                {
                    if(flags[i] & X_DELTA)
                    {
                        value += file.toUInt8();
                    }
                    else
                    {
                        value -= file.toUInt8();
                    }
                }
                else if(~(flags[i]) & X_DELTA)
                {
                    value += file.toInt16();
                }

                glyph.points[i].position.x = value * scale.x;
            }

            value = 0;
            for(int i=0; i<numPoints; i++)
            {
                if(flags[i] & Y_IS_BYTE)
                {
                    if(flags[i] & Y_DELTA)
                    {
                        value += file.toUInt8();
                    }
                    else
                    {
                        value -= file.toUInt8();
                    }
                }
                else if(~(flags[i]) & Y_DELTA)
                {
                    value += file.toInt16();
                }

                glyph.points[i].position.y = value * scale.y;
            }
        }
};
struct CharacterMap
{
    uint16_t version, numTables;
    uint32_t offset;
    uint16_t format;
    
    std::vector<EncodingRecord> encodingRecords;

    uint32_t length, language = -1;
    uint16_t segCount = -1, searchRange = -1, entrySelector = -1, rangeShift = -1;
    std::unordered_map<int32_t, uint16_t> glyphIndexMap;

    uint32_t error = 0;

    CharacterMap() {}
    CharacterMap(BinaryFile& file, uint32_t index)
    {
        file.seek(index);

        version = file.toUInt16();
        numTables = file.toUInt16();

        if(version != 0)
        {
            error = file.position() < 0 ? ttf::END_OF_FILE:ttf::UNSUPPORTED_CMAP;
            return;
        }
        
        for(int i = 0; i < numTables; i++)
        {
            encodingRecords.push_back
            ({
                file.toUInt16(),
                file.toUInt16(),
                file.toUInt32()
            });
        }
        offset = -1;
        for(int i = 0; i < numTables; i++)
        {
            EncodingRecord record = encodingRecords[i];
            bool isWindowsPlatform = record.platformID == 3 && (record.encodingID == 0 || record.encodingID == 1 || record.encodingID == 10);
            bool isUnicodePlatform = record.platformID == 0 && (record.encodingID == 0 || record.encodingID == 1 || record.encodingID == 2 || record.encodingID == 3 || record.encodingID == 4);

            if (isWindowsPlatform || isUnicodePlatform)
            {
                offset = record.offset;
                break;
            }
        }
        
        if (offset == -1)
        {
            error = file.position() < 0 ? ttf::END_OF_FILE:ttf::UNSUPPORTED_PLATFORM;
            return;
        }
        
        format = file.toUInt16();
        switch(format)
        {
            case 0:
                parseFormat0(file);
            break;
            case 4:
                parseFormat4(file);
            break;
            default:
                error = file.position() < 0 ? ttf::END_OF_FILE:ttf::UNSUPPORTED_FORMAT;
            return;
        }
        glyphIndexMap[' '] = 3;
    }

    bool parseFormat0(BinaryFile& file)
    {
        length = file.toUInt16();
        language = file.toUInt16();

        for(int c=0; c<256; c++)
            glyphIndexMap[c] = (c-29) & 0xFFFF;

        return true;
    }

    bool parseFormat4(BinaryFile& file)
    {
        length = file.toUInt16();
        language = file.toUInt16();
        segCount = file.toUInt16()/2;
        searchRange = file.toUInt16();
        entrySelector = file.toUInt16();
        rangeShift = file.toUInt16();

        std::vector<uint16_t> endCodes;
        for(int i=0; i<segCount; i++)
        {
            endCodes.push_back(file.toUInt16());
        }

        if(file.toUInt16())
        {
            error = ttf::INVALID_BENCHMARK;
            return false;
        }

        std::vector<uint16_t> startCodes;
        for(int i=0; i<segCount; i++)
        {
            startCodes.push_back(file.toUInt16());
        }

        std::vector<int16_t> idDeltas;
        for(int i=0; i<segCount; i++)
        {
            idDeltas.push_back(file.toInt16());
        }

        uint16_t idRangeOffsetsStart = file.position();

        std::vector<uint16_t> idRangeOffsets;
        for(int i=0; i<segCount; i++)
        {
            idRangeOffsets.push_back(file.toUInt16());
        }

        for(int i=0; i<segCount; i++)
        {
            uint16_t endCode = endCodes[i];
            uint16_t startCode = startCodes[i];

            for(int32_t c=startCode; c<=endCode; c++)
            {                
                if(idRangeOffsets[i])
                {
                    uint16_t glyphID = *(idRangeOffsets[i]/2 + (c + startCodes[i]) + &idRangeOffsets[i]);
                    if(glyphID)
                    {
                        glyphIndexMap[c] = (glyphID + idDeltas[i]) & 0xFFFF;
                    }
                    else
                    {
                        glyphIndexMap[c] = c;
                    }
                }
                else
                {
                    glyphIndexMap[c] = (c + idDeltas[i]) & 0xFFFF;
                }
            }
        }
        return true;
    }
};
struct TTF
{
    uint32_t scalar;
    uint16_t numTables, searchRange, entrySelector, rangeShift;

    std::unordered_map<std::string, uint32_t[3]> tables;

    uint32_t errorCode = 0;

    Header head;
    MaxProfile maxp;
    HorizontalHeader hhea;
    HorizontalMetrics hmtx;
    IndexLocations loca;
    GlyphData glyf;
    CharacterMap cmap;

    TTF(BinaryFile& file)
    {
        scalar = file.toUInt32();
        numTables = file.toUInt16();
        searchRange = file.toUInt16();
        entrySelector = file.toUInt16();
        rangeShift = file.toUInt16();

        if(!parseTables(file))
        {
            file.close();
            errorCode = file.position() < 0 ? ttf::END_OF_FILE:ttf::INVALID_CHECKSUM;
            return;
        }

        head = Header(file, tables["head"][1]);
        maxp = MaxProfile(file, tables["maxp"][1]);
        hhea = HorizontalHeader(file, tables["hhea"][1]);
        hmtx = HorizontalMetrics(file, tables["hmtx"][1], maxp.numGlyphs, hhea.numOfLongHorMetrics);
        loca = IndexLocations(file, tables["loca"][1], head.indexToLocFormat, maxp.numGlyphs);
        glyf = GlyphData(file, tables["glyf"][1], loca.locations, 1);
        cmap = CharacterMap(file, tables["cmap"][1]);

        if(cmap.error)
        {
            file.close();
            errorCode = cmap.error;
            return;
        }
    }

    bool parseTables(BinaryFile& file)
    {
        for (int i = 0; i < numTables; i++) 
        {
            std::string tag = file.toString(4);
            tables[tag][0] = file.toUInt32(); // checksum
            tables[tag][1] = file.toUInt32(); // offset
            tables[tag][2] = file.toUInt32(); // length

            if(tag != "head" && calculateTableChecksum(file, tables[tag][1], tables[tag][2]) != tables[tag][0])
            {
                return false;
            }
        }
        return true;
    }

    private:
        uint32_t calculateTableChecksum(BinaryFile& file, uint32_t offset, uint32_t length)
        {
            auto old = file.position();
            file.seek(offset);
            uint32_t sum = 0;
            int64_t nlongs = ((length + 3) / 4) | 0;

            while( nlongs-- )
            {
                sum = (sum + file.toUInt32() & 0xffffffff);
            }

            file.seek(old);
            return sum;
        }
};

Font file::loadTTF(const std::string &fileName)
{
    Font font;

    BinaryFile file(fileName);
    if(file.open())
    {
        TTF ttf = TTF(file);
        switch(ttf.errorCode)
        {
            case ttf::END_OF_FILE:
                std::cout << "ERROR :: Parser reach the end of the file " << fileName << " before data could be read." << std::endl;
            break;
            case ttf::INVALID_CHECKSUM:
                std::cout << "ERROR :: Font " << fileName << " table data could not be verified; checksum was invalidated." << std::endl;
            break;
            case ttf::INVALID_BENCHMARK:
                std::cout << "ERROR :: Benchmark value was not set to the correct value for " << fileName << ". This could indicate data corruption of the file." << std::endl;
            break;
            case ttf::UNSUPPORTED_CMAP:
                std::cout << "ERROR :: Font " << fileName << " CMAP version not supported; Current version is " << ttf.cmap.version << " but version 0 is required." << std::endl;
            break;
            case ttf::UNSUPPORTED_FORMAT:
                std::cout << "ERROR :: Font " << fileName << " does not contain any recognized platform and encoding." << std::endl;
            break;
            case ttf::UNSUPPORTED_PLATFORM:
                std::cout << "ERROR :: TTF format ("<<ttf.cmap.format<<") not supported for " << fileName << ". Format 0 or 4 is required." << std::endl;
            break;
            default:
                font.maxScale = vec2::zero;
                font.unitsPerEm = ttf.head.unitsPerEm;
                font.characters = std::vector<CharacterTTF>('~'+1);
                for(int i=0; i<='~'; i++)
                {
                    uint16_t index = ttf.cmap.glyphIndexMap[i];
                    if(index < ttf.glyf.glyphs.size() && index > 0)
                    {
                        Glyph glyph = ttf.glyf.glyphs[index];
                        Metric metric = ttf.hmtx.hMetrics[index];
                        
                        font.characters[i] = {Vector2I(glyph.xMin, glyph.yMin), Vector2I(glyph.xMax - glyph.xMin, glyph.yMax - glyph.yMin), metric.leftSideBearing, metric.advanceWidth - metric.leftSideBearing - (glyph.xMax - glyph.xMin), glyph.contourEnds, glyph.points};
                        font.maxScale = vec2::max(font.maxScale, font.characters[i].scale);
                    }
                }
            break;
        }
        file.close();
    }
    else
    {
        std::cout << "ERROR :: " << fileName << " could not be opened." << std::endl;
    }
    return font;
}

int8_t binary::bitArrayToInt8(const std::vector<bool>& arr)
{
    size_t count = arr.size();
    int8_t result = 0;
    int tmp;
    for (int i = 0; i < count; i++)
    {
        tmp = arr[i];
        result |= tmp << (count - i - 1);
    }
    return result;
}

struct HuffmanNode
{
    std::string symbol = "";
    HuffmanNode *left, *right;
};
struct HuffmanTree
{

};
void inflateBlockNoCompression(BinaryFile& file, std::vector<char> result)
{
    uint16_t size = file.toUInt16();
    uint16_t nsize = file.toUInt16();
    for(int i=0; i<size; i++)
    {
        result.push_back(file.toUInt8());
    }
}
std::vector<char> file::loadPNG(const std::string &fileName)
{
    std::vector<char> result;

    BinaryFile file(fileName);
    if(file.open())
    {
        uint64_t magicNumber = file.toUInt64();
        if(magicNumber == 0x89504e470d0a1a0a)
        {
            uint32_t width, height;
            uint8_t bitDepth, colorType, compressionMethod, filterMethod, interlacedMethod;

            uint32_t name = -1;
            while(name != 0x49454e44)
            {
                uint32_t size = file.toUInt32();
                name = file.toUInt32();
                uint8_t data[size];
                std::bitset<8> byte;

                switch(name)
                {
                    case 0x49484452:
                    {
                        width = file.toUInt32();
                        height = file.toUInt32();
                        bitDepth = file.toUInt8();
                        colorType = file.toUInt8();
                        compressionMethod = file.toUInt8();
                        filterMethod = file.toUInt8();
                        interlacedMethod = file.toUInt8();
                    }
                    break;
                    case 0x49444154:
                    {
                        uint8_t cmf = file.toUInt8();
                        byte = std::bitset<8>(cmf);
                        uint8_t cm = binary::bitArrayToInt8({byte[3], byte[2], byte[1], byte[0]});
                        uint8_t cinfo = binary::bitArrayToInt8({byte[7], byte[6], byte[5], byte[4]});

                        uint8_t flg = file.toUInt8();
                        byte = std::bitset<8>(flg);
                        uint8_t fcheck = binary::bitArrayToInt8({byte[4], byte[3], byte[2], byte[1], byte[0]});
                        uint8_t fdict = byte[5];
                        uint8_t flevel = binary::bitArrayToInt8({byte[7], byte[6]});

                        int bytesRead = 0;
                        bool bfinal = 0;

                        while(!bfinal)
                        {
                            byte = std::bitset<8>(file.toUInt8());
                            bfinal = byte[0];
                            uint8_t btype = binary::bitArrayToInt8({byte[1], byte[2]});

                            switch(btype)
                            {
                                case 0:

                                break;
                                case 1:

                                break;
                                case 2:

                                break;
                                case 3:
                                    std::cout << "ERROR :: Invalid BTYPE for PNG file " << fileName << ". Entry is either corrupted or is not a PNG file." << std::endl;
                                break;
                            }

                            bytesRead++;
                        }

                        file.close();
                        return result;

                        for(int i=0; i<size-2-bytesRead; i++)
                        {
                            file.toUInt8();
                        }
                    }
                    break;
                    default:
                    {
                        for(int i=0; i<size; i++)
                        {
                            data[i] = file.toUInt8();
                        }
                    }
                    break;
                }
                uint32_t crc = file.toUInt32();
            }
        }
        else
            std::cout << "ERROR :: Benchmark value was not set to the correct value for " << fileName << ". Entry is either corrupted or is not a PNG file." << std::endl;
        file.close();
    }
    else
    {
        std::cout << "ERROR :: " << fileName << " could not be opened." << std::endl;
    }
    return result;
}

struct WAVHeader
{
    std::string ckID, WAVEID;
    uint32_t cksize;
};
struct FMT
{
    std::string ckID;
    uint32_t cksize, nSamplesPerSec, nAvgBytesPerSec;
    uint16_t wFormatTag, nChannels, nBlockAlign, wBitsPerSample;
};
WAV file::loadWAV(const std::string& fileName)
{
    WAV result;

    BinaryFile file = BinaryFile(fileName);
    if(file.open())
    {
        WAVHeader wav;
        wav.ckID = file.toString(4);
        wav.cksize = file.toUInt32_LE();
        wav.WAVEID = file.toString(4);
        
        FMT fmt;
        fmt.ckID = file.toString(4);
        fmt.cksize = file.toUInt32_LE();
        fmt.wFormatTag = file.toUInt16_LE();
        result.format = fmt.nChannels = file.toUInt16_LE();
        result.sampleRate = fmt.nSamplesPerSec = file.toUInt32_LE();
        fmt.nAvgBytesPerSec = file.toUInt32_LE();
        fmt.nBlockAlign = file.toUInt16_LE();
        result.bitsPerSample = fmt.wBitsPerSample = file.toUInt16_LE();

        switch(fmt.wFormatTag)
        {
            case 1:
                std::string dataHeader = file.toString(4);
                uint32_t size = file.toUInt32_LE();

                for(uint32_t i = 0; i<size; i++)
                {
                    result.data.push_back(file.toUInt8());
                }
            break;
        }
        file.close();
    }

    return result;
}

MP3 file::loadMP3(const std::string& fileName)
{
    MP3 result;

    BinaryFile file = BinaryFile(fileName);
    if(file.open())
    {
        uint8_t byte = file.toUInt8();
        if(byte == 255)
        {
            
        }
        else
        {
            
        }
        file.close();
    }

    return result;
}