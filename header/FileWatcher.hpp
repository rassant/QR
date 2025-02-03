#pragma once
#include <filesystem>
#include "FileCopying.hpp"
#include "InputData.hpp"

class FileCopyManager
{
    PhotographerCollection paths_flash_; // пути откуда копируем
    WorkDirectoryManager   path_save_;   // пути куда копируем
    void StartCoping ();
    FileCopyManager ( const PhotographerCollection &, const WorkDirectoryManager &);
};




class FileWatcher {
private:
    FileCopying  file_copying_;
    std::unordered_map<std::string,std::vector<std::filesystem::path>> photographers_;
    unsigned int interval_;

    static auto isDirectoryEmpty(const std::filesystem::path& dir) -> bool;


public:
    FileWatcher(           FileCopying& file_copying, unsigned int interval_seconds);
    FileWatcher(PhotographerCollection&  paths_flash, unsigned int interval_seconds);
    
    void Start();
};

