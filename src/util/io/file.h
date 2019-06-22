//
// Created by sneeuwpop on 21-6-19.
//

#pragma once

#include <string>

class File
{
public:
    static std::string readAssetAsString(const char *path);

private:
    static std::string readString(std::string *path);
};