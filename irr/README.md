IrrlichtMt version 1.9
======================

IrrlichtMt is the 3D engine of [Aperos Engine](https://github.com/VetusChronos/aperosengine).
It is based on the [Irrlicht Engine](https://irrlicht.sourceforge.io/) but is now developed independently.
It is intentionally not compatible to upstream and is planned to be eventually absorbed into AperosEngine.

Build
-----

The build system is CMake.

The following libraries are required to be installed:
* zlib, libPNG, libJPEG
* OpenGL
  * or on mobile: OpenGL ES (can be optionally enabled on desktop too)
* on Unix: X11
* SDL2 (see below)

Aside from standard search options (`ZLIB_INCLUDE_DIR`, `ZLIB_LIBRARY`, ...) the following options are available:
* `ENABLE_OPENGL` - Enable OpenGL driver
* `ENABLE_OPENGL3` (default: `OFF`) - Enable OpenGL 3+ driver
* `ENABLE_GLES2` - Enable OpenGL ES 2+ driver
* `USE_SDL2` (default: platform-dependent, usually `ON`) - Use SDL2 instead of older native device code

However, IrrlichtMt cannot be built or installed separately.

Platforms
---------

We aim to support these platforms:
* Windows via MinGW
* Linux (GL or GLES)
* macOS
* Android

This doesn't mean other platforms don't work or won't be supported, if you find something that doesn't work contributions are welcome.

License
-------

See [LICENSE](./LICENSE)
