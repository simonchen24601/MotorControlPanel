cmake_minimum_required(VERSION 3.16)

project(MotorControlPanel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui SerialPort Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h

)

qt_standard_project_setup()

qt_add_executable(MotorControlPanel
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    messageinterface.h messageinterface.cpp
    controller.h controller.cpp
    settingsdialog.ui
    mainwindow.ui
    console.h
    console.cpp
    settingsdialog.h
    settingsdialog.cpp
    terminal.qrc
)

set_target_properties(MotorControlPanel PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(MotorControlPanel PRIVATE Qt6::Widgets Qt6::SerialPort Qt::Gui Qt::Core Xinput)

# Resources:
set(resource_files
    "images/application-exit.png"
    "images/clear.png"
    "images/connect.png"
    "images/disconnect.png"
    "images/settings.png"
)

qt_add_resources(MotorControlPanel "MotorControlPanel"
    PREFIX
        "/"
    FILES
        ${resource_files}
)
