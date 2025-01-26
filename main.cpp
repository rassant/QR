
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

#include "FileCopying.hpp"
#include "FileWatcher.hpp"
int main() {
    FileCopying fc("./resource/", "./resource/to/");
    FileWatcher watcher(fc, 10); // Проверка каждые 3 секунды
    watcher.Start(); // Запуск в бесконечном цикле
    return 0;
}

