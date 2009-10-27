/*
 * Moderní UI
 */

    !include "MUI2.nsh"

/*
 * Hlavní nastavení
 */

    /* Název instaláku a výstupní soubor */
    Name "Absencoid"
    OutFile "absencoid-1.0-beta.exe"

    /* Default adresář pro instalaci */
    InstallDir "$PROGRAMFILES\Absencoid"

    /* Jinak ho získáme z registrů */
    InstallDirRegKey HKCU "Software\Absencoid" ""

    /* Privilegia administrátora pro Win Vista+ */
    RequestExecutionLevel admin

/*
 * Globální nastavení
 */

    /* TODO: Obrázky

    !define MUI_HEADERIMAGE

    /* Bitmapa pro hlavičku - 150x57 px * /
    !define MUI_HEADERIMAGE_BITMAP "header.bmp"

    /* Bitmapa pro první stránku - 164x314 px * /
    !define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp"

    */

    /* Varování při ukončení */
    !define MUI_ABORTWARNING

/*
 * Jednotlivé stránky
 */

    /* Uvítací stránka */
    !define MUI_WELCOMEPAGE_TITLE "Absencoid 1.0 beta"
    !define MUI_WELCOMEPAGE_TEXT "Tento instalační program provede instalaci Absencoidu na váš počítač."
    !insertmacro MUI_PAGE_WELCOME

    /* Licence (zatím jen changelog) - převedený z UTF-8 */
    !insertmacro MUI_PAGE_LICENSE "_CHANGELOG.txt"

    /* Nastavení instalačního adresáře */
    !insertmacro MUI_PAGE_DIRECTORY

    /* Stránka se samotnou instalací */
    !insertmacro MUI_PAGE_INSTFILES

    /* Stránky v odinstaláku */
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES

/*
 * Jazyky
 */

  !insertmacro MUI_LANGUAGE "Czech"

/*
 * Sekce instaláku
 */

Section "Instalační sekce" SecDummy

    SetOutPath "$INSTDIR"

    File "absencoid.exe"
    File "CHANGELOG.txt"
    File "mingwm10.dll"
    File "QtCore4.dll"
    File "QtGui4.dll"
    File "QtNetwork4.dll"
    File "QtSql4.dll"
    File "QtXml4.dll"

    /* Jedno DLL musí být v podadresáři */
    SetOutPath "$INSTDIR\sqldrivers"
    File "sqldrivers\qsqlite4.dll"

    /* Uložíme adresář instalace */
    WriteRegStr HKCU "Software\Absencoid" "" $INSTDIR

    /* Vytvoříme odinstalák */
    WriteUninstaller "$INSTDIR\uninstall.exe"

    /* Položka ve Start menu */
    CreateDirectory "$SMPROGRAMS\Absencoid"
    CreateShortCut  "$SMPROGRAMS\Absencoid\Absencoid.lnk" "$INSTDIR\absencoid.exe"
    CreateShortCut  "$SMPROGRAMS\Absencoid\Odinstalovat.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

/*
 * Odinstalák
 */

Section "Uninstall"

    Delete "$INSTDIR\absencoid.exe"
    Delete "$INSTDIR\CHANGELOG.txt"
    Delete "$INSTDIR\mingwm10.dll"
    Delete "$INSTDIR\QtCore4.dll"
    Delete "$INSTDIR\QtGui4.dll"
    Delete "$INSTDIR\QtNetwork4.dll"
    Delete "$INSTDIR\QtSql4.dll"
    Delete "$INSTDIR\QtXml4.dll"
    Delete "$INSTDIR\sqldrivers\qsqlite4.dll"

    Delete "$INSTDIR\uninstall.exe"

    RMDir "$INSTDIR\sqldrivers"
    RMDir "$INSTDIR"

    /* Smažeme položky ve Start menu */
    Delete "$SMPROGRAMS\Absencoid\Absencoid.lnk"
    Delete "$SMPROGRAMS\Absencoid\Odinstalovat.lnk"

    RMDir "$SMPROGRAMS\Absencoid"

    /* Smažeme po sobě i registry */
    DeleteRegKey HKCU "Software\Modern UI Test"

SectionEnd