//
// Created by sneeuwpop on 21-6-19.
//

#include "file.h"
#include <string>
#include <fstream>

std::string File::readAssetAsString(const char *path) {
    return readString(&std::string("../../../../assets/").append(path));
}

std::string File::readString(std::string *path) {
    std::ifstream file(path->c_str());
    if (!file.is_open())
        perror("error while opening file");

    std::string str;
    std::string file_contents;
    while (std::getline(file, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }

    if (file.bad())
        perror("error while reading file");

    return file_contents;
}
