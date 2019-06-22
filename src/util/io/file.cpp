//
// Created by sneeuwpop on 21-6-19.
//

#include "file.h"
#include <string>
#include <fstream>
#include <sstream>

std::string File::readAssetAsString(const char *path) {
    return readString(&std::string("../../../../assets/").append(path));
}

std::string File::readString(std::string *path) {
    std::ifstream fileStream(path->c_str(), std::ios::in);
    if (!fileStream.is_open())
        perror("error while opening file");

    std::string returnValue;

    std::stringstream stringStream;
    stringStream << fileStream.rdbuf();
    returnValue = stringStream.str();
    fileStream.close();

    if (fileStream.bad())
        perror("error while reading file");

    return returnValue;
}