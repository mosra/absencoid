find_package(Qt4 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

# Nezobrazování konzolového okna pod Windows
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -Wl,-subsystem,windows")
endif(WIN32)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

qt4_wrap_cpp(AbsencoidMOC
    MainWindow.h
    SummaryTab.h
    Update.h
    HottestModel.h
    CreateUpdateDialog.h
    UpdateDialog.h
    TeachersModel.h
    TeachersTab.h
    ClassesModel.h
    ClassesTab.h
    TimetableModel.h
    TimetableTab.h
    ChangesModel.h
    ChangesTab.h
    AbsencesModel.h
    AbsencesTab.h
    )

# Ve FindQt4.cmake je schovaná volba, že v qt4_add_resources můžu za OPTIONS
# narvat parametry pro rcc. Default parametry (threshold) pro rcc jsou
# asi nedokumentované.

# Resources (textová data, proto komprese)
qt4_add_resources(AbsencoidRCC data.qrc OPTIONS -compress 9 -threshold 0)

# Grafika (tu nekomprimovat)
qt4_add_resources(GraphicsRCC ../graphics/graphics.qrc)

# Zdrojáky
set(AbsencoidSRC
    main.cpp MainWindow.cpp AboutDialog.cpp
    ComboBoxDelegate.cpp DateEditDelegate.cpp SpinBoxDelegate.cpp
    SummaryTab.cpp CreateUpdateDialog.cpp UpdateDialog.cpp
    Dump.cpp Update.cpp HottestModel.cpp
    TeachersModel.cpp TeachersTab.cpp
    ClassesModel.cpp ClassesTab.cpp
    TimetableModel.cpp TimetableTab.cpp
    ChangesModel.cpp ChangesTab.cpp
    AbsencesModel.cpp AbsencesTab.cpp
    )

# Ikona programu (pro Windows)
if(WIN32)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <FLAGS> -o <OBJECT> -i <SOURCE>")
    set(AbsencoidSRC ${AbsencoidSRC} ${CMAKE_SOURCE_DIR}/graphics/resource.rc)
endif(WIN32)

add_executable(absencoid ${AbsencoidMOC} ${AbsencoidRCC} ${GraphicsRCC} ${AbsencoidSRC})
target_link_libraries(absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTNETWORK_LIBRARY})
