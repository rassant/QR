#pragma once
#include "FileCopying.hpp"

class FileCopying; 

class FileWatcher {
public:
    FileWatcher(FileCopying& file_copying, unsigned int interval_seconds);
    void Start();

private:
    FileCopying  file_copying_;
    unsigned int interval_;
};

