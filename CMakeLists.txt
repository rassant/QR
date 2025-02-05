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
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(QRENCODE REQUIRED libqrencode)

if(UNIX AND NOT APPLE)  # Проверка — это Linux, но не MacOS
    message(STATUS "Configuring for Linux")
    
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(QRENCODE REQUIRED libqrencode)
endif()



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


if(UNIX AND NOT APPLE)  # Проверка — это Linux, но не MacOS
  pkg_check_modules(QRENCODE REQUIRED libqrencode)
  if(QRENCODE_FOUND)
    include_directories(${QRENCODE_INCLUDE_DIRS})
    target_link_libraries(OpenCVExample PRIVATE ${QRENCODE_LIBRARIES})
  else()
    message(FATAL_ERROR "[| libqrencode |] not found!")
  endif()
endif()



























#
#
#
#
#
#
#
#
# cmake_minimum_required(VERSION 3.10)
#
# project(OpenCVExample)
#
# # Установка стандарта C++
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# # Укажите путь к вашему vcpkg toolchain файлу
# if(WIN32)
#     set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
# else()
#     set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
# endif()
#
# # Укажите путь к установленным библиотекам
# if(WIN32)
#     set(CMAKE_PREFIX_PATH "D:/GitHub/vcpkg/installed/x64-windows")
# else()
#     set(CMAKE_PREFIX_PATH "/path/to/vcpkg/installed/x64-linux")  # Укажите путь для Linux
# endif()
#
# # Укажите путь к protoc
# if(WIN32)
#     set(Protobuf_PROTOC_EXECUTABLE "D:/GitHub/vcpkg/installed/x64-windows/tools/protobuf/protoc.exe")
# else()
#     set(Protobuf_PROTOC_EXECUTABLE "/path/to/vcpkg/installed/x64-linux/tools/protobuf/protoc")
# endif()
#
# find_package(Protobuf REQUIRED)
# find_package(OpenCV REQUIRED)
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# find_package(exiv2 CONFIG REQUIRED)
# find_package(Iconv REQUIRED)
# set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(Threads REQUIRED)
#
# # Список исходных файлов
# set(SOURCES
#     main.cpp
#     source/Application.cpp
#     source/FileCopying.cpp
#     source/FileWatcher.cpp
#     source/QRImageProcessor.cpp
#     source/InputData.cpp
#     source/FileCopyManager.cpp
# )
#
# # Создание исполняемого файла
# add_executable(OpenCVExample ${SOURCES})
#
# ##########################################################################
#
# # Линковка с библиотеками OpenCV
# if(OpenCV_FOUND)
#     message("~~~~~~~~~~~~~~~~~~~~~~~~!OpenCV FOUND!~~~~~~~~~~~~~~~~~~~~~~")
#     target_link_libraries(OpenCVExample PRIVATE ${OpenCV_LIBS})
# else()
#     message(FATAL_ERROR "OpenCV not found!")
# endif()
#
# # Линковка с библиотеками Iconv
# if(Iconv_FOUND)
#     message("~~~~~~~~~~~~~~~~~~~~~~!Iconv FOUND!~~~~~~~~~~~~~~~~~~~~~~")
#     target_link_libraries(OpenCVExample PRIVATE Iconv::Iconv)
# else()
#     message(FATAL_ERROR "Iconv not found!")
# endif()
#
# # Линковка с библиотеками Exiv2
# if(exiv2_FOUND)
#     message("~~~~~~~~~~~~~~~~~~~~~~!exiv2 FOUND!~~~~~~~~~~~~~~~~~~~~~~")
#     target_link_libraries(OpenCVExample PRIVATE Exiv2::exiv2lib)
# else()
#     message(FATAL_ERROR "Exiv2 not found!")
# endif()
#
# # Линковка с библиотеками Boost
# if(Boost_FOUND)
#     message("~~~~~~~~~~~~~~~~~~~~~~!Boost FOUND!~~~~~~~~~~~~~~~~~~~~~~")
#     target_link_libraries(OpenCVExample PRIVATE Boost::system)
#     target_link_libraries(OpenCVExample PRIVATE Boost::filesystem)
# else()
#     message(FATAL_ERROR "Boost not found!")
# endif()
#
# # Линковка с потоками
# if(Threads_FOUND)
#     message("~~~~~~~~~~~~~~~~~~~~~~!Threads FOUND!~~~~~~~~~~~~~~~~~~~~~~")
#     target_link_libraries(OpenCVExample PRIVATE Threads::Threads)
# else()
#     message(FATAL_ERROR "Threads not found!")
# endif()
#
