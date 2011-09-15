cmake_minimum_required(VERSION 2.6)

# Název projektu
project(Absenciod)

# Vytvoření správcovské verze nebo verze pro normální uživatele (méně dostupných
# funkcí). Správcovská verze má dostupnou editaci učitelů, předmětů atd.
# Funkce jsou zakázány jen "disablováním" příslušných tlačítek v GUI, takže je
# možnost, že se to někomu povede překonat. Editace se přímo nezakazuje, prostě
# nemá smysl, protože při další aktualizaci se změny ztratí.

OPTION(ADMIN_VERSION "Administrátorská verze" OFF)

# Zdrojáky programu
add_subdirectory(src)
