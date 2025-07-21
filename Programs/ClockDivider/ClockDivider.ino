#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

// pins
const int BTN_PIN = 1;
const int CLK_IN = 2;
const int CTRL_IN = 3;

const int LED_PIN = 0;
const int CLK_OUT = 4;

// variables
int buttonState = LOW;
int clkInState = LOW;
int lastInState = LOW;
int clkOutState = LOW;

// counters
int pulseCount = 0;

void setup()
{
	PRR = (1 << PRUSI); // Disable USI

	pinMode(LED_PIN, OUTPUT);
	pinMode(CLK_OUT, OUTPUT);
	pinMode(CTRL_IN, INPUT);
	pinMode(CLK_IN, INPUT);
	pinMode(BTN_PIN, INPUT);

	checkVoltage();
	ADMUX = 0; // Reset multiplexer

	cli();

	GIMSK |= (1 << PCIE);					// Pin Change Interrupt Enable
	PCMSK |= (1 << PCINT1) | (1 << PCINT2); // Pin 1 & 2

	sei();

	digitalWrite(LED_PIN, HIGH);
}

ISR(PCINT0_vect) // Pin Interrupt
{
	buttonState = PINB & 0x2; // Read Pin 1
	clkInState = PINB & 0x4;  // Read Pin 2
}

void loop()
{
	int ctrlRead = analogRead(CTRL_IN);
	int divtableOffset = map(ctrlRead, 0, 1023, 0, 4);
	const int divtable[5] = {
		0, // OFF
		1, // Divide by 2
		2, // Divide by 4
		4, // Divide by 8
		8, // Divide by 16
	};
	int divValue = pgm_read_byte_near(divtable + divtableOffset);

	if (divValue != 0)
	{
		if (clkInState != lastInState)
		{
			if (clkInState == 1)
			{
				pulseCount += 1;
				lastInState = clkInState;
			}
		}
		if (pulseCount == divValue)
		{
			clkOutState = !clkOutState;
			digitalWrite(CLK_OUT, clkOutState);
			digitalWrite(LED_PIN, clkOutState);
			pulseCount = 0;
		}
	}
	else
	{
		digitalWrite(CLK_OUT, CLK_IN);
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