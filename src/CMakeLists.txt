find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(AbsencoidMOC MainWindow.h)
add_executable(Absencoid main.cpp MainWindow.cpp ${AbsencoidMOC})
target_link_libraries(Absencoid ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
