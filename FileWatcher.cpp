// FileWatcher.cpp
#include "FileWatcher.hpp"
#include "FileCopying.hpp"
#include <thread>

FileWatcher::FileWatcher(FileCopying& file_copying, unsigned int interval_seconds)
    : file_copying_(file_copying), interval_(interval_seconds) {}

void FileWatcher::Start() {
    while (true) {
        try {
            file_copying_.Run();
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при копировании: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(interval_));
    }
}
