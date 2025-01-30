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

# Определяем платформу
if(WIN32)
    message(STATUS "Configuring for Windows")
    include_directories(
        "D:/GitHub/vcpkg/installed/x64-windows/include"
        "D:/GitHub/vcpkg/installed/x64-windows/include/opencv2"
    )
    set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg" CACHE STRING "Vcpkg toolchain file")
else()
    message(STATUS "Configuring for Linux")
    include_directories(
        "/usr/include/opencv4"
    )
endif()

# Добавление исполняемого файла
add_executable(MyApplication ${SOURCES})

# Поиск OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(MyApplication PRIVATE ${OpenCV_LIBS})

# Поиск libqrencode через pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(QRENCODE REQUIRED libqrencode)
include_directories(${QRENCODE_INCLUDE_DIRS})
target_link_libraries(MyApplication PRIVATE ${QRENCODE_LIBRARIES})

# Поиск Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)
target_link_libraries(MyApplication PRIVATE Boost::system Boost::filesystem)

# Поиск exiv2
# find_package(exiv2 REQUIRED PATHS /usr/local/lib/cmake/exiv2)
# target_link_libraries(MyApplication PUBLIC Exiv2::Exiv2)
# Поиск exiv2
find_package(exiv2 REQUIRED)
if (exiv2_FOUND)
    # Вариант 1: Используйте имя цели из официальной документации
    target_link_libraries(MyApplication PUBLIC exiv2lib)

    # Вариант 2: Или попробуйте альтернативные имена
    # target_link_libraries(MyApplication PUBLIC Exiv2::exiv2lib)
    # target_link_libraries(MyApplication PUBLIC exiv2::exiv2)
else()
    message(FATAL_ERROR "[| exiv2 |] not found!")
endif()

# cmake_minimum_required(VERSION 3.10)
# project(MyApplication)
#
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")
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
# # Определяем платформу и устанавливаем пути к заголовочным файлам
# if(WIN32)
#     message(STATUS "Configuring for Windows")
#     include_directories(
#         "D:/GitHub/vcpkg/installed/x64-windows/include"
#         "D:/GitHub/vcpkg/installed/x64-windows/include/opencv2"
#     )
#     set(CMAKE_TOOLCHAIN_FILE "D:/GitHub/vcpkg/scripts/buildsystems/vcpkg" CACHE STRING "Vcpkg toolchain file")
# else()
#     message(STATUS "Configuring for Linux")
#     include_directories(
#         "/usr/include/opencv4"
#         "/usr/include/opencv2"
#     )
# endif()
#
# # Добавление исполняемого файла
# add_executable(MyApplication ${SOURCES})
#
#
#
# # Поиск OpenCV
# find_package(OpenCV REQUIRED)
# if(OpenCV_FOUND)
#     target_link_libraries(MyApplication PRIVATE ${OpenCV_LIBS})
# else()
#     message(FATAL_ERROR "[| OpenCV |] not found!")
# endif()
#
#
#
# # Поиск libqrencode
# # find_package(QRencode REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(QRENCODE REQUIRED libqrencode)
# if(QRENCODE_FOUND)
#     target_link_libraries(MyApplication PRIVATE QRencode)
#     # target_link_libraries(MyApplication PRIVATE QRencode::QRencode)
# else()
#     message(FATAL_ERROR "[| libqrencode |] not found!")
# endif()
#
#
#
# # Поиск Boost
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# if (Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     target_link_libraries(MyApplication PRIVATE Boost::system Boost::filesystem)
# else()
#     message(FATAL_ERROR "[| Boost |] not found!")
# endif()
#
#
#
# # Поиск exiv2
# find_package(exiv2 REQUIRED)
# if (exiv2_FOUND)
#     include_directories(${exiv2_INCLUDE_DIRS})
#     # Работало с виндой но в линуксе не работает
#     # target_link_libraries(MyApplication PUBLIC Exiv2::exiv2lib)
#     target_link_libraries(MyApplication PUBLIC Exiv2)
# else()
#     message(FATAL_ERROR "[| exiv2 |] not found!")
# endif()
#
