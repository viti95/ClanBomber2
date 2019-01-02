# ClanBomber2

This is a fork of ClanBomber2 [NonGNU](http://www.nongnu.org/clanbomber/), ported to SDL2

## Aditional features

* Ported to SDL2, is faster and better supported. Full hardware acceleration.
* Updated to C++11
* Windowed mode can be resized (internal resolution is still 800x600)
* Re-added music from ClanBomber (bud.mod, press F2 to activate in game)
* Optimized routines for low end computers (Raspberry Pi, netbooks, ...)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Linux Debian / Ubuntu / Raspbian

```
sudo apt-get install gcc g++ autopoint automake autoconf autoconf-archive gettext pkg-config libtool make libboost-all-dev libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

Windows (MSYS2 MinGW 64)

```
pacman -Syyu autoconf autoconf-archive automake pkg-config mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-boost mingw-w64-x86_64-make mingw-w64-x86_64-libtool
```

### Building

Download ClanBomber2 source code, under your project folder

```
git clone https://github.com/viti95/ClanBomber2.git
```

Update generated config files

```
cd ClanBomber2
autoreconf -fvi
```

Configure the project to your preference

* Linux Debian / Ubuntu

```
./configure CXXFLAGS="-O3 -march=native"
```

* Windows (MSYS2 MinGW 64)

```
./configure CXXFLAGS="-O3 -march=native" --with-boost=/mingw64
```

* Raspberry Pi 2

```
./configure CXXFLAGS="-O3 -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard" --with-boost-libdir=/usr/lib/arm-linux-gnueabihf
```

* Raspberry Pi 3

```
./configure CXXFLAGS="-O3 -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard" --with-boost-libdir=/usr/lib/arm-linux-gnueabihf
```

* Debug Build

```
./configure CXXFLAGS="-g"
```

And finally to build (replace <num_threads> with the number of logical cores your processor has)

```
make -j <num_threads>
```

### Installing

Once you've built the sources, you can install the game simply by typing

```
make install
```

### Command line usage

```
Usage: clanbomber2 [--software|direct3d|opengl|opengles|opengles2] [--fullscreen]
```

* --software, disables rendering hardware acceleration. Slower, but fixes some problems with unsupported hardware
* --direct3d, forces Direct3D as renderer
* --opengl, forces OpenGL as renderer
* --opengles, forces OpenGL ES 1.0 as renderer
* --opengles2, forces OpenGL ES 2.0 as renderer (recommended for Raspberry Pi)
* --fullscreen

## TODO / Nice to have

- [ ] Port to LibRetro (support for consoles)
- [ ] Add support for mobile platforms (iOS / Android)
- [ ] New or better AI, current doesn't feel human at all and in most cases it's impossible to win
- [ ] Add options to use old (ClanBomber 1) resources
- [ ] Add benchmarking options
- [ ] Update build system to CMake
- [ ] Re-add online (was removed due to problems with Raspbian)
- [ ] Optimize by using multiple threads
- [ ] Add options to enable / disable VSync (currently disabled)
- [ ] Remove dependency on Boost library
- [ ] Support more languages

## Contributing

Feel free to add issues or pull requests here on GitHub. I cannot guarantee that I will accept your changes, but feel free to fork the repo and make changes as you see fit. Thanks!
