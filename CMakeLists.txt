cmake_minimum_required(VERSION 3.16)

set(VER_MAJOR "0" CACHE STRING "Version major")
set(VER_MINOR "1" CACHE STRING "Version minor")
set(VER_PATCH "0" CACHE STRING "Version bugfix")

project(breeks LANGUAGES CXX VERSION ${VER_MAJOR}.${VER_MINOR}.${VER_PATCH})

add_definitions(-DPROGRAM_NAME="${CMAKE_PROJECT_NAME}")

add_definitions(-DVERSION_MAJOR=${VER_MAJOR})
add_definitions(-DVERSION_MINOR=${VER_MINOR})
add_definitions(-DVERSION_PATCH=${VER_PATCH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql QuickWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql QuickWidgets REQUIRED)

add_subdirectory(src)
