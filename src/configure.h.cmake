#ifndef CONFIGURE_H
#define CONFIGURE_H

#define SVN_VERSION "${SVN_VERSION}"

#cmakedefine ADMIN_VERSION

#ifdef ADMIN_VERSION
#define APP_VERSION         "0.92"
#else
#define APP_VERSION         "0.92bfu"
#endif

#define APP_VERSION_LONG    "1.0 beta"

#endif
