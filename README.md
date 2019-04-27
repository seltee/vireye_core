# VirEye is homebrew game console just for 15$. 
VirEye uses stm32f103c8t6 and ili9341 display. 
It can run up to 50 frames per second in 320x240 resolution and start games from SD card.
Core uses only 16 kb of rom memory, programs uses the rest 112 kb.
Write your games easy with usual c++ compiler (Clang from LLVM). 

# Software you need
* STM32 ST-Link utility - you will need it to upload firmware. Make sure, that your system path points to utility binaries
* LLVM and Clang to compile C++ into arm binaries
* Linker and Patcher from VirEye tools. You may found their executable binaries in the application repository. Linker builds .vex executable file from .o files you got from Clang. Patcher combines core hex code with your program to upload it to board through SWD.

# Schematics
May be found in "additional" folder.

# Repositories
##### Basic applications and examples with necessary tools for building and debugging: 
https://github.com/seltee/vireye_apps

##### Source code of linker and patcher (desktop applications, you may found their binaries in previous repository):
https://github.com/seltee/vireye_tools

# Showdown
######Coming soon

# Hardware parts:
* Blue pill based board running stm32f103c8t6
* Display with ILI9341 controller and SPI (note: some of this displays using 8080, but it's not compatible with this project)
* Analog stick with X and Y, like it was on PSP or some DIY cheap versions (some of them have pins for easy mount)
* Sound amplifier and speaker (Sound is mono)
* 6 buttons and bunch of wires
* 8 2 kOm resister and 8 1 kOm resister or 24 2 kOm resisters for sound DAC (see schematics)
* SD card slot if it is not mounted on display (most of such displays have card slot)
* Glue for mounting speaker
* STM32 programmer (ST-LINK V2, for example)
* A lot of patience