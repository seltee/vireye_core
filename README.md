### Vireye is homebrew game console, that uses stm32f103c8t6 and ili9341 display. It can run up to 50 frames per second in 320x240 resolution.

If you're already assembled this board and you don't want to work on the core, you don't need this repository. You need only vireye tools https://github.com/seltee/vireye_tools to compile your programs and run it through swd.

Use free version of keil uvision to build this project. Also by default it's configured to use st-link v2 (aliexpress version).

### Youtube videos (rus):

https://www.youtube.com/watch?v=qS5v5xbwclY

### Default wiring (check additional/scheme.jpg):

Display:

led - a1

dc - a2

reset - a3

cs - a4

clock - a5

miso - a6

mosi - a7

Buttons:

A - b6

B - b8

X - b7

Y - b9

Y Axis - b0

X Axis - b1
