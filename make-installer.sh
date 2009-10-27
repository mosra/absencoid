#!/bin/bash

builddir=build-win32
nsisdir=build-nsis

# Build
mkdir -p $builddir ; cd $builddir

cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-Qt4-mingw32.cmake .. || exit 1
make -j3 || exit 1
cd ..

# Vytvoření adresáře pro tvoření instaláku, naplnění soubory
mkdir -p $nsisdir ; cd $nsisdir
cp ../$builddir/src/absencoid.exe . || exit 1
cp ../CHANGELOG.txt . || exit 1

i486-mingw32-strip absencoid.exe

# Zkopírování potřebných DLL (pokud zde ještě nejsou)
for dll in mingwm10.dll QtCore4.dll QtGui4.dll QtNetwork4.dll QtSql4.dll QtXml4.dll ; do
    [ -e $dll ] || cp /usr/i486-mingw32/bin/$dll . || exit 1
done

# Jedno DLL v podsložce
mkdir -p sqldrivers
[ -e sqldrivers/qsqlite4.dll ] || \
    cp /usr/i486-mingw32/bin/plugins/sqldrivers/qsqlite4.dll sqldrivers/ || exit 1

# Konverze do nativního Windows kódování
iconv -f UTF-8 -t Windows-1250 ../installer.nsi -o _installer.nsi
iconv -f UTF-8 -t Windows-1250 CHANGELOG.txt -o _CHANGELOG.txt

# Vytvoření instaláku
makensis _installer.nsi || exit 1

# Úklid
rm _installer.nsi _CHANGELOG.txt
