#include "../header/FileWatcher.hpp"
#include "../header/FileCopying.hpp"
#include <thread>
#include <iostream>

FileWatcher::FileWatcher(FileCopying& file_copying, unsigned int interval_seconds)
    : file_copying_(file_copying), interval_(interval_seconds) {}

void FileWatcher::Start() {
    while (true) {
        try {
            file_copying_.Run();
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при копировании: " << e.what() << '\n';
        }

        unsigned int count_interval = interval_;
        for (unsigned int i = count_interval; i > 0; --i) {
            std::cout << "\rОтслеживаю папку... " << i << " секунд осталось. " << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
    }
}



auto FileWatcher::isDirectoryEmpty(const std::filesystem::path& dir) -> bool {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        throw std::runtime_error("Указанный путь не существует или не является директорией");
    }
    return std::filesystem::begin(std::filesystem::directory_iterator(dir)) == std::filesystem::end(std::filesystem::directory_iterator());
}
