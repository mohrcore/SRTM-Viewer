#include "directory_scanner.h"

using namespace dir;

#ifndef DIRENT
#ifdef FILESYSTEM_EXPERIMENTAL
#include <experimental/filesystem>
using directory_iterator = std::experimental::filesystem::directory_iterator;
#else
#include <filesystem>
using directory_iterator = std::filesystem::directory_iterator;
#endif

void dir::lsd(std::string dir, std::function<bool(std::string)> on_path) {
    for (const auto & entry : directory_iterator(dir)) {
        bool cont = on_path(entry.path());
        if (!cont)
            break;
    }
}

#else

//Broken, doesn't work - idk why. Don't use DIRENT flag as for now
#include <dirent.h>

void dir::lsd(std::string dir, std::function<bool(std::string)> on_path) {
    DIR * dir_;
    dirent *  entry;
    if ((dir_ = opendir(dir.c_str())) = NULL)
        throw std::string("DIRECTORY NOT FOUND, THE EGO IS GONE: ") + dir; //I'm lazy and I don't care what this function actually throws
    bool cont = true;
    while ((entry = readdir(dir_)) != NULL && cont)
        cont = on_path(std::string(entry->d_name));
    closedir(dir_);
}

#endif