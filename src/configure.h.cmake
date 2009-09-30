#ifndef CONFIGURE_H
#define CONFIGURE_H

#define SVN_VERSION "${SVN_VERSION}"

#cmakedefine ADMIN_VERSION

#ifdef ADMIN_VERSION
#define APP_VERSION         "0.8a"
#else
#define APP_VERSION         "0.8u"
#endif

#define APP_VERSION_LONG    "1.0-dev"

#endif
