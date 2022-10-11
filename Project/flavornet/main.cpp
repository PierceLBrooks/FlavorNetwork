
#include <iostream>
#include "flavornet.h"

int main(int argc, char** argv)
{
    std::cout << "Hello, world!" << std::endl;
    std::cout << nameCount << std::endl;
    std::cout << vectorSize << std::endl;
    std::cout << ingredientNames[nameCount-1] << std::endl;
    for (int i = 0; i < vectorSize; i++)
    {
        std::cout << ingredientVectors[nameCount-1][i];
    }
    std::cout << std::endl;
    return 0;
}

