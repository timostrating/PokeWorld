////
//// Created by sneeuwpop on 24-12-20.
////
//
//#pragma once
//
//#include <iostream>
//#include <functional>
//#include <string>
//#include <vector>
//#include <thread>
//
//class FileWatch
//{
//
//    std::vector<std::string> paths;
//    std::thread watchThread;
//
//public:
//
//    using callback = std::function<void(const std::string &filePath)>;
//
//    callback onCreate = [] (auto) {};
//    callback onDelete = [] (auto) {};
//    callback onChange = [] (auto) {};
//
//    /**
//     * set this to false in a callback to stop the watching thread.
//     */
//    bool continueWatching = true;
//
//    void addDirectoryToWatch(const std::string &path, bool recursive);
//
//    void startWatchingSync();
//
//    void startWatchingAsync();
//
//};
//
//
//
//
