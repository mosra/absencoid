cmake_minimum_required(VERSION 2.6)

# Název projektu
project(Absenciod)

# Verze ze SVN
find_package(Subversion)
if(Subversion_FOUND)
    Subversion_WC_INFO(${CMAKE_HOME_DIRECTORY} Absencoid)
    set(SVN_VERSION "r${Absencoid_WC_REVISION}")
else(Subversion_FOUND)
    set(SVN_VERSION "unknown")
endif(Subversion_FOUND)

message(STATUS "Verze: SVN ${SVN_VERSION}")

# Zdrojáky programu
add_subdirectory(src)
