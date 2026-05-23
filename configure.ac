dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.63])
AC_INIT([ClanBomber], [2.4.0])
AM_INIT_AUTOMAKE([-Wall -Werror])

AC_PROG_CXX

AC_LANG([C++])
AC_C_BIGENDIAN
AC_PROG_RANLIB
AM_GNU_GETTEXT_VERSION([0.19])
AM_GNU_GETTEXT([external])

#
# Check for SDL 3.0.0
#

PKG_CHECK_MODULES(SDL3, sdl3 sdl3-image sdl3-ttf sdl3-mixer)
AC_SUBST(SDL3_CFLAGS)
AC_SUBST(SDL3_LIBS)

#
# Enable relative paths
#

AC_ARG_ENABLE(
	[relative-paths],
	[  --enable-relative-paths        relative paths (default=no)]
)
if test "$enable_relative_paths" = yes
then
	RELATIVE_DATADIR="."
else
	RELATIVE_DATADIR=$datadir
fi
AC_SUBST(RELATIVE_DATADIR)


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/fonts/Makefile
	src/maps/Makefile
	src/menuentry/Makefile
	src/menuentry/clanbomber.desktop
	src/pics/Makefile
	src/wavs/Makefile
	po/Makefile.in
])

AC_OUTPUT
