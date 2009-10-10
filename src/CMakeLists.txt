find_package(Qt4 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

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

add_executable(absencoid
    main.cpp MainWindow.cpp AboutDialog.cpp
    ComboBoxDelegate.cpp DateEditDelegate.cpp SpinBoxDelegate.cpp
    SummaryTab.cpp CreateUpdateDialog.cpp UpdateDialog.cpp
    Dump.cpp Update.cpp HottestModel.cpp
    TeachersModel.cpp TeachersTab.cpp
    ClassesModel.cpp ClassesTab.cpp
    TimetableModel.cpp TimetableTab.cpp
    ChangesModel.cpp ChangesTab.cpp
    AbsencesModel.cpp AbsencesTab.cpp
    ${AbsencoidMOC} ${AbsencoidRCC} ${GraphicsRCC}
    )
target_link_libraries(absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTNETWORK_LIBRARY})
