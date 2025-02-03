#pragma once
#include <filesystem>
#include "FileCopying.hpp"
#include "InputData.hpp"

class FileCopying; 

class FileWatcher {
private:
    FileCopying  file_copying_;
    std::unordered_map<std::string,std::vector<std::filesystem::path>> photographers_;
    unsigned int interval_;

public:
    FileWatcher(FileCopying& file_copying, unsigned int interval_seconds);
    FileWatcher(PhotographerCollection& paths_flash, unsigned int interval_seconds);
    
    void Start();
};

