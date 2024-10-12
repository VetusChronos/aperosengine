Aperos (Voxel) Engine
=====================

[![License](https://img.shields.io/badge/license-LGPLv2.1%2B-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html)

Aperos Engine is a free open source voxel game engine focusing on ease of modding and game creation.
With the aim of bringing the AperosVoxel game to life.

Aperos Engine is based on [Minetest](https://minetest.net):

> Credits to (C) 2010-2018 Perttu Ahola <celeron55@gmail.com> for creating Minetest
> and Minetest collaborators for their contributions (see the LICENSE file). 
> Thanks to Minetest contributors!

Locales
-------
* [Português Brasil](docs/locales/pt_BR/)

Table of Contents
-----------------
1. [Further Documentation](#further-documentation)
2. [Default Controls](#default-controls)
3. [Paths](#paths)
4. [Configuration File](#configuration-file)
5. [Command-line Options](#command-line-options)
6. [Compiling](#compiling)
7. [Docker](#docker)
8. [Version Scheme](#version-scheme)


Further documentation
---------------------
- GitHub: https://github.com/VetusChronos/aperosengine/
- [Developer documentation](docs/locales/en/developing/)
- [docs/](docs/locales/en/) directory of source distribution

Default controls
----------------
All controls are re-bindable using settings.
Some can be changed in the key config dialog in the settings tab.

| Button                        | Action                                                         |
|-------------------------------|----------------------------------------------------------------|
| Move mouse                    | Look around                                                    |
| W, A, S, D                    | Move                                                           |
| Space                         | Jump/move up                                                   |
| Shift                         | Sneak/move down                                                |
| Q                             | Drop itemstack                                                 |
| Shift + Q                     | Drop single item                                               |
| Left mouse button             | Dig/punch/use                                                  |
| Right mouse button            | Place/use                                                      |
| Shift + right mouse button    | Build (without using)                                          |
| E                             | Inventory menu                                                 |
| Mouse wheel                   | Select item                                                    |
| 0-9                           | Select item                                                    |
| Z                             | Zoom (needs zoom privilege)                                    |
| T                             | Chat                                                           |
| /                             | Command                                                        |
| Esc                           | Pause menu/abort/exit (pauses only singleplayer game)          |
| +                             | Increase view range                                            |
| -                             | Decrease view range                                            |
| K                             | Enable/disable fly mode (needs fly privilege)                  |
| J                             | Enable/disable fast mode (needs fast privilege)                |
| H                             | Enable/disable noclip mode (needs noclip privilege)            |
| Ctrl                          | Aux1 (Move fast in fast mode. Games may add special features)  |
| C                             | Cycle through camera modes                                     |
| V                             | Cycle through minimap modes                                    |
| Shift + V                     | Change minimap orientation                                     |
| F1                            | Hide/show HUD                                                  |
| F12                           | Hide/show chat                                                 |
| F7                            | Disable/enable fog                                             |
| F4                            | Disable/enable camera update (Mapblocks are not updated anymore when disabled, disabled in release builds)  |
| F3                            | Cycle through debug information screens                        |
| F6                            | Cycle through profiler info screens                            |
| F10                           | Show/hide console                                              |
| F2                            | Take screenshot                                                |

Paths
-----
Locations:

* `bin`   - Compiled binaries
* `share` - Distributed read-only data
* `user`  - User-created modifiable data

Where each location is on each platform:

* Windows .zip / RUN_IN_PLACE source:
    * `bin`   = `bin`
    * `share` = `.`
    * `user`  = `.`
* Windows installed:
    * `bin`   = `C:\Program Files\AperosEngine\bin (Depends on the install location)`
    * `share` = `C:\Program Files\AperosEngine (Depends on the install location)`
    * `user`  = `%APPDATA%\AperosEngine` or `%APEROSENGINE_USER_PATH%`
* Linux installed:
    * `bin`   = `/usr/bin`
    * `share` = `/usr/share/aperosengine`
    * `user`  = `~/.aperosengine` or `$APEROSENGINE_USER_PATH`
* macOS:
    * `bin`   = `Contents/MacOS`
    * `share` = `Contents/Resources`
    * `user`  = `Contents/User` or `~/Library/Application Support/aperosengine` or `$APEROSENGINE_USER_PATH`

Worlds can be found as separate folders in: `user/worlds/`

Configuration file
------------------
- Default location:
    `user/aperosengine.conf`
- This file is created by closing AperosEngine for the first time.
- A specific file can be specified on the command line:
    `--config <path-to-file>`
- A run-in-place build will look for the configuration file in
    `location_of_exe/../aperosengine.conf` and also `location_of_exe/../../aperosengine.conf`

Command-line options
--------------------
- Use `--help`

Compiling
---------

- [Compiling on GNU/Linux](docs/locales/en/compiling/linux.md)
- [Compiling on Windows](docs/locales/en/compiling/windows.md)
- [Compiling on MacOS](docs/locales/en/compiling/macos.md)

Docker
------

- [Developing aperosengineserver with Docker](docs/locales/en/developing/docker.md)
- [Running a server with Docker](docs/locales/en/docker_server.md)

Version scheme
--------------
Aperos Engine use the scheme `major.minor.patch.stage`.

- Major is incremented when the release contains breaking changes, all other
numbers are set to 0.
- Minor is incremented when the release contains new non-breaking features,
patch is set to 0.
- Patch is incremented when the release only contains bugfixes and very
minor/trivial features considered necessary.
- Stage is incremented when the indicate whether the engine has been released, 
is in beta, alpha, or in dev

**Stages**:

- **Release**: Indicates that the engine has been released
- **Beta**: Indicates that the engie is in beta phase, and that new feature additions are paused and current features will be polished
- **Alpha**: Indicates that the engine is in the alpha phase, and that new feature additions are being implemented, removed or being tested, which may result in instability
- **Dev**: Indicates that the engine has not been released for official testing
