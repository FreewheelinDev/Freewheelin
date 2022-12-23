cmake_minimum_required(VERSION 3.21)
project(Freewheelin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.6")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        MultimediaWidgets
        REQUIRED)

set(QT_SOURCE_FILES
        res.qrc
        )

add_executable(Freewheelin main.cpp MainWindow.cpp MainWindow.h MainWindow.ui res.qrc ThePlayer.h TheButton.cpp TheButton.h EditDurationBox.cpp EditDurationBox.h)
target_link_libraries(Freewheelin
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Multimedia
        Qt5::MultimediaWidgets
        )
