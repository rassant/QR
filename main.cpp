
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

/*#include "FileWatcher.hpp"*/

#include "./header/FileCopying.hpp"
#include <algorithm>
#include <string>
#include <span>

using namespace std;

auto main(int argc, char** argv) -> int {

    std::cout << '\n';
    std::cout << "Использование:\n";
    std::cout << "[a.out] [Папка_откуда_копировать] [Папка_куда_скопировать] [Время_задержки_сканирования]" << '\n';
    std::cout << "Например: a.out ./resource/ ./resource/to/ 5" << '\n';
    std::cout << '\n';

    span<char*> args_span(argv, static_cast<size_t>(argc));
    vector<string> args;
    std::ranges::transform(args_span, back_inserter(args)
                        , [](char* arg) { return string(arg); });


    if (argc == 4)
    {
        const string & from_source    = args[1];
        const string & to_destination = args[2];
        /*const int wait_second   = std::stoi(args[3]);*/

        FileCopying file_copy(FromSourceTag   {},from_source
                            , ToDestinationTag{},to_destination);
        /*FileWatcher watcher(fc, std::stoi(wait_second));*/
        /*watcher.Start(); */
        return 0;
    }

    /*const int wait_second = 5;*/
    static FileCopying file_copy(FromSourceTag{}, "./resource/", ToDestinationTag{}, "./resource/to/" );
    file_copy.Run();
    /*FileWatcher watcher(fc, wait_second); // Проверка каждые 3 секунды*/
    /*watcher.Start(); // Запуск в бесконечном цикле*/
    return 0;
}

