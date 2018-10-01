dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.63])
AC_INIT([ClanBomber], [2.3.0], [rsl@members.fsf.org])
AM_INIT_AUTOMAKE([-Wall -Werror])


#CXXFLAGS="-std=c++11 -Ofast -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -lpthread -DNDEBUG"
CXXFLAGS="-std=c++11 -Ofast -march=native -ftree-vectorize -lpthread -DNDEBUG"
#CXXFLAGS="-std=c++11 -Ofast -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -lpthread -DNDEBUG"

#CFLAGS="-std=c++11 -Ofast -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -lpthread -DNDEBUG"
CFLAGS="-std=c++11 -Ofast -march=native -ftree-vectorize -lpthread -DNDEBUG"
#CFLAGS="-std=c++11 -Ofast -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -lpthread -DNDEBUG"

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
AX_BOOST_ASIO
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM
#AX_BOOST_THREAD


#
# Check for SDL 1.2.0
#

PKG_CHECK_MODULES(
	[SDL],
	[sdl >= 1.2.0],
	,
	AC_MSG_ERROR([*** SDL required])
)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

#
# Check for SDL_image
#

AC_CHECK_LIB(
	[SDL_image],
	[IMG_Load],
	[SDL_IMAGE_LIBS=-lSDL_image],
	AC_MSG_ERROR([*** SDL_image requiered])
)
AC_SUBST(SDL_IMAGE_LIBS)

#
# Check for SDL_mixer
#

AC_CHECK_LIB(
	[SDL_mixer],
	[Mix_OpenAudio],
	[SDL_MIXER_LIBS=-lSDL_mixer],
	AC_MSG_ERROR([*** SDL_mixer requiered])
)
AC_SUBST(SDL_MIXER_LIBS)

#
# Check for SDL_ttf
#

AC_CHECK_LIB(
	[SDL_ttf],
	[TTF_OpenFont],
	[SDL_TTF_LIBS=-lSDL_ttf],
	AC_MSG_ERROR([*** SDL_ttf requiered])
)
AC_SUBST(SDL_TTF_LIBS)

#
# Check for SDL_gfx
#

AC_CHECK_LIB(
	SDL_gfx,
	[zoomSurface],
	[SDL_GFX_LIBS=-lSDL_gfx],
	AC_MSG_ERROR([*** SDL_gfx requiered])
)
AC_SUBST(SDL_GFX_LIBS)

#
# Enable TCP only
#

AC_ARG_ENABLE(
	[tcp-only],
	[  --enable-tcp-only       enable tcp-only networking/protocol (default=no)],
	,
	[enable_tcp_only=no]
)

if test x$enable_tcp_only = xyes; then
	AC_DEFINE(ENABLE_TCP_ONLY, 1, [define if you want tcp-only networking/protocol])
fi

#
# Enable console
#

AC_ARG_ENABLE(
	[console],
	[  --enable-console       enable console (default=no)],
	,
	[enable_console=no]
)
AM_CONDITIONAL([ENABLE_CONSOLE], [test x$enable_console = xyes])
if test x$enable_console = xyes; then
   	AX_BOOST_THREAD
	AC_DEFINE([ENABLE_CONSOLE], 1, [define if you want a console])
	#
	# Check for Guile 1.8
	#
	PKG_CHECK_MODULES(
		[LIBGUILE],
		[guile-1.8 >= 1.8.0],
		,
		AC_MSG_ERROR([*** Libguile is required for console])
	)
fi
AC_SUBST(LIBGUILE_LIBS)

#
# Enable relative paths
#

AC_ARG_ENABLE(
	[relative-paths],
	[  --enable-relative-paths        relative paths (default=no)]
)
if test "$enable_relative_paths" = yes
then
	adl_COMPUTE_STANDARD_RELATIVE_PATHS
	RELATIVE_DATADIR=$bindir_backward_relative_path/$datadir_forward_relative_path
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
