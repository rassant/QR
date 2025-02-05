#
# cmake_minimum_required(VERSION 3.10)
# project(MyApplication)
#
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")
#
# # Указываем, что хотим использовать статические библиотеки
# set(BUILD_SHARED_LIBS OFF)
# set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
#
# # Укажите путь к вашим исходным файлам
# set(SOURCES
#     main.cpp
#     source/Application.cpp
#     source/FileCopying.cpp
#     source/FileWatcher.cpp
#     source/QRImageProcessor.cpp
# )
#
# # Определяем платформу
# if(WIN32)
#     message(STATUS "Configuring for Windows")
#     # Указываем путь к vcpkg toolchain file
#     set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
#
#     # Указываем путь к установленным библиотекам через vcpkg
#     set(CMAKE_PREFIX_PATH "D:/GitHub/vcpkg/installed/x64-windows-static" ${CMAKE_PREFIX_PATH})
#
#     # Включаем директории с заголовочными файлами
#     include_directories(
#         "D:/GitHub/vcpkg/installed/x64-windows-static/include"
#         "D:/GitHub/vcpkg/installed/x64-windows-static/include/opencv2"
#     )
# else()
#     message(STATUS "Configuring for Linux")
#     # Включаем директории с заголовочными файлами
#     include_directories(
#         "/usr/include/opencv4"
#     )
#
#     # Используем pkg-config только на Linux
#     find_package(PkgConfig REQUIRED)
#     pkg_check_modules(QRENCODE REQUIRED libqrencode)
#     if(QRENCODE_FOUND)
#         include_directories(${QRENCODE_INCLUDE_DIRS})
#     else()
#         message(FATAL_ERROR "[| libqrencode |] not found!")
#     endif()
# endif()
#
# # Добавление исполняемого файла
# add_executable(MyApplication ${SOURCES})
#
# # Поиск OpenCV
# find_package(OpenCV REQUIRED)
# if(OpenCV_FOUND)
#     target_link_libraries(MyApplication PRIVATE ${OpenCV_LIBS})
# else()
#     message(FATAL_ERROR "[| OpenCV |] not found!")
# endif()
#
# # Линковка libqrencode
# if(WIN32)
#     # На Windows ищем libqrencode вручную, если find_package не работает
#     find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h PATHS "D:/GitHub/vcpkg/installed/x64-windows-static/include")
#     find_library(QRENCODE_LIBRARY NAMES qrencode PATHS "D:/GitHub/vcpkg/installed/x64-windows-static/lib")
#
#     if(QRENCODE_INCLUDE_DIR AND QRENCODE_LIBRARY)
#         message(STATUS "Found qrencode: ${QRENCODE_LIBRARY}")
#         include_directories(${QRENCODE_INCLUDE_DIR})
#         target_link_libraries(MyApplication PRIVATE ${QRENCODE_LIBRARY})
#     else()
#         message(FATAL_ERROR "[| libqrencode | not found on Windows!]")
#     endif()
# else()
#     # На Linux используем pkg-config
#     target_link_libraries(MyApplication PRIVATE ${QRENCODE_LIBRARIES})
# endif()
#
# # Поиск Boost
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# if (Boost_FOUND)
#     target_link_libraries(MyApplication PRIVATE Boost::system Boost::filesystem)
# else()
#     message(FATAL_ERROR "[| Boost |] not found!")
# endif()
#
# # Поиск exiv2
# find_package(exiv2 REQUIRED)
# if (exiv2_FOUND)
#     target_link_libraries(MyApplication PUBLIC exiv2lib)
# else()
#     message(FATAL_ERROR "[| exiv2 |] not found!")
# endif()
#
#


cmake_minimum_required(VERSION 3.10)

project(OpenCVExample)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите путь к вашему vcpkg toolchain файлу
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    set(CMAKE_PREFIX_PATH "D:/GitHub/vcpkg/installed/x64-windows")
    set(Protobuf_PROTOC_EXECUTABLE "D:/GitHub/vcpkg/installed/x64-windows/tools/protobuf/protoc.exe")
    find_package(Protobuf REQUIRED)
endif()


find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(exiv2 CONFIG REQUIRED)
find_package(Iconv REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(QRENCODE REQUIRED libqrencode)

# Список исходных файлов
set(SOURCES
    main.cpp
    source/Application.cpp
    source/FileCopying.cpp
    source/FileWatcher.cpp
    source/QRImageProcessor.cpp
    source/InputData.cpp
    source/FileCopyManager.cpp
)

# Создание исполняемого файла
add_executable(OpenCVExample ${SOURCES})

##########################################################################
# Линковка с библиотеками OpenCV
if(OpenCV_FOUND)
    message("~~~~~~~~~~~~~~~~~~~~~~~~!OpenCV FOUND!~~~~~~~~~~~~~~~~~~~~~~")
    target_link_libraries(OpenCVExample PRIVATE ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "OpenCV not found!")
endif()

# Линковка с библиотеками Iconv
if(Iconv_FOUND)
    message("~~~~~~~~~~~~~~~~~~~~~~!Iconv FOUND!~~~~~~~~~~~~~~~~~~~~~~")
    target_link_libraries(OpenCVExample PRIVATE Iconv::Iconv)
else()
    message(FATAL_ERROR "Iconv not found!")
endif()

# Линковка с библиотеками Exiv2
if(exiv2_FOUND)
    message("~~~~~~~~~~~~~~~~~~~~~~!exiv2 FOUND!~~~~~~~~~~~~~~~~~~~~~~")
    target_link_libraries(OpenCVExample PRIVATE Exiv2::exiv2lib)
else()
    message(FATAL_ERROR "Exiv2 not found!")
endif()

# Линковка с библиотеками Boost
if(Boost_FOUND)
    message("~~~~~~~~~~~~~~~~~~~~~~!Boost FOUND!~~~~~~~~~~~~~~~~~~~~~~")
    target_link_libraries(OpenCVExample PRIVATE Boost::system)
    target_link_libraries(OpenCVExample PRIVATE Boost::filesystem)
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# Линковка с потоками
if(Threads_FOUND)
    message("~~~~~~~~~~~~~~~~~~~~~~!Threads FOUND!~~~~~~~~~~~~~~~~~~~~~~")
    target_link_libraries(OpenCVExample PRIVATE Threads::Threads)
else()
    message(FATAL_ERROR "Threads not found!")
endif()

# pkg_check_modules(QRENCODE REQUIRED libqrencode)
if(QRENCODE_FOUND)
    include_directories(${QRENCODE_INCLUDE_DIRS})
    target_link_libraries(OpenCVExample PRIVATE ${QRENCODE_LIBRARIES})
else()
    message(FATAL_ERROR "[| libqrencode |] not found!")
endif()
