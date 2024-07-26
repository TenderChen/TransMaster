cmake_minimum_required(VERSION 3.19)
project(TransMaster LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    REQUIRED    
    COMPONENTS
        Core
        Gui
        Widgets
        LinguistTools
)
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES zh en)

set(PROJECT_SOURCES
    main.cpp
    TransMaster.ui
    TransMaster.h
    TransMaster.cpp
    TransMaster.rc
)

qt_add_executable(TransMaster ${PROJECT_SOURCES})



set_target_properties(TransMaster
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(TransMaster
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

qt_add_translations(TransMaster)
