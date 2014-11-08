cmake_minimum_required(VERSION 3.0)

project(HideousWarehouse)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Qml)
find_package(LibClang REQUIRED)

set(CEDIT_SRCS
    Project.h
    Project.cpp
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
    File.h
    File.cpp
    Json.h
    Json.cpp
    BaseException.h
    BaseException.cpp
    Document.h
    Document.cpp
    CppParser.h
    CppParser.cpp
    TextEdit.h
    TextEdit.cpp

    main.cpp
)

include_directories(${LibClang_INCLUDE_DIRS})

add_executable(HideousWarehouse ${CEDIT_SRCS})
qt5_use_modules(HideousWarehouse Core Gui Widgets Qml)
target_link_libraries(HideousWarehouse ${LibClang_LIBRARIES})
