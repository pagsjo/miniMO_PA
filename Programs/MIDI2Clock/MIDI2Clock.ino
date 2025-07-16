#include <SoftwareSerialminiMO.h>
#include <MIDI.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

SoftwareSerial midiSerial(2, -1);
MIDI_CREATE_INSTANCE(SoftwareSerial, midiSerial, MIDI)

// button interrupt
volatile bool inputButtonValue;

// timer interrupt
volatile unsigned int globalTicks = 0;

// button press control
int button_delay;
int button_delay_b;
bool beenDoubleClicked = false;
bool beenLongPressed = false;
byte additionalClicks = 0;

// MIDI
byte midiClock = 0xf8;
int pulseCount = 0;

void setup()
{
	PRR = (1 << PRUSI);					 // Disable USI
	DIDR0 = (1 << ADC1D) | (1 << ADC3D); // Disable digital input on PB2, PB3

	pinMode(0, OUTPUT); // LED
	pinMode(4, OUTPUT); // Clock output
	pinMode(3, INPUT);	// Clock division input
	pinMode(2, OUTPUT); // Unused
	pinMode(1, INPUT);	// Digital input (button)

	checkVoltage();
	ADMUX = 0; // Reset multiplexer

	cli(); // Interrupts OFF

	// Timer Interrupt Generation -timer 0
	TCCR0A = (1 << WGM01); // Clear Timer on Compare (CTC) with OCR0A
	TCCR0B = (1 << CS02);  // prescale by 256
	OCR0A = 0xfa;		   // 0xfa = 250 //125hz https://www.easycalculation.com/engineering/electrical/avr-timer-calculator.php
	TIMSK = (1 << OCIE0A); // Enable Interrupt on compare with OCR0A

	// initSteps(500); // initialize step information

	sei();

	MIDI.begin();
}

ISR(PCINT0_vect) // PIN Interruption - has priority over Timer 0; this ensures that the switch will work
{
	inputButtonValue = PINB & 0x02; // Reads button (digital input1, the second bit in register PINB. We check the value with & binary 10, so 0x02)
}

ISR(TIMER0_COMPA_vect) // Timer0 interruption
{
	globalTicks++;
}

void loop()
{
	midi2clock();
}

void midi2clock()
{
	if (MIDI.read() == midiClock)
	{
		pulseCount += 1;
		if (pulseCount >= 4)
		{
			digitalWrite(4, HIGH);
			pulseCount = 0;
		}
	}
}

void checkVoltage() // voltage from 255 to 0; 46 is (approx)5v, 94 is 2.8, 104-106 is 2.5
{
	// we measure a fixed value of 1.1 against Vcc, so the lower the measurement, the higher Vcc
	ADMUX = (0 << REFS1) | (0 << REFS0); // Vcc as reference
	ADMUX |= (1 << ADLAR);				 // Left adjust result (8 bit conversion stored in ADCH)
	ADMUX |= (1 << MUX3) | (1 << MUX2);	 // 1.1v input
	delay(250);							 // Wait for Vref to settle
	ADCSRA |= (1 << ADSC);				 // Start conversion
	while (bit_is_set(ADCSRA, ADSC))
		;			// wait while measuring
	if (ADCH > 103) // aprox 2.6
		flashLED(8, 100);
	else
		flashLED(1, 250);
}

void flashLED(int times, int gap) // for voltage check (uses regular delay)
{
	for (int i = 0; i < times; i++)
	{
		digitalWrite(0, HIGH);
		delay(gap);
		digitalWrite(0, LOW);
		delay(gap);
	}
}