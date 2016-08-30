
# USB-RFID-Keyboard

This device behave like a USB Keyboard and a USB Serial port.
As USB Keyboard, when a programmed card is read:
- a formatted text, programmed in the RFID card, is written;
- the LEDs are lighted according to a programmed color.
As USB Serial port supported cards can be programmed using a terminal application.

### Software Development Environment
For software development I have used the [Atom](https://atom.io/) editor with [platformio-ide](https://atom.io/packages/platformio-ide) and [platformio-ide-terminal](https://atom.io/packages/platformio-ide-terminal) packages.

In _PlatformIO -> Initialize new PlatformIO Project or modify existing..._ I have selected the __Teensy LC__ board.

### Hardware
The hardware is built with a Teensy-LC, a MFRC522 card reader and some WS2812B LEDs.
USB-RFID-Keyboard was born from an exercise in using cheap RFID readers and cards so the first version was build using a breadboard and wires.
A schematic is in the work and will be published when ready.

### Dependencies

The project depends on two libraries : [MRFC522](https://github.com/miguelbalboa/rfid) and [FastLED](https://github.com/FastLED/FastLED).

#### Install dependencies - PlatformIO CLI version less then 3

Launch PlatformIO Library Manager from _PlatformIO_ -> _Library Manager CLI_, and type in the terminal window:
```
pio lib install 63
pio lib install 126
```
__Note:__ _in case you wonder where the numbers 63 and 126 came from, go to the_ [PlatformIO Libraries Registry](http://platformio.org/lib).

#### Install dependencies - PlatformIO CLI version at least 3

The libraries should be installed automatically when building the project _(NOT tested, based on official documentation)_.

### License

USB-RFID-Keyboard's software and documentation is released under the [GNU GPLv3](http://www.gnu.org/licenses/gpl-3.0.html) License. See the __LICENSE-GPLv3.txt__ file.

USB-RFID-Keyboard's hardware schematic is licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).
See the __LICENSE-CC-BY-SA-4.0.txt__ file.
