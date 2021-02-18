# SDL2 Input Injector For Perfect Dark & Goldeneye

This is a modified/rewritten C++ version of the Keyboard/Mouse Injector Plugin written by Carnivorous.

This plugin works with the custom, overclocked version of the 1964 Emulator released by Stolen and Carnivorous, which can be found here: [link](http://www.shootersforever.com/forums_message_boards/viewtopic.php?t=7045)

While this entire plugin was effectively rewritten, the code was primarily copied and readapted from Stolen and Carnivorous' work. A majority of the credit for this project goes to them and the ShootersForever community.

This plugin is still a work in progress, and is still not ready for general use. 

## Features:
* Full Controller Support for Goldeneye/Perfect Dark.
  * Mouse Style Gyro Aim supported on all controllers supported by SDL2.
  * Joycons not currently supported.  
* ~~Player Identification (Color bar for DS4, Player Lights for SPC/Joycon, both for DS5)~~ (Appears to be currently broken.)    
* Extensive input customization.
* Support for up to 4 players.
* Some Gyro-Exclusive Features:
    - Free Aim (Always-Active Aim Mode - experimental)
    - [Flick Stick](http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick)
  
### Todo List:
- [x] ~~Rewrite input backend using SDL.~~
- [x] ~~Implement basic QT Plugin UI~~
- [x] ~~Implement Gyroscope Calibration~~
- [ ] Implement configuration saving
- [ ] Code polishing & organization
- [ ] Polish up for prebuilt release.
- [ ] Fix up `CMakeLists.txt` & learn how to better utilize cmake
- [ ] Finish build instructions.


## Potential Future Features
* Force Feedback (Potentially possible if we can poke the moments the game is supposed to have rumble.)
* Analog Movement (Also requires poking the game's player struct for info.)
* Keybord & Mouse Support
    - SDL2 natively supports keyboard and mouse input, so it makes sense to potentially include it. However, this version will not be able to supplant the original KB/M injector as SDL2 only supports the system KB/M. 

## Dependencies & Requirements
This plugin only works on Stolen/Carnivorous' GE/PD 1964 hack. Use the link above to find it.
- MinGW64 (32-bit, x86)
- QT Framework 5.15.2
- SDL2 (Latest Development Snapshot)

Currently, you will have to configure ```cmakelist.txt``` and make sure your build environment is set up.

## Building
This assumes you already have mingw32, cmake, and the SDL headers somewhere on your system & your path set up. Build instructions should be similar to other cmake projects - detailed information coming later.

There are two cmake variables that must be set:
- QT_INSTALL (Points to your QT Install directory)
- SDL_INCLUDES (Points to the include directory for your SDL install)

## Releases
Releases are not yet being provided. You will have to build the plugin yourself.

## Bugs
Bugs are present in the issue tracker.