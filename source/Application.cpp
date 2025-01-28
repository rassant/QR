#include "../header/Application.hpp"
#include "../header/FileWatcher.hpp"
#include "../header/FileCopying.hpp"
#include "../header/QRImageProcessor.hpp"
#include <algorithm>
#include <span>
#include <stdexcept>
#include <thread>


Application::
Application (int argc, char ** argv) : argc_ (argc) {

    std::span<char*> args_span (argv, static_cast<size_t> (argc));
    std::ranges::transform (args_span, back_inserter (argv_)
                           , [] (char* arg) 
                           { return std::string (arg); }
                           );
}



void Application::
ParseArguments ( ) 
{
    if (argc_ < 4 )
    {
        throw std::invalid_argument ("Недостаточно аргументов");
    }

    from_source_     = argv_[1];
    to_destination_  = argv_[2];
    int wait_second  = std::stoi (argv_[3]);

    if (wait_second <= 0) {
        throw std::out_of_range ("Не должно быть отрицательного значения или нулевого. Требуется целое положительное число.\n");
    }

    second_wait_ = static_cast<unsigned int> (wait_second);
}




void Application::
ProcessUserRequest ( )
{
    ParseArguments ( );

    FileCopying file_copy (FromSourceTag   {}, from_source_
                         , ToDestinationTag{}, to_destination_);
    FileWatcher watcher (file_copy, second_wait_);
    watcher.Start ( ); 
}


void Application::
PrintUsage ( ){
    std::cout << '\n';
    std::cout << "Использование:\n";
    std::cout << "[a.out] [Папка_откуда_копировать] [Папка_куда_скопировать] [Время_задержки_сканирования]" << '\n';
    std::cout << "Например: a.out ./resource/ ./resource/to/ 5" << '\n';
    std::cout << '\n';
}

/*void Application::Run ( ) */
/*{*/
/*    QRImageProcessor qr_image_processor;*/
/**/
/*    if (argc_ == 4)*/
/*    {*/
/*        ProcessUserRequest ( );*/
/**/
/*    } else {*/
/*        static FileCopying file_copy (FromSourceTag{}, "./resource/"*/
/*                                    , ToDestinationTag{}, "./resource/to/" );*/
/**/
/**/
/*        const int wait_second = 5;*/
/*        FileWatcher watcher (file_copy, wait_second); // Проверка каждые 3 секунды*/
/**/
/*        watcher.Start ( ); // Запуск в бесконечном цикле*/
/*	    qr_image_processor.Process("./resource/to");*/
/*    }*/
/*}*/
/*void Application::Run() {*/
/*    QRImageProcessor qr_image_processor;*/
/**/
/*    if (argc_ == 4) {*/
/*        ProcessUserRequest();*/
/*    } else {*/
/*        static FileCopying file_copy(FromSourceTag{}, "./resource/",*/
/*                                    ToDestinationTag{}, "./resource/to/");*/
/**/
/*        const int wait_second = 5;*/
/*        FileWatcher watcher(file_copy, wait_second);*/
/**/
/*        // Запускаем FileWatcher в отдельном потоке*/
/*        std::thread watcher_thread([&watcher]() {*/
/*            watcher.Start();*/
/*        });*/
/**/
/*        // Обрабатываем QR-коды в основном потоке*/
/*        qr_image_processor.Process("./resource/to");*/
/**/
/*        // Если нужно, можно добавить ожидание завершения потока (но в данном случае это не требуется)*/
/*        watcher_thread.detach();*/
/*    }*/
/*}*/

/*void Application::Run() {*/
/*    QRImageProcessor qr_image_processor;*/
/**/
/*    if (argc_ == 4) {*/
/*        ProcessUserRequest();*/
/*    } else {*/
/*        static FileCopying file_copy(*/
/*            FromSourceTag{}, "./resource/",*/
/*            ToDestinationTag{}, "./resource/to/"*/
/*        );*/
/**/
/*        const int wait_second = 5;*/
/*        static FileWatcher watcher(file_copy, wait_second); // Делаем статическим*/
/**/
/*        // Запускаем отслеживание в отдельном потоке*/
/*        std::thread watcher_thread([]() {*/
/*            watcher.Start(); // Бесконечный цикл внутри*/
/*        });*/
/**/
/*        // Обрабатываем существующие файлы*/
/*        qr_image_processor.Process("./resource/to");*/
/**/
/*        // Оставляем основной поток активным*/
/*        while (true) {*/
/*            std::this_thread::sleep_for(std::chrono::seconds(1));*/
/*        }*/
/**/
/*        watcher_thread.join(); // Необязательно, но для корректного завершения*/
/*    }*/
/*}*/


void Application::Run() {
    QRImageProcessor qr_image_processor;

    if (argc_ == 4) {
        ProcessUserRequest();
    } else {
        static FileCopying file_copy(
            FromSourceTag{}, "./resource/",
            ToDestinationTag{}, "./resource/to/"
        );

        const int wait_second = 5;
        static FileWatcher watcher(file_copy, wait_second); 

        // Запускаем отслеживание в отдельном потоке
        std::thread watcher_thread([]() {
            watcher.Start(); // Бесконечный цикл внутри
        });

        // Обрабатываем существующие файлы (НАКЛАДЫВАЕМ QR)
        qr_image_processor.Process("./resource/to");

        // Оставляем основной поток активным
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        watcher_thread.join(); 
    }
}
