#!/bin/bash

builddir=build-win32
nsisdir=build-nsis

# Build
mkdir -p $nsisdir
mkdir -p $builddir ; cd $builddir

# Různé edice programu
if [ $# -gt 0 -a "$1" = "admin" ] ; then
    sed -e 's/uživatelská edice/správcovská edice/g' \
        -e 's/-user.exe/-admin.exe/g' ../installer.nsi \
        -e 's/\[uživatel\]/[správce]/g' > ../$nsisdir/installer.nsi
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/archlinux/Qt4-mingw32.cmake -DADMIN_VERSION=ON .. || exit 1
else
    cp ../installer.nsi ../$nsisdir/
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/archlinux/Qt4-mingw32.cmake -DADMIN_VERSION=OFF .. || exit 1
fi

make -j3 || exit 1
cd ..

# Vytvoření adresáře pro tvoření instaláku, naplnění soubory
cd $nsisdir
cp ../CHANGELOG.txt . || exit 1
cp ../$builddir/src/absencoid.exe . || exit 1

# Osekání HTML z disclaimeru, spojení s changelogem do jednoho souboru
sed -e 's/<[^>]*>//g' ../data/disclaimer.txt > disclaimer.txt
echo -e "\n\nNovinky a změny v této verzi:\n" >> disclaimer.txt
cat ../CHANGELOG.txt >> disclaimer.txt

i486-mingw32-strip absencoid.exe

# Zkopírování potřebných DLL (pokud zde ještě nejsou)
for dll in mingwm10.dll libgcc_s_sjlj-1.dll libstdc++-6.dll QtCore4.dll QtGui4.dll QtNetwork4.dll QtSql4.dll QtXml4.dll ; do
    [ -e $dll ] || cp /usr/i486-mingw32/bin/$dll . || exit 1
done
[ -e libgcc_s_dw2-1.dll ] || cp /usr/i486-mingw32/bin/qt-bin/libgcc_s_dw2-1.dll . || exit 1

# Jedno DLL v podsložce
mkdir -p sqldrivers
[ -e sqldrivers/qsqlite4.dll ] || \
    cp /usr/i486-mingw32/bin/qt-plugins/sqldrivers/qsqlite4.dll sqldrivers/ || exit 1

# Konverze do nativního Windows kódování
iconv -f UTF-8 -t Windows-1250 installer.nsi -o _installer.nsi
iconv -f UTF-8 -t Windows-1250 disclaimer.txt -o _disclaimer.txt

# Konverze obrázků do BMP
convert ../graphics/nsis-header.png BMP3:nsis-header.bmp
convert ../graphics/nsis-welcome.png BMP3:nsis-welcome.bmp

# Vytvoření instaláku
makensis _installer.nsi || exit 1

# Úklid
rm _installer.nsi _disclaimer.txt
