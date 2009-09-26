find_package(Qt4 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

qt4_wrap_cpp(AbsencoidMOC
    MainWindow.h
    SummaryTab.h
    CreateUpdateDialog.h
    TeachersTab.h
    ClassesModel.h
    ClassesTab.h
    TimetableModel.h
    TimetableTab.h
    ChangedLessonsTab.h
    )

# Ve FindQt4.cmake je schovaná volba, že v qt4_add_resources můžu za OPTIONS
# narvat parametry pro rcc. Default parametry (threshold) pro rcc jsou
# asi nedokumentované.

# Resources (textová data, proto komprese)
qt4_add_resources(AbsencoidRCC data.qrc OPTIONS -compress 9 -threshold 0)

add_executable(absencoid
    main.cpp MainWindow.cpp
    ComboBoxDelegate.cpp DateEditDelegate.cpp SpinBoxDelegate.cpp
    SummaryTab.cpp Dump.cpp CreateUpdateDialog.cpp
    TeachersModel.cpp TeachersTab.cpp
    ClassesModel.cpp ClassesTab.cpp
    TimetableModel.cpp TimetableTab.cpp
    ChangedLessonsModel.cpp ChangedLessonsTab.cpp
    ${AbsencoidMOC} ${AbsencoidRCC}
    )
target_link_libraries(absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QTXML_LIBRARY})
