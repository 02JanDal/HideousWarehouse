include_directories(${CMAKE_SOURCE_DIR})

set(HW_CPP_SRCS
    cpp/CppPlugin.h
    cpp/CppPlugin.cpp
    cpp/CppCompletionModel.h
    cpp/CppCompletionModel.cpp
    cpp/CppDocument.h
    cpp/CppDocument.cpp
    cpp/CppManager.h
    cpp/CppManager.cpp
    cpp/CppOutlineModel.h
    cpp/CppOutlineModel.cpp
    cpp/CppSyntaxHighlighter.h
    cpp/CppSyntaxHighlighter.cpp
    cpp/CppUnit.h
    cpp/CppUnit.cpp
)
add_library(HideousWarehouseCpp SHARED ${HW_CPP_SRCS})
qt5_use_modules(HideousWarehouseCpp Core Gui)
target_link_libraries(HideousWarehouseCpp HideousWarehouseCore HideousWarehouseUtil)

set(HW_CMAKE_SRCS
    cmake/CMakePlugin.h
    cmake/CMakePlugin.cpp
    cmake/CMakeProject.h
    cmake/CMakeProject.cpp
)
add_library(HideousWarehouseCMake SHARED ${HW_CMAKE_SRCS})
qt5_use_modules(HideousWarehouseCMake Core)
target_link_libraries(HideousWarehouseCMake HideousWarehouseCore HideousWarehouseUtil)
