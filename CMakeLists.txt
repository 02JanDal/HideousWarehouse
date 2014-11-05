cmake_minimum_required(VERSION 3.0)

project(CEdit)

find_package(Qt5 REQUIRED Core Gui Widgets Qml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CEDIT_SRCS
    Project.h
    Project.cpp
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
    File.h
    File.cpp
    BaseException.h
    BaseException.cpp
    Document.h
    Document.cpp
    main.cpp
)

add_executable(CEdit ${CEDIT_SRCS})
qt5_use_modules(CEdit Core Gui Widgets Qml)
