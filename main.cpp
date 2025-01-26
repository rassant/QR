
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

#include "FileCopying.hpp"
#include "FileWatcher.hpp"
#include <string>

using namespace std;

int main(int argc, char** argv) {
    std::cout << "Использование:\n";
    std::cout << "[a.out] [Папка_откуда_копировать] [Папка_куда_скопировать] [Время_задержки_сканирования]" << std::endl;
    std::cout << "Например: a.out ./resource/ ./resource/to/ 5" << std::endl << std::endl;

    if (argc == 4)
    {
        string programm_name = argv[0];
        string from          = argv[1];
        string to            = argv[2];
        string wait_second   = argv[3];

        FileCopying fc(from, to, std::stoi (wait_second));
        FileWatcher watcher(fc, std::stoi(wait_second));
        watcher.Start(); 
        return 0;
    }

    const int wait_second = 5;
    FileCopying fc("./resource/", "./resource/to/", wait_second);
    FileWatcher watcher(fc, wait_second); // Проверка каждые 3 секунды
    watcher.Start(); // Запуск в бесконечном цикле
    return 0;
}

