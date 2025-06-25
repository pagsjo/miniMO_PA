# miniMO_PA
Programmable mini Synthesizer Module

https://minimosynth.com/

[Made by enveloop](https://github.com/enveloop/miniMO)<br>
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
### DCO and external CV
Pitch behaves strangely when using external CV.

### millis()/micros()
The following programs requires _**disabling**_ **millis()/micros()**:
* Algorithmic Generator
* AM Generator
* DCO
* Delay
* LPF
* LunaMod Generator
* Phaser

## Edited modules:
### DCO
Edited to be more playable from external CV.
* **Range 1**: C1-C3 (~33Hz-131Hz)
* **Range 2**: C3-C5 (~131Hz-523Hz)
* **Range 3**: C5-C7 (~523Hz-2093Hz)

See [Known Issues](#dco-and-external-cv)

Requires _**disabling**_ **millis()/micros()**.