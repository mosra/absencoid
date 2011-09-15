Absencoid is an old and largely unmaintained project for preventing problems
when you are intentionally not going to school. The administrator manages
timetables and changes and users are just writing down their absences and the
application automatically counts absence percentage and percentage forecast for
whole semester.

The application is licensed under [WTFPL]("http://en.wikipedia.org/wiki/WTFPL").

**Warning: All code comments and user interface is in Czech. Class and function
names are in English, fortunately.**

FEATURES
========

 * Multiplatform, written in C++/Qt with NSIS installer for Windows.
 * Two editions, one for admin and one for users with simplified interface.
 * Ability to make backups and synchronize user installations with online
   updates.

INSTALLATION
============

Dependencies
------------

 * CMake    - for building
 * Qt

Compilation on Linux
--------------------

The application doesn't have any packaging scripts for Linux distribution and
actually it was used directly from the build dir.

    mkdir -p build
    cd build
    cmake .. && make

If you want to compile admin version, run cmake with `-DADMIN_VERSION=ON`.

Compilation for Windows on ArchLinux
------------------------------------

The Windows installer is generated using `make-installer.sh` script and it
probably works only in ArchLinux. Additional dependencies you need for
crosscompiling:

 * nsis
 * mingw32-gcc
 * mingw32-qt

Then run `./make-installer.sh` or `./make-installer.sh admin` if you want to
compile admin version.

CONTACT
=======

Do you want to continue development of this application or use it for your
purpose? Feel free to contact me at:

 * Website - http://mosra.cz/blog/
 * GitHub - http://github.com/mosra/absencoid
 * E-mail - mosra@centrum.cz
 * Jabber - mosra@jabbim.cz
