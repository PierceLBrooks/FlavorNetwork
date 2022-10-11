
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "flavornet.h"
#include "HnSRTree/HnSRTreeFile.hh"
#include "HnSRTree/HnPoint.hh"
#include "HnSRTree/HnDataItem.hh"

int main(int argc, char** argv)
{
    HnSRTreeFile indexFile;
    HnPointVector points = new_HnPointVector();
    HnDataItemVector dataItems = new_HnDataItemVector();
    HnProperties properties = new_HnProperties();
    int dataItemSize = 0;
    for (int i = 0; i < nameCount; i++)
    {
        int length = strlen(ingredientNames[i])+1;
        HnPoint point = new_HnPoint(vectorSize);
        HnDataItem dataItem = new_HnDataItem(ingredientNames[i], length);
        if (length > dataItemSize)
        {
            dataItemSize = length;
        }
        for (int j = 0; j < vectorSize; j++)
        {
            point.setCoordAt((double)ingredientVectors[i][j], j);
        }
        points.addElement(point);
        dataItems.addElement(dataItem);
    }
    dataItemSize += 1;
    indexFile = new_HnSRTreeFile("flavornet.idx", vectorSize, dataItemSize, points, dataItems, properties);
    if (indexFile == HnSRTreeFile::null)
    {
        return -1;
    }
    indexFile.close();
    return 0;
}

