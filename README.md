# SDL2 Input Injector For Perfect Dark & Goldeneye

This is a modified/rewritten C++ version of the Keyboard/Mouse Injector Plugin written by Carnivorous.

This plugin works with the custom, overclocked version of the 1964 Emulator released by [Stolen and Carnivorous](http://www.shootersforever.com/forums_message_boards/viewtopic.php?t=7045)

This plugin is a rewrite heavily based on work done by Stolen and Carnivorous. A majority of the credit for this project goes to them and the ShootersForever community.

This plugin is beta software and is very likely to have bugs or other issues. Please report bugs in the repository issue tracker.

## Features:
* Full Controller Support for Goldeneye/Perfect Dark.
  * Mouse Style Gyro Aim supported on all controllers supported by SDL2.
  * Joycons not currently implemented. 

* Extensive input customization.
* Support for up to 4 players.
* Some features designed for gyroscope aiming:
    - Unlocked Aimer Modes (Free Aim, Splatoon (planned))
    - [Flick Stick](http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick)
  
### Todo List:
- [x] ~~Rewrite input backend using SDL.~~
- [x] ~~Implement basic QT Plugin UI~~
- [x] ~~Implement Gyroscope Calibration~~
- [X] ~~Implement configuration saving~~
- [ ] Controller Player ID
- [ ] Code polishing & organization


## Potential Future Features
* Force Feedback (Potentially possible if we can poke the moments the game is supposed to have rumble.)
* Analog Movement (Also requires poking the game's player struct for info.)
* Keybord & Mouse Support
    - SDL2 natively supports keyboard and mouse input, so it makes sense to potentially include it. However, this version will not be able to supplant the original KB/M injector as SDL2 only supports the system KB/M. 

## Dependencies & Requirements
This plugin only works on Stolen/Carnivorous' GE/PD 1964 hack. Use the link above to find it.
- MSVC (Visual Studio 2017 Toolchain)
- cmake
- QT Framework 5.15.2
- SDL2


## Building
This plugin builds using the Visual Studio 2017 toolchain.

The emulator used by this plugin is a 32-bit program. Ensure you have the 32-bit (x86 or i686) versions of all dependencies.

##Dependencies:
- msvc (Visual Studio 2017)
- GlideN64 qt5-static for Zilmar-spec builds (x86) (obtainable from [here](https://github.com/gonetz/GLideN64/releases/tag/qt_build))
- SDL2 (x86)
- [nlohmann/json](https://github.com/nlohmann/json) (automatically downloaded by cmake)
- cmake

##Building

Obtaining SDL2:
  * Download and extract the latest SDL2 package from [The SDL2 Website](https://www.libsdl.org/download-2.0.php)
    * Download the Visual C++ Development Libraries.
  * Extract the package somewhere on your computer.
  * Copy ```cmake/SDL2Config.cmake``` supplied into the SDL2 directory (Example: if you extracted it at ```C:\msvc\SDL2-2.0.16```, you copy SDL2Config.cmake into that directory.)

Obtaining Qt5:
  * Download and extract the qt5-static build from the above link.
    * Ensure you download ```qt-5_15-x86-msvc2017-static.7z```
  * Extract the package somewhere on your computer.

Building:

* The following variables must be supplied. 
  * Qt5_DIR
    * Example: ```<Place you extracted qt5>\lib\cmake\Qt5```
  * SDL2_DIR
    * Example: ```<Place you extracted SDL2>\.```

* You can also supply an optional ```1964_INSTALL_DIRECTORY```
  * This will be set to the directory you installed the emulator (containing 1964.exe)
  * Setting this variable will automatically copy the plugin to the 1964 directory when built.

  * When set:
    ```
    mkdir build
    cd build
    cmake .. -DQt5_DIR=<Place you extracted Qt5> -DSDL2_DIR=<Place you extracted SDL2>
    make
    ```
    
To run:
  * Copy ```"<Place you extracted SDL2">\lib\x86\SDL2.dll``` into the emulator's directory (where 1964.exe is located)
  * Copy the plugin (If you did not define 1964_INSTALL_DIRECTORY) to the emulator's plugins directory (1964/plugins)

## Releases
Releases are not yet being provided. You will have to build the plugin yourself.