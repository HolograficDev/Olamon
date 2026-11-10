cmake_minimum_required(VERSION 3.19)
project(Olamon LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(Olamon
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    README.md
    sidemenu.h sidemenu.cpp
    writingtab.h writingtab.cpp
    canvassettings.h
    document.h document.cpp
    newcanvasdialog.h newcanvasdialog.cpp
    pageitem.h pageitem.cpp
    canvasview.h canvasview.cpp
    fontmanager.h fontmanager.cpp
    resources.qrc
)

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