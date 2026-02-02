cmake_minimum_required(VERSION 3.16)
project(do_an1 VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

find_package(Qt6 QUIET COMPONENTS Widgets Core SerialPort)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    wit_c_sdk.c
    wit_c_sdk.h
    REG.h
)

qt_add_executable(do_an1 ${PROJECT_SOURCES})

target_link_libraries(do_an1 PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::SerialPort
)
