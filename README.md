# SDL2 Input Injector For Perfect Dark & Goldeneye

This is a modified/rewritten C++ version of the Keyboard/Mouse Injector Plugin written by Carnivorous.

This plugin works with the custom, overclocked version of the 1964 Emulator released by [Stolen and Carnivorous](http://www.shootersforever.com/forums_message_boards/viewtopic.php?t=7045)

This plugin is a rewrite heavily based on work done by Stolen and Carnivorous. A majority of the credit for this project goes to them and the ShootersForever community.

This plugin is still a work in progress.

## Features:
* Full Controller Support for Goldeneye/Perfect Dark.
  * Mouse Style Gyro Aim supported on all controllers supported by SDL2.
  * Joycons not currently supported.  

* Extensive input customization.
* Support for up to 4 players.
* Some features designed for gyroscope aiming:
    - Free Aim (Always-Active Aim Mode - experimental)
    - [Flick Stick](http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick)
  
### Todo List:
- [x] ~~Rewrite input backend using SDL.~~
- [x] ~~Implement basic QT Plugin UI~~
- [x] ~~Implement Gyroscope Calibration~~
- [X] ~~Implement configuration saving~~
- [ ] Controller Player ID
- [ ] Code polishing & organization
- [ ] Fix up `CMakeLists.txt` & learn how to better utilize cmake


## Potential Future Features
* Force Feedback (Potentially possible if we can poke the moments the game is supposed to have rumble.)
* Analog Movement (Also requires poking the game's player struct for info.)
* Keybord & Mouse Support
    - SDL2 natively supports keyboard and mouse input, so it makes sense to potentially include it. However, this version will not be able to supplant the original KB/M injector as SDL2 only supports the system KB/M. 

## Dependencies & Requirements
This plugin only works on Stolen/Carnivorous' GE/PD 1964 hack. Use the link above to find it.
- MSVC Compiler
- cmake
- QT Framework 5.15.2
- SDL2

Currently, you will have to configure ```cmakelist.txt``` and make sure your build environment is set up.

## Building
This plugin uses MSVC to build.

The emulator this plugin is designed to work with is 32-bit only, so you must download i686 versions of the compilers and libraries.

###Dependencies:
- msvc (Visual Studio 2017)
- GlideN64 qt5-static for Zilmar-spec builds (obtainable from [here](https://github.com/gonetz/GLideN64/releases/tag/qt_build))
- SDL2
- [nlohmann/json](https://github.com/nlohmann/json)
- cmake

## Releases
Releases are not yet being provided. You will have to build the plugin yourself.

## Bugs
Bugs are present in the issue tracker.