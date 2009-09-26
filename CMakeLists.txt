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

# Zakomentovat pro vytvoření verze pro normální uživatele (méně dostupných
# funkcí). Správcovská verze má dostupnou editaci učitelů, předmětů atd.
# Funkce jsou zakázány jen "disablováním" příslušných tlačítek v GUI, takže je
# možnost, že se to někomu povede překonat. Editace se přímo nezakazuje, prostě
# nemá smysl, protože při další aktualizaci se změny ztratí.

#set(ADMIN_VERSION)

# Zdrojáky programu
add_subdirectory(src)
