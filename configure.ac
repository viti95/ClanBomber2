dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.63])
AC_INIT([ClanBomber], [2.3.0], [rsl@members.fsf.org])
AM_INIT_AUTOMAKE([-Wall -Werror])

AC_PROG_CXX

AC_LANG([C++])
AC_C_BIGENDIAN
AC_PROG_RANLIB
AM_GNU_GETTEXT_VERSION([0.19])
AM_GNU_GETTEXT([external])

#
# Check for Boost 1.55, at least (filesystem)
#

AX_BOOST_BASE([1.55])
AX_BOOST_SYSTEM

#
# Check for SDL 2.0.0
#

PKG_CHECK_MODULES(SDL2, sdl2 SDL2_image SDL2_ttf SDL2_mixer)
AC_SUBST(SDL2_CFLAGS)
AC_SUBST(SDL2_LIBS)

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
