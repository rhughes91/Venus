#define NOMINMAX

#include <iostream>
#include <algorithm>
#include <bitset>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <windows.h>

#include "file_util.h"
#include "shader.h"

extern std::string g_source;

std::string loadFileToString(const std::string &fileName)
{
    std::string content = "", line;
    std::ifstream myFile;

    myFile.open(g_source+fileName);
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
        std::cout << "ERROR :: " << g_source+fileName << " could not be opened." << std::endl;
    }
    return content;
}

std::vector<std::string> loadFileToStringVector(const std::string &fileName)
{
    std::vector<std::string> content;
    std::string line;
    std::ifstream myFile;

    myFile.open(g_source+fileName);
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
        std::cout << "ERROR :: " << fileName << " could not be opened.";
    }
    return content;
}

std::string getCurrentDirectoryName()
{
    TCHAR buffer[260] = { 0 };
    GetModuleFileName(NULL, buffer, 256);

    std::string source = std::string(buffer);
    std::replace(source.begin(), source.end(), '\\', '/');
    return source.substr(0, source.find_last_of("/")) + "/";
}

Mesh loadObjFile(const std::string &fileName, Vector2 tiling)
{
    std::vector<Vertex> vertices;
    int tri[3][3];

    std::vector<Vector3> inVertices, inNormals;
    std::vector<Vector2> inUVs;

    Vector3 dimensions;
    Vector3 bufferVert, bufferNormal;
    Vector2 bufferUV;
    
    std::string line;
    std::ifstream myFile;
    myFile.open(g_source + "resources/models/" + fileName);
    if(myFile.is_open())
    {
        float minX, maxX, minY, maxY, minZ, maxZ;
        bool first = true;
        while (std::getline(myFile, line))
        {
            if(line.rfind("vt", 0) == 0)
            {
                sscanf((line.substr(3)).c_str(), "%f %f", &bufferUV.x, &bufferUV.y);
                inUVs.push_back(bufferUV * tiling);
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
    return {vertices, dimensions};
}

template<typename T, int I>
void printBinary(T value)
{
    std::bitset<I> bits(value);
    std::cout << bits.to_string() << std::endl;
    // Storing integral values in the string:
    for(auto i: bits.to_string(char(0), char(1))) {
        std::cout << (int)i;
    }
    std::cout << std::endl;
}
uint8_t toUInt8(std::ifstream& file)
{
    char c;
    file.get(c);
    return c;
}
uint16_t toUInt16(std::ifstream& file)
{
    return ((toUInt8(file) << 8) | toUInt8(file));
}
int16_t toInt16(std::ifstream& file)
{
    uint8_t number = toUInt16(file);
    if(number & 0x8000)
        number -= 1 << 16;
    return number;
}
int16_t toFWord(std::ifstream& file)
{
    return toInt16(file);
}
int16_t toUFWord(std::ifstream& file)
{
    return toUInt16(file);
}
int32_t toInt32(std::ifstream& file)
{
    return (toUInt8(file) << 24) | (toUInt8(file) << 16) | (toUInt8(file) << 8) | toUInt8(file);
}
uint32_t toUInt32(std::ifstream& file)
{
    return toInt32(file);
}
std::string toString(std::ifstream& file, int length)
{
    std::string result;
    for (int i = 0; i < length; i++)
    {
        result += char(toUInt8(file));
    }
    return result;
}
float toFixed(std::ifstream& file)
{
    return toInt32(file) / (1 << 16);
}
uint32_t toDate(std::ifstream& file)
{
    uint32_t macTime = toUInt32(file) * 0x100000000 + toUInt32(file);
    return macTime * 1000;
}

uint32_t calculateTableChecksum(std::ifstream& file, uint32_t offset, uint32_t length)
{
    auto old = file.tellg();
    file.seekg(offset);
    uint32_t sum = 0;
    int64_t nlongs = ((length + 3) / 4) | 0;

    while( nlongs-- )
    {
        sum = (sum + toUInt32(file) & 0xffffffff);
    }

    file.seekg(old);
    return sum;
}

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

struct HMTX
{
    std::vector<Metric> hMetrics;
    std::vector<int16_t> leftSideBearing;

    HMTX(std::ifstream& file, uint16_t numGlyphs, uint16_t numOfLongHorMetrics)
    {
        for (int i = 0; i < numOfLongHorMetrics; i++) {
            hMetrics.push_back
            ({
                toUInt16(file), toInt16(file)
            });
        }
        for (int i = 0; i < numGlyphs - numOfLongHorMetrics; i++)
        {
            leftSideBearing.push_back(toFWord(file));
        }
    }
};
struct Glyph
{
    int16_t numberOfContours, xMin, yMin, xMax, yMax;
};

void loadTTF(const std::string &fileName)
{
    char c;
    std::ifstream file;

    file.open(g_source + "resources/fonts/" + fileName, std::ios::binary);
    if(file.is_open())
    {
        uint32_t scalar = toUInt32(file);
        uint16_t numTables = toUInt16(file);
        uint16_t searchRange = toUInt16(file);
        uint16_t entrySelector = toUInt16(file);
        uint16_t rangeShift = toUInt16(file);

        std::unordered_map<std::string, uint32_t[3]> tables = std::unordered_map<std::string, uint32_t[3]>();
        for (int i = 0; i < numTables; i++) 
        {
            std::string tag = toString(file, 4);
            tables[tag][0] = toUInt32(file); // checksum
            tables[tag][1] = toUInt32(file); // offset
            tables[tag][2] = toUInt32(file); // length

            if(tag != "head" && calculateTableChecksum(file, tables[tag][1], tables[tag][2]) != tables[tag][0])
            {
                std::cout << "ERROR :: Font table data could not be verified; checksum was invalidated." << std::endl;
                file.close();
                return;
            }
        }
        
        // head
            file.seekg(tables["head"][1]);
            uint16_t majorVersion = toUInt16(file);
            uint16_t minorVersion = toUInt16(file);
            float fontRevision = toFixed(file);
            uint32_t checksumAdjustment = toUInt32(file);
            uint32_t magicNumber = toUInt32(file);
            uint16_t flags = toUInt16(file);
            uint16_t unitsPerEm = toUInt16(file);
            uint32_t created = toDate(file);
            uint32_t modified = toDate(file);
            int16_t xMin = toFWord(file);
            int16_t yMin = toFWord(file);
            int16_t xMax = toFWord(file);
            int16_t yMax = toFWord(file);
            uint16_t macStyle = toUInt16(file);
            uint16_t lowestRecPPEM = toUInt16(file);
            uint16_t fontDirectionHint = toInt16(file);
            uint16_t indexToLocFormat = toInt16(file);
            uint16_t glyphDataFormat = toInt16(file);

        // maxp
            file.seekg(tables["maxp"][1]);
            float maxpVersion = toFixed(file);
            uint16_t numGlyphs = toUInt16(file);
            uint16_t maxPoints = toUInt16(file);
            uint16_t maxContours = toUInt16(file);
            uint16_t maxCompositePoints = toUInt16(file);
            uint16_t maxCompositeContours = toUInt16(file);
            uint16_t maxZones = toUInt16(file);
            uint16_t maxTwilightPoints = toUInt16(file);
            uint16_t maxStorage = toUInt16(file);
            uint16_t maxFunctionDefs = toUInt16(file);
            uint16_t maxInstructionDefs = toUInt16(file);
            uint16_t maxStackElements = toUInt16(file);
            uint16_t maxSizeOfInstructions = toUInt16(file);
            uint16_t maxComponentElements = toUInt16(file);
            uint16_t maxComponentDepth = toUInt16(file);

        // hhea
            file.seekg(tables["hhea"][1]);
            float hheaVersion = toFixed(file);
            int16_t ascent = toFWord(file);
            int16_t descent = toFWord(file);
            int16_t lineGap = toFWord(file);
            uint16_t advanceWidthMax = toUFWord(file);
            int16_t minLeftSideBearing = toFWord(file);
            int16_t minRightSideBearing = toFWord(file);
            int16_t xMaxExtent = toFWord(file);
            int16_t caretSlopeRise = toInt16(file);
            int16_t caretSlopeRun = toInt16(file);
            int16_t caretOffset = toFWord(file);
            toInt16(file);toInt16(file);toInt16(file);toInt16(file); // skip 4 reserved places
            int16_t metricDataFormat = toInt16(file);
            uint16_t numOfLongHorMetrics = toUInt16(file);
        
        // hmtx
            file.seekg(tables["hmtx"][1]);
            HMTX hmtx = HMTX(file, numGlyphs, numOfLongHorMetrics);

        // loca
            file.seekg(tables["loca"][1]);
            std::vector<uint32_t> loca;
            for(int i = 0; i < numGlyphs + 1; i++)
            {
                loca.push_back(indexToLocFormat ? toUInt32(file):toUInt16(file));
            }
        
        // glyf
            std::vector<Glyph> glyphs;
            for(int i = 0; i < loca.size() - 1; i++)
            {
                uint32_t multiplier = indexToLocFormat == 0 ? 2 : 1;
                uint32_t locaOffset = loca[i] * multiplier;

                file.seekg(tables["glyf"][1] + locaOffset);
                glyphs.push_back
                ({
                    toInt16(file),
                    toInt16(file),
                    toInt16(file),
                    toInt16(file),
                    toInt16(file)
                });
            }

        // cmap
            file.seekg(tables["cmap"][1]);
            uint16_t cmapVersion = toUInt16(file);
            uint16_t cmapTables = toUInt16(file);

            std::vector<EncodingRecord> encodingRecords;
            // glyphIndexMap = {}

            if(cmapVersion != 0)
            {
                std::cout << "ERROR :: Font CMAP version not supported; Current version is " << cmapVersion << " but version 0 is required." << std::endl;
                file.close();
                return;
            }
            for(int i = 0; i < cmapTables; i++)
            {
                encodingRecords.push_back
                ({
                    toUInt16(file),
                    toUInt16(file),
                    toUInt32(file)
                });
            }
            int selectedOffset = -1;
            for(int i = 0; i < cmapTables; i++)
            {
                EncodingRecord record = encodingRecords[i];
                bool isWindowsPlatform = record.platformID == 3 && (record.encodingID == 0 || record.encodingID == 1 || record.encodingID == 10);
                bool isUnicodePlatform = record.platformID == 0 && (record.encodingID == 0 || record.encodingID == 1 || record.encodingID == 2 || record.encodingID == 3 || record.encodingID == 4);

                if (isWindowsPlatform || isUnicodePlatform)
                {
                    selectedOffset = record.offset;
                    break;
                }
            }
            
            if (selectedOffset == -1)
            {
                std::cout << "ERROR :: Font does not contain any recognized platform and encoding." << std::endl;
                file.close();
                return;
            }
            
            file.seekg(selectedOffset);
            toUInt16(file);
            uint16_t format = toUInt16(file);
            std::cout << format << std::endl;

        file.close();
    }
    else
    {
        std::cout << "ERROR :: " << fileName << " could not be opened." << std::endl;
    }
}