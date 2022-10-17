
#include <map>
#include <limits>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "flavornet.h"
#include "HnSRTree/HnSRTreeFile.hh"
#include "HnSRTree/HnPoint.hh"
#include "HnSRTree/HnDataItem.hh"

float getDistance(const float* left, const float* right, int length)
{
    float distance = 0.0f;
    for (int i = 0; i < length; ++i)
    {
        distance += powf(left[i]-right[i], 2.0f);
    }
    return sqrtf(distance);
}

int main(int argc, char** argv)
{
    HnPoint point;
    HnSRTreeFile indexFile;
    HnPointVector points = new_HnPointVector();
    HnDataItemVector dataItems = new_HnDataItemVector();
    HnProperties properties = new_HnProperties();
    int dataItemSize = 0;
    int manualCount = 0;
    int indexCount = 0;
    int testCount = 100;
    int neighborCount = 10;
    std::vector<float> minimum;
    std::vector<float> maximum;
    std::vector<int> manualIndices;
    std::vector<int> indexIndices;
    std::map<int, std::vector<int>> manualNeighbors;
    std::map<int, std::vector<int>> indexNeighbors;
    std::map<std::string, int> nameIndices;
    for (int i = 0; i < vectorSize; ++i)
    {
        minimum.push_back(std::numeric_limits<float>::max());
        maximum.push_back(std::numeric_limits<float>::min());
    }
    for (int i = 0; i < nameCount; ++i)
    {
        int length = strlen(ingredientNames[i])+1;
        HnDataItem dataItem = new_HnDataItem(ingredientNames[i], length);
        point = new_HnPoint(vectorSize);
        if (length > dataItemSize)
        {
            dataItemSize = length;
        }
        for (int j = 0; j < vectorSize; ++j)
        {
            float coord = ingredientVectors[i][j];
            if (coord < minimum[j])
            {
                minimum[j] = coord;
            }
            if (coord > maximum[j])
            {
                maximum[j] = coord;
            }
            point.setCoordAt((double)coord, j);
        }
        points.addElement(point);
        dataItems.addElement(dataItem);
        nameIndices[std::string(ingredientNames[i])] = i;
    }
    ++dataItemSize;
    properties.setProperty(HnSRTreeBlockSize, std::to_string(((int)(((double)sizeof(double))*pow((double)(vectorSize*nameCount), 3.0/4.0)))).c_str());
    indexFile = new_HnSRTreeFile("flavornet.idx", vectorSize, dataItemSize, points, dataItems, properties);
    if (indexFile == HnSRTreeFile::null)
    {
        std::cout << "Index Error!" << std::endl;
        return -1;
    }
    std::cout << "Entities: " << nameCount << std::endl;
    std::cout << "Dimensions: " << vectorSize << std::endl;
    std::cout << "Tests Per Strategy: " << testCount << std::endl;
    std::cout << "Neighbors Per Test: " << neighborCount << std::endl;
    srand(time(NULL));
    auto manualFirst = std::chrono::steady_clock::now();
    do
    {
        ++manualCount;
        manualIndices.push_back(rand()%nameCount);
        manualNeighbors[manualIndices.back()] = std::vector<int>();
        for (int i = 0; i < neighborCount; ++i)
        {
            float distance = -1.0f;
            for (int j = 0; j < nameCount; ++j)
            {
                float temp = 1.0f;
                for (int k = 0; k < i; ++k)
                {
                    if (manualNeighbors[manualIndices.back()][k] == j)
                    {
                        temp = -1.0f;
                        break;
                    }
                }
                if (temp < 0.0f)
                {
                    continue;
                }
                temp = getDistance(ingredientVectors[manualIndices.back()], ingredientVectors[j], vectorSize);
                if (distance < 0.0f)
                {
                    distance = temp;
                    manualNeighbors[manualIndices.back()].push_back(j);
                    continue;
                }
                if (temp < distance)
                {
                    distance = temp;
                    manualNeighbors[manualIndices.back()][i] = j;
                }
            }
        }
    } while (manualCount < testCount);
    auto manualLast = std::chrono::steady_clock::now();
    srand(time(NULL));
    auto indexFirst = std::chrono::steady_clock::now();
    do
    {
        ++indexCount;
        indexIndices.push_back(rand()%nameCount);
        indexNeighbors[indexIndices.back()] = std::vector<int>();
        points = new_HnPointVector();
        dataItems = new_HnDataItemVector();
        point = new_HnPoint(vectorSize);
        for (int i = 0; i < vectorSize; ++i)
        {
            point.setCoordAt((double)ingredientVectors[indexIndices.back()][i], i);
        }
        indexFile.getNeighbors(point, neighborCount, &points, &dataItems);
        for (int i = 0; i < dataItems.size(); ++i)
        {
            indexNeighbors[indexIndices.back()].push_back(nameIndices[std::string(dataItems.elementAt(i).toCharArray())]);
        }
    } while (indexCount < testCount);
    auto indexLast = std::chrono::steady_clock::now();
    indexFile.close();
    std::chrono::duration<double> manualDuration = manualLast-manualFirst;
    std::cout << "Brute Force Strategy Time: " << manualDuration.count() << "s" << std::endl;
    std::chrono::duration<double> indexDuration = indexLast-indexFirst;
    std::cout << "Index Strategy Time: " << indexDuration.count() << "s" << std::endl;
    /*
    for (int i = 0; i < manualIndices.size(); ++i)
    {
        //std::cout << "#" << i+1 << std::endl;
        for (int j = 0; j < manualNeighbors[manualIndices[i]].size(); ++j)
        {
            float distance = getDistance(ingredientVectors[manualIndices[i]], ingredientVectors[manualNeighbors[manualIndices[i]][j]], vectorSize);
            std::cout << j << ": " << ingredientNames[manualIndices[i]] << " -> " << ingredientNames[manualNeighbors[manualIndices[i]][j]] << " = " << distance << std::endl;
        }
    }
    for (int i = 0; i < indexIndices.size(); ++i)
    {
        //std::cout << "#" << i+1 << std::endl;
        for (int j = 0; j < indexNeighbors[indexIndices[i]].size(); ++j)
        {
            float distance = getDistance(ingredientVectors[indexIndices[i]], ingredientVectors[indexNeighbors[indexIndices[i]][j]], vectorSize);
            std::cout << j << ": " << ingredientNames[indexIndices[i]] << " -> " << ingredientNames[indexNeighbors[indexIndices[i]][j]] << " = " << distance << std::endl;
        }
    }
    */
    return 0;
}

