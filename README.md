# miniMO_PA
Programmable mini Synthesizer Module </br>

https://minimosynth.com/

Edited by pagsjo

Compiles using Arduino IDE 2.3.6 and [ATTinyCore by Spence Konde](https://github.com/SpenceKonde/ATTinyCore) with the following settings:

* **Board:** ATtiny25/45/85 (No bootloader)
* **B.O.D. Level:** Disabled
* **Chip:** ATtiny85
* **Clock Source:** 8 MHz (internal)
* **Save EEPROM:** EEPROM retained
* **LTO:** Enabled
* **millis()/micros():** Enabled
* **Timer 1 Clock:** CPU (CPU frequency)

## Known Issues
### millis()/micros()
The following programs requires **disabling** **millis()/micros()**:
* Algorithmic Generator
* AM Generator
* DCO
* Delay
* LPF
* LunaMod Generator
* Phaser

## Edited modules:
### DCO
Edited to have a more usable frequency range (C0-C7, derived from the Arturia Minibrute 2's MIDI range)

Currently all three ranges are the same.

Requires **disabling** **millis()/micros()**.