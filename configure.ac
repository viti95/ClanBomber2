dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.63])
AC_INIT([ClanBomber], [2.3.0], [rsl@members.fsf.org])
AM_INIT_AUTOMAKE([-Wall -Werror])


#CXXFLAGS="-std=c++11 -O3 -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -DNDEBUG"
CXXFLAGS="-std=c++11 -O3 -march=native -ftree-vectorize -DNDEBUG"
#CXXFLAGS="-std=c++11 -O3 -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -DNDEBUG"
#CXXFLAGS="-std=c++11 -g"

#CFLAGS="-std=c++11 -O3 -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -DNDEBUG"
CFLAGS="-std=c++11 -O3 -march=native -ftree-vectorize -DNDEBUG"
#CFLAGS="-std=c++11 -O3 -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -DNDEBUG"
#CFLAGS="-std=c++11 -g"

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
AX_BOOST_FILESYSTEM
AX_BOOST_SYSTEM

#
# Check for SDL 2.0.0
#

SDL_VERSION="2.0.0"
AM_PATH_SDL2(
	$SDL_VERSION,
	:,
	AC_MSG_ERROR([*** SDL2 required])
)

#AC_SUBST(SDL_CFLAGS)
#AC_SUBST(SDL_LIBS)

#
# Check for SDL2_image
#

AC_CHECK_LIB(
	[SDL2_image],
	[IMG_Load],
	[SDL2_IMAGE_LIBS=-lSDL2_image],
	AC_MSG_ERROR([*** SDL2_image requiered])
)
AC_SUBST(SDL2_IMAGE_LIBS)

#
# Check for SDL2_mixer
#

AC_CHECK_LIB(
	[SDL2_mixer],
	[Mix_OpenAudio],
	[SDL2_MIXER_LIBS=-lSDL2_mixer],
	AC_MSG_ERROR([*** SDL2_mixer requiered])
)
AC_SUBST(SDL2_MIXER_LIBS)

#
# Check for SDL2_ttf
#

AC_CHECK_LIB(
	[SDL2_ttf],
	[TTF_OpenFont],
	[SDL2_TTF_LIBS=-lSDL2_ttf],
	AC_MSG_ERROR([*** SDL2_ttf requiered])
)
AC_SUBST(SDL2_TTF_LIBS)

#
# Check for SDL2_gfx
#

AC_CHECK_LIB(
	SDL2_gfx,
	[zoomSurface],
	[SDL2_GFX_LIBS=-lSDL2_gfx],
	AC_MSG_ERROR([*** SDL2_gfx requiered])
)
AC_SUBST(SDL2_GFX_LIBS)

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
