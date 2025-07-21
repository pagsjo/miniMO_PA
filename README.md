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

Calibrate with a two-octave span using V/oct.

Requires _**disabling**_ **millis()/micros()**.

## New modules:
### ClockDivider
Takes an analog clock input and divides it by 2, 4, 8 or 16 (controllable by the knob).

Outputs the divided clock on I/O 1 & 2 (Pin 4).

#### I/O:
* 1 & 2: Output
* 3: Division (also knob)
* 4: Input