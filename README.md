# Multi State Automata
![Image of an ESP32 running the multi-state automata simulation](https://github.com/davidjihwan/Multi_State_Automata/blob/main/Media/GOL_IMG2.HEIC)

## Summary
This is an implementation of a multi-state automata for the LILYGO ESP32 T-Display. There are three states in this implementation. Each state has a unique color, as well as state that it is weak against. The world consists of cells that correspond to a state. If a cell is surrounded by x (where x is some set threshold) or more cells that it is weak against, it will take on that state. 

The code is written in the Arduino IDE, so other Arduino-compatible devices may be able to run the code with some basic modifications. Many thanks to [Erik Fransson](https://www.youtube.com/watch?v=TvZI6Xc0J1Y&t=76s) for inspiration. I also go into more detail about the decision decisions behind the project on [my website](TODO).

## Media
The final product looks something like this: 
![Gif of an ESP32 running the multi-state automata simulation](Media/GOL_GIF1)

![Another gif of an ESP32 running the multi-state automata simulation](Media/GOL_GIF2)

## Materials
- [Arduino IDE](https://www.arduino.cc/en/software)
- [LILYGO ESP32 T-Display](https://www.lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board?srsltid=AfmBOordft8S6UaQt1WnySrXgbTSEONV1JsxoCGzCaYUcaJFkpL_2dnz)
- USB-C cable

## Quick Start Guide
1. Download the [Arduino IDE](https://www.arduino.cc/en/software) on your computer of choice.
2. Install the "TFT_eSPI" library by Bodmer and the "Adafruit GFX" library by Adafruit in the Arduino IDE using the library manager (Tools/Manage Libraries).
3. Open the file Arduino/libraries/TFT_eSPI/User_Setup_Select.h
4. Comment out the line #include <User_setup.h> and uncomment out the line include <User_Setups/Setup25_TTGO_T_Display.h> 
5. Install the relevant [ESP32 Drivers](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) (CH9102).
6. Download the project file [here](Arduino/CES_Module_1.ino).
7. Open the project file.
8. Change your board: Tools > Boards > esp32 > ESP32 Dev Module
9. Change the port: Tools > Port > your-port (mac users should select wchusb)
10. Plug the ESP32 into your computer.
11. Press the upload button on the top left of the screen to upload your code to the ESP32.
TODO: image
12. That's it! The code should now be running on the ESP32 and keep simulating the automata as long as it can draw power from your computer.

Credits: Thanks to Prof. Tiffany Tseng for the guide on ESP32 setup!

## Additional Resources
https://gitlab.com/erikfransson/game_of_life
https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life


