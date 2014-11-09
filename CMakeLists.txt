cmake_minimum_required(VERSION 3.0)

project(HideousWarehouse)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED Core Gui Widgets Qml)
find_package(LibClang REQUIRED)

set(HW_UTIL_SRCS
    util/File.h
    util/File.cpp
    util/Json.h
    util/Json.cpp
    util/BaseException.h
    util/BaseException.cpp
)
add_library(HideousWarehouseUtil SHARED ${HW_UTIL_SRCS})
qt5_use_modules(HideousWarehouseUtil Core)

set(HW_CORE_SRCS
    core/DocumentModel.h
    core/DocumentModel.cpp
    core/BaseProject.h
    core/BaseProject.cpp
    core/BaseDocument.h
    core/BaseDocument.cpp
    core/PluginManager.h
    core/PluginManager.cpp
    core/IPlugin.h

    core/GitIgnoreFilterModel.h
    core/GitIgnoreFilterModel.cpp
)
add_library(HideousWarehouseCore SHARED ${HW_CORE_SRCS})
qt5_use_modules(HideousWarehouseCore Core Gui Widgets)
target_link_libraries(HideousWarehouseCore HideousWarehouseUtil)

set(HW_GUI_SRCS
    gui/MainWindow.h
    gui/MainWindow.cpp
    gui/MainWindow.ui
    gui/TextEdit.h
    gui/TextEdit.cpp
)
add_library(HideousWarehouseGui SHARED ${HW_GUI_SRCS})
qt5_use_modules(HideousWarehouseGui Core Gui Widgets)
target_link_libraries(HideousWarehouseGui HideousWarehouseCore HideousWarehouseUtil)

add_executable(HideousWarehouse main.cpp resources/resources.qrc)
qt5_use_modules(HideousWarehouse Core Gui Widgets Qml)
target_link_libraries(HideousWarehouse HideousWarehouseUtil HideousWarehouseCore HideousWarehouseGui ${LibClang_LIBRARIES})

include_directories(${LibClang_INCLUDE_DIRS})

add_subdirectory(plugins)
