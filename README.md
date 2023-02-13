# n64emu-SDL2GyroInjector

SDL2InputInjector is a rewritten version of the original Keyboard + Mouse Injector Plugin designed to take advantage of SDL2's motion control support.

The plugin is a practical reference implementation of ideas and concepts catalogued on [GyroWiki](http://gyrowiki.jibbsmart.com/). It can also be used for standard twin-stick input.

It is heavily based on the original Keyboard + Mouse injection code written by [Stolen and Carnivorous](http://www.shootersforever.com/forums_message_boards/viewtopic.php?t=7045). A majority of the credit goes to them for the original codebase & plugin concept.

Original codebase is maintained by Graslu00: https://github.com/Graslu/1964GEPD

## Download

This plugin is only compatible with the 1964GEPD 0.8.5 emulator. It will **not** run on any other emulator. 

The emulator can be obtained from Graslu's [release page](https://github.com/Graslu/1964GEPD/releases/tag/latest).
The latest release of the plugin can be found here: https://github.com/TauAkiou/n64emu-SDL2GyroInjector/releases

## Features:
* Full Controller Support for Goldeneye/Perfect Dark.
  * Mouse Style Gyro Aim supported on all controllers supported by SDL2.
* Extensive input customization.
* Support for up to 4 players on controllers.
* Implements Jibb Smart's [Flick Stick](http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick)
* Floating/Deattached Crosshair (Vertical, or fully unlocked modes. WIP.)

### Todo List:
- [x] ~~Rewrite input backend using SDL.~~
- [x] ~~Implement basic QT Plugin UI~~
- [x] ~~Implement Gyroscope Calibration~~
- [X] ~~Implement configuration saving~~
- [ ] Refactor & clean up controller code.
- [ ] Controller Player ID
- [ ] Code polishing & organization

## Dependencies & Requirements
- MSVC (Visual Studio 2017 Toolchain)
- cmake
- QT Framework 5.15.2 - qt5-static for Zilmar-spec builds ((obtainable from [here](https://github.com/gonetz/GLideN64/releases/tag/qt_build)))
- SDL2 (x86) - Obtainable from [The SDL2 Github](https://github.com/libsdl-org/SDL/releases)
- nlohmann/json (automatically downloaded by cmake)

## Building
This plugin is only confirmed to build properly on MSVC. Builds using MinGW and related toolchains are not supported.

*NOTE:* The supported version of 1964 is a 32-bit (x86) application. Ensure you configure your build environment appropriately and download the correct dependencies.

Obtain Dependencies
  * SDL2
    * Download and extract the latest SDL2 package from the above SDL2 Github Link.
    * Extract the package somewhere on your computer.
    * Copy ```cmake/SDL2Config.cmake``` from this repository into the folder you extracted SDL2. (Example: if you extracted it at ```C:\msvc\SDL2-2.0.16```, you copy SDL2Config.cmake into that directory.)
  * Qt5
    * Download and extract the qt5-static build from the above link.
      * Ensure you download ```qt-5_15-x86-msvc2017-static.7z```
    * Extract the package somewhere on your computer.

Building:

* Define the following cmake variables.
  * Qt5_DIR (Required)
    * Example: ```<Place you extracted qt5>\lib\cmake\Qt5```
  * SDL2_DIR (Required)
    * Example: ```<Place you extracted SDL2>\.```
  * 1964_INSTALL_DIRECTORY (Optional)
    * Set to the directory you extracted the emulator.
    * This variable will allow cmake to automatically copy the plugin to the emulator's directory.

  * Example build output:
    ```
    mkdir build
    cd build
    cmake .. -DQt5_DIR=<Place you extracted Qt5> -DSDL2_DIR=<Place you extracted SDL2>
    make
    ```
    
To run:
  * Copy ```"<Place you extracted SDL2">\lib\x86\SDL2.dll``` into the emulator's directory (where 1964.exe is located)
  * Copy the plugin (If you did not define 1964_INSTALL_DIRECTORY) to the emulator's plugins directory (1964/plugins)