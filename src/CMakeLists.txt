find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

qt4_wrap_cpp(AbsencoidMOC MainWindow.h TeachersTab.h ClassesModel.h)
add_executable(absencoid main.cpp MainWindow.cpp TeachersModel.cpp TeachersTab.cpp ClassesModel.cpp ${AbsencoidMOC})
target_link_libraries(absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY})
