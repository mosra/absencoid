#ifndef CONFIGURE_H
#define CONFIGURE_H

#define SVN_VERSION "${SVN_VERSION}"

#cmakedefine ADMIN_VERSION

/*
 * Další místa, kde změnit číslo verze:
 *      /CHANGELOG.txt
 *      /installer.nsi
 */

#ifdef ADMIN_VERSION
#define APP_VERSION         "1.1"
#else
#define APP_VERSION         "1.1bfu"
#endif

#define APP_VERSION_LONG    "1.1"

#endif
