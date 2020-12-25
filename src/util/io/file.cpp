//
// Created by sneeuwpop on 21-6-19.
//

#include "file.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../debug/nice_error.h"

std::string File::readAssetAsString(const char *path) {
    return readString(&std::string("../../../../assets/").append(path));  // TODO: This is not universal.
}

std::string File::readString(std::string *path) {
    std::cout << "READING: " << path->c_str() << std::endl;

    std::ifstream fileStream(path->c_str(), std::ios::in);
    if (!fileStream.is_open())
        throw nice_error("error while opening file: " + *path);

    std::stringstream stringStream;
    stringStream << fileStream.rdbuf();
    fileStream.close();

    if (fileStream.bad())
        throw nice_error("error while reading file");

    return stringStream.str();
}

//void File::iterateDirectoryRecursively(const std::string &path_, std::function<void(const std::string &, bool)> cb)
//{
//    std::string path = path_;
//    if (!stringEndsWith(path, "/"))
//        path += "/";
//
//#ifndef _WIN32
//    if (auto dir = opendir(path.c_str())) {
//        while (auto f = readdir(dir)) {
//            if (f->d_name[0] == '.') continue;
//            bool isDir = f->d_type == DT_DIR;
//            if (isDir)
//                iterateDirectoryRecursively(path + f->d_name, cb);
//
//            cb(path + f->d_name, isDir);
//        }
//        closedir(dir);
//    }
//#else
//    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path))
//    {
//        cb(dirEntry.path().generic_string(), dirEntry.is_directory());
//    }
//#endif
//}