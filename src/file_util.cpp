#include <iostream>
#include <algorithm>
#include <fstream>
#include <limits>

#include "file_util.h"

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
        std::cout << "ERROR :: " << fileName << " could not be opened.";
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
        std::cout << "ERROR :: " << fileName << " could not be opened.";
    }
    return {vertices, dimensions};
}