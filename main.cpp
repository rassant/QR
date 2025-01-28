
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

#include "./header/Application.hpp"
#include <iostream>
using namespace std;


auto main(int argc, char** argv) -> int {
 try {
        Application app(argc, argv);
        Application::PrintUsage();
        app.Run();
    } 
    catch (const std::exception& e) {
        std::cerr << "Ошибка в работе программы: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

