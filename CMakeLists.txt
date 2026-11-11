cmake_minimum_required(VERSION 3.19)
project(Olamon LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(Olamon
    WIN32 MACOSX_BUNDLE
    README.md
    resources/resources.qrc

    src/main.cpp

    src/app/mainwindow.h src/app/mainwindow.cpp
    src/app/sidemenu.h src/app/sidemenu.cpp

    src/core/canvassettings.h
    src/core/document.h src/core/document.cpp
    src/core/fontmanager.h src/core/fontmanager.cpp

    src/writing/writingtab.h src/writing/writingtab.cpp
    src/writing/canvasview.h src/writing/canvasview.cpp
    src/writing/pageitem.h src/writing/pageitem.cpp
    src/writing/newcanvasdialog.h src/writing/newcanvasdialog.cpp

    src/writing/tools/tool.h src/writing/tools/tool.cpp
    src/writing/tools/drawparagraphtool.h src/writing/tools/drawparagraphtool.cpp
    src/writing/tools/texttool.h src/writing/tools/texttool.cpp
    src/writing/tools/transformtool.h src/writing/tools/transformtool.cpp
    src/writing/paragraphitem.h src/writing/paragraphitem.cpp
    src/writing/paragraphitem.h src/writing/paragraphitem.cpp
)

# lets the code keep using plain includes like "document.h"
target_include_directories(Olamon PRIVATE
    src/app
    src/core
    src/writing
    src/writing/tools
)

# qt headers get compiled once and reused, makes builds a lot faster
target_precompile_headers(Olamon PRIVATE <QtWidgets>)

target_link_libraries(Olamon
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Olamon
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Olamon
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})