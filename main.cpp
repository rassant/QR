
/*g++ -ggdb main.cpp ./FileCopying.cpp ./FileWatcher.cpp -std=c++20 -lboost_system -lboost_filesystem -lexiv2 && ./a.out*/

//g++ -std=c++20 -O2 -Wall main.cpp ./source/* $(pkg-config --cflags --libs opencv4) -lqrencode -lboost_system -lboost_filesystem -lexiv2  && ./a.out

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


/*#include <gtest/gtest.h>*/
/*auto main(int argc, char** argv) -> int {*/
/* ::testing::InitGoogleTest(&argc, argv);*/
/*    return RUN_ALL_TESTS();*/
/**/
/*    return 0;*/
/*}*/



   
