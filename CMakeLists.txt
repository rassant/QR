
cmake_minimum_required(VERSION 3.10)
project(MyApplication)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# Укажите путь к вашим исходным файлам
set(SOURCES
    main.cpp
    source/Application.cpp
    source/FileCopying.cpp
    source/FileWatcher.cpp
    source/QRImageProcessor.cpp
)

# Укажите пути к заголовочным файлам
include_directories(
    "D:/GitHub/vcpkg/installed/x64-windows/include"
    "D:/GitHub/vcpkg/installed/x64-windows/include/opencv2"
)

# Укажите путь к vcpkg
set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg" CACHE STRING "Vcpkg toolchain file")
# set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Добавление исполняемого файла
add_executable(MyApplication ${SOURCES})

# Поиск OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(MyApplication PRIVATE ${OpenCV_LIBS})

# Поиск libqrencode
find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY_RELEASE NAMES qrencode)
find_library(QRENCODE_LIBRARY_DEBUG NAMES qrencoded)
set(QRENCODE_LIBRARIES optimized ${QRENCODE_LIBRARY_RELEASE} debug ${QRENCODE_LIBRARY_DEBUG})

# Поиск Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(MyApplication PRIVATE Boost::system Boost::filesystem)
else()
    message(FATAL_ERROR "Boost not found!")
endif()

# Поиск exiv2
# find_package(exiv2 REQUIRED)
find_package(exiv2 CONFIG REQUIRED)

if (exiv2_FOUND)
    include_directories(${exiv2_INCLUDE_DIRS})
    # target_link_libraries(MyApplication PRIVATE exiv2::exiv2)
    target_link_libraries(MyApplication PRIVATE Exiv2::exiv2lib)
else()
    message(FATAL_ERROR "exiv2 not found!")
endif()

# Линковка с библиотеками
target_link_libraries(MyApplication PRIVATE
    ${QRENCODE_LIBRARIES}
)
