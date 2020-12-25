////
//// Created by sneeuwpop on 24-12-20.
////
//
//#include "file_watch.h"
//#include "file.h"
//
//#include <thread>
//#include <map>
//
//void FileWatch::addDirectoryToWatch(const std::string &path, bool recursive)
//{
//    paths.push_back(path);
//    if (!stringEndsWith(path, "/"))
//        paths.back() += '/';
//
//    if (recursive)
//        File::iterateDirectoryRecursively(path, [&] (auto path, bool isDir) {
//            if (isDir) addDirectoryToWatch(path, false);
//        });
//}
//
//void FileWatch::startWatchingAsync()
//{
//    watchThread = std::thread(&FileWatcher::startWatchingSync, this);
//}
//
//#ifdef linux
//
//#include <sys/inotify.h>
//#include <unistd.h>
//
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
//
//void FileWatch::startWatchingSync()
//{
//    int fd = inotify_init();
//
//    if ( fd < 0 )
//        throw nice_error("inotify_init() failed");
//
//    std::map<int, std::string> watchToPath;
//
//    for (auto &p : paths)
//    {
//        int wd = inotify_add_watch(fd, p.c_str(), IN_CREATE | IN_DELETE | IN_CLOSE_WRITE);
//        watchToPath[wd] = p;
//    }
//
//    std::cout << "start watching " << watchToPath.size() << " directories.\n";
//
//    while (continueWatching)
//    {
//        char buffer[EVENT_BUF_LEN];
//        int length = read( fd, buffer, EVENT_BUF_LEN );
//
//        if (length < 0)
//            throw nice_error("reading inotify events failed");
//
//        inotify_event *event = (inotify_event *) &buffer[0];
//        if (!event->len || event->mask & IN_ISDIR)
//            continue;
//
//        auto &directory = watchToPath[event->wd];
//        std::string filePath = directory + std::string(event->name);
//
////        if (event->mask & IN_CREATE)
////            onCreate(filePath);
////        else if (event->mask & IN_DELETE)
////            onDelete(filePath);
////        else if (event->mask & IN_CLOSE_WRITE)
////            onChange(filePath);
//        std::cout << "Owh jummy " << filePath;
//    }
//    for (auto &w : watchToPath)
//        inotify_rm_watch(fd, w.first);
//
//    close(fd);
//
//    std::cout << "ended watching " << watchToPath.size() << " directories.\n";
//}
//
//#else
//
//void FileWatch::startWatchingSync()
//{
//    throw nice_error("FileWatcher is not implemented on this platform");
//}
