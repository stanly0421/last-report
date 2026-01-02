cmake_minimum_required(VERSION 3.16)

project(last-report VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    Network
)

# Try to find WebEngineWidgets
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS WebEngineWidgets QUIET)

if(NOT Qt${QT_VERSION_MAJOR}WebEngineWidgets_FOUND)
    message(FATAL_ERROR 
        "\n"
        "========================================\n"
        "ERROR: Qt WebEngineWidgets module not found!\n"
        "========================================\n"
        "\n"
        "This application requires Qt WebEngine to display embedded YouTube videos.\n"
        "\n"
        "To install Qt WebEngine:\n"
        "\n"
        "Ubuntu/Debian:\n"
        "  sudo apt-get install qt${QT_VERSION_MAJOR}-webengine-dev\n"
        "\n"
        "Fedora/RHEL:\n"
        "  sudo dnf install qt${QT_VERSION_MAJOR}-qtwebengine-devel\n"
        "\n"
        "macOS (Homebrew):\n"
        "  brew install qt@${QT_VERSION_MAJOR}\n"
        "\n"
        "Windows:\n"
        "  Install Qt WebEngine via Qt Maintenance Tool or download from qt.io\n"
        "\n"
        "After installation, try building again.\n"
        "========================================\n"
    )
endif()

set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
    widget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(last-report
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(last-report
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(last-report PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
)

# Set target properties
set_target_properties(last-report PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(last-report)
endif()

# Installation rules
install(TARGETS last-report
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
