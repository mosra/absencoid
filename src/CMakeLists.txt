find_package(Qt4 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

qt4_wrap_cpp(AbsencoidMOC
    MainWindow.h
    TeachersTab.h
    ClassesModel.h
    ClassesTab.h
    TimetableModel.h
    TimetableTab.h
    ChangedLessonsTab.h
    )
add_executable(absencoid
    main.cpp MainWindow.cpp ComboBoxDelegate.cpp DateEditDelegate.cpp
    TeachersModel.cpp TeachersTab.cpp
    ClassesModel.cpp ClassesTab.cpp
    TimetableModel.cpp TimetableTab.cpp
    ChangedLessonsModel.cpp ChangedLessonsTab.cpp
    ${AbsencoidMOC}
    )
target_link_libraries(absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY})
