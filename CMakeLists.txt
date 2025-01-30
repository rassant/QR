cmake_minimum_required(VERSION 3.10)
project(MyApplication)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# Указываем, что хотим использовать статические библиотеки
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# Укажите путь к вашим исходным файлам
set(SOURCES
    main.cpp
    source/Application.cpp
    source/FileCopying.cpp
    source/FileWatcher.cpp
    source/QRImageProcessor.cpp
)

# Определяем платформу
if(WIN32)
    message(STATUS "Configuring for Windows")
    # Указываем путь к vcpkg toolchain file
    set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    
    # Указываем путь к установленным библиотекам через vcpkg
    set(CMAKE_PREFIX_PATH "D:/GitHub/vcpkg/installed/x64-windows-static" ${CMAKE_PREFIX_PATH})
    
    # Включаем директории с заголовочными файлами
    include_directories(
        "D:/GitHub/vcpkg/installed/x64-windows-static/include"
        "D:/GitHub/vcpkg/installed/x64-windows-static/include/opencv2"
    )
else()
    message(STATUS "Configuring for Linux")
    # Включаем директории с заголовочными файлами
    include_directories(
        "/usr/include/opencv4"
    )

    # Используем pkg-config только на Linux
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(QRENCODE REQUIRED libqrencode)
    if(QRENCODE_FOUND)
        include_directories(${QRENCODE_INCLUDE_DIRS})
    else()
        message(FATAL_ERROR "[| libqrencode |] not found!")
    endif()
endif()

# Добавление исполняемого файла
add_executable(MyApplication ${SOURCES})

# Поиск OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    target_link_libraries(MyApplication PRIVATE ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "[| OpenCV |] not found!")
endif()

# Линковка libqrencode
if(WIN32)
    # На Windows ищем libqrencode вручную, если find_package не работает
    find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h PATHS "D:/GitHub/vcpkg/installed/x64-windows-static/include")
    find_library(QRENCODE_LIBRARY NAMES qrencode PATHS "D:/GitHub/vcpkg/installed/x64-windows-static/lib")

    if(QRENCODE_INCLUDE_DIR AND QRENCODE_LIBRARY)
        message(STATUS "Found qrencode: ${QRENCODE_LIBRARY}")
        include_directories(${QRENCODE_INCLUDE_DIR})
        target_link_libraries(MyApplication PRIVATE ${QRENCODE_LIBRARY})
    else()
        message(FATAL_ERROR "[| libqrencode | not found on Windows!]")
    endif()
else()
    # На Linux используем pkg-config
    target_link_libraries(MyApplication PRIVATE ${QRENCODE_LIBRARIES})
endif()

# Поиск Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
    target_link_libraries(MyApplication PRIVATE Boost::system Boost::filesystem)
else()
    message(FATAL_ERROR "[| Boost |] not found!")
endif()

# Поиск exiv2
find_package(exiv2 REQUIRED)
if (exiv2_FOUND)
    target_link_libraries(MyApplication PUBLIC exiv2lib)
else()
    message(FATAL_ERROR "[| exiv2 |] not found!")
endif()



