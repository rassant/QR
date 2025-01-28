
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

/*#include "FileWatcher.hpp"*/

#include "./header/FileCopying.hpp"
#include "header/FileWatcher.hpp"
#include <algorithm>
#include <string>
#include <span>

using namespace std;

class Application 
{
private:
    int argc_;
    vector<string> argv_;

    string from_source_;
    string to_destination_;
    unsigned int second_wait_{};


  public:
    Application(int argc,  char ** argv) : argc_(argc) {

        span<char*> args_span(argv, static_cast<size_t>(argc));
        std::ranges::transform(args_span, back_inserter(argv_)
                               , [](char* arg) { return string(arg); });
    }

    void parseArguments () 
    {
        if (argc_<4 )
        {
            throw std::invalid_argument("Недостаточно аргументов");
        }

        from_source_     = argv_[1];
        to_destination_  = argv_[2];
        int wait_second  = std::stoi(argv_[3]);

        if (wait_second <= 0) {
            throw std::out_of_range("Не должно быть отрицательного значения или нулевого. Требуется целое положительное число.\n");
        }

        second_wait_ = static_cast<unsigned int>(wait_second);
    }

    void processUserRequest ()
    {
        parseArguments();

        FileCopying file_copy(FromSourceTag   {},from_source_
                            , ToDestinationTag{},to_destination_);
        FileWatcher watcher(file_copy, second_wait_);
        watcher.Start(); 
    }


    static void printUsage (){
        std::cout << '\n';
        std::cout << "Использование:\n";
        std::cout << "[a.out] [Папка_откуда_копировать] [Папка_куда_скопировать] [Время_задержки_сканирования]" << '\n';
        std::cout << "Например: a.out ./resource/ ./resource/to/ 5" << '\n';
        std::cout << '\n';
    }

    void run () 
    {
        if (argc_ == 4)
        {
            processUserRequest ();
        }
        else 
        {
            const int wait_second = 5;
            static FileCopying file_copy(FromSourceTag{}, "./resource/", ToDestinationTag{}, "./resource/to/" );
            file_copy.Run();
            FileWatcher watcher(file_copy, wait_second); // Проверка каждые 3 секунды
            watcher.Start(); // Запуск в бесконечном цикле
        }
    }
};


auto main(int argc, char** argv) -> int {
 try {
        Application app(argc, argv);
        Application::printUsage();
        app.run();
    } 
    catch (const std::exception& e) {
        std::cerr << "Ошибка в работе программы: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

