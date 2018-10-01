SUBDIRS = maps pics wavs fonts menuentry

clanbomber2dir = $(RELATIVE_DATADIR)/$(PACKAGE)
relative_localedir = $(RELATIVE_DATADIR)/locale

AM_CPPFLAGS = \
	$(SDL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-DCB_DATADIR=\"$(clanbomber2dir)\" \
	-DCB_LOCALEDIR=\"$(relative_localedir)\"

LDADD = \
	$(SDL_LIBS) \
	$(SDL_IMAGE_LIBS) \
	$(SDL_MIXER_LIBS) \
	$(SDL_TTF_LIBS) \
	$(SDL_GFX_LIBS) \
	$(LIBINTL) \
	$(LIBGUILE_LIBS) \
	$(BOOST_LDFLAGS) \
	$(BOOST_ASIO_LIB) \
	$(BOOST_SYSTEM_LIB) \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_THREAD_LIB)

bin_PROGRAMS = clanbomber2

clanbomber2_SOURCES = \
	Bomb.cpp \
	Bomb.h \
	Bomber.cpp \
	Bomber.h \
	Bomber_Corpse.cpp \
	Bomber_Corpse.h \
	ClanBomber.cpp \
	ClanBomber.h \
	GameConfig.cpp \
	GameConfig.h \
	Controller.cpp \
	Controller.h \
	Controller_AI.cpp \
	Controller_AI.h \
	Controller_AI_mass.cpp \
	Controller_AI_mass.h \
	Controller_Joystick.cpp \
	Controller_Joystick.h \
	Controller_RCMouse.cpp \
	Controller_RCMouse.h \
	Controller_Keyboard.cpp \
	Controller_Keyboard.h \
	Corpse_Part.cpp \
	Corpse_Part.h \
	Credits.cpp \
	Credits.h \
	Disease.cpp \
	Disease.h \
	Disease_Fast.cpp \
	Disease_Fast.h \
	Disease_Frozen.cpp \
	Disease_Frozen.h \
	Disease_PutBomb.cpp \
	Disease_PutBomb.h \
	Disease_Stoned.cpp \
	Disease_Stoned.h \
	Explosion.cpp \
	Explosion.h \
	Extra.cpp \
	Extra.h \
	Extra_Bomb.cpp \
	Extra_Bomb.h \
	Extra_Glove.cpp \
	Extra_Glove.h \
	Extra_Joint.cpp \
	Extra_Joint.h \
	Extra_Kick.cpp \
	Extra_Kick.h \
	Extra_Koks.cpp \
	Extra_Koks.h \
	Extra_Power.cpp \
	Extra_Power.h \
	Extra_Skateboard.cpp \
	Extra_Skateboard.h \
	Extra_Viagra.cpp \
	Extra_Viagra.h \
	GameObject.cpp \
	GameObject.h \
	GameStatus.cpp \
	GameStatus.h \
	GameStatus_Team.cpp \
	GameStatus_Team.h \
	Map.cpp \
	Map.h \
	MapEditor.cpp \
	MapEditor.h \
	MapEntry.cpp \
	MapEntry.h \
	MapSelector.cpp \
	MapSelector.h \
	MapTile.cpp \
	MapTile.h \
	MapTile_Arrow.cpp \
	MapTile_Arrow.h \
	MapTile_Box.cpp \
	MapTile_Box.h \
	MapTile_Ground.cpp \
	MapTile_Ground.h \
	MapTile_Ice.cpp \
	MapTile_Ice.h \
	MapTile_None.cpp \
	MapTile_None.h \
	MapTile_Trap.cpp \
	MapTile_Trap.h \
	MapTile_Wall.cpp \
	MapTile_Wall.h \
	Menu.cpp \
	Menu.h \
	Observer.cpp \
	Observer.h \
	PlayerSetup.cpp \
	PlayerSetup.h \
	Resources.cpp \
	Resources.h \
	Timer.cpp \
	Timer.h \
	Utils.cpp \
	Utils.h \
	gettext.h \
	UtilsCL_Vector.h \
	UtilsAccess.cpp \
	UtilsAccess.h \
	UtilsSleep.cpp \
	UtilsSleep.h \
	UtilsGetHome.cpp \
	UtilsGetHome.h \
	AudioBuffer.h \
	AudioBufferSDL.cpp \
	AudioBufferSDL.h \
	AudioSimple.h \
	AudioSimpleSDL.cpp \
	AudioSimpleSDL.h \
	Factories.hpp \
	FactoriesSDL.hpp \
	Music.h \
	MusicSDL.cpp \
	MusicSDL.h \
	Font.h \
	FontSDL.cpp \
	FontSDL.h \
	Surface.h \
	SurfaceSDL.cpp \
	SurfaceSDL.h \
	PluginManager.cpp \
	PluginManager.h
