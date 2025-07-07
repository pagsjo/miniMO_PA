/*
//**************************
//*   miniMO CV Splitter   *
//*     2025 by pagsjo     *
//*  based on miniMO code  *
//*      by enveloop       *
//**************************
//
	http://www.minimosynth.com
	CC BY 4.0
	Licensed under a Creative Commons Attribution 4.0 International license: 
	http://creativecommons.org/licenses/by/4.0/
//

I/O
	1&2: Outputs - CV out
	3: N/A
	4: Input - CV in

OPERATION
	Plug CV into I/O 4. This CV is duplicated and sent out on I/O 1 & 2.

BATTERY CHECK
	When you switch the module ON,
		-If the LED blinks once, the battery is OK
		-If the LED blinks fast several times, the battery is running low
*/

#include <avr/io.h>
#include <util/delay.h>

//output
int CV;

//button interrupt
volatile bool inputButtonValue;

void setup() {
	//disable USI to save power as we are not using it
	PRR = 1<<PRUSI;

	//set LED pin and check the battery level
	pinMode(0, OUTPUT); //LED
	checkVoltage();

	ADCSRA = (1 << ADEN);	//reset ADC Control (ADC Enable 1, everything else 0)
	ADCSRA |= (1 << ADPS2);	//set adc prescaler  to 16 for 500kHz sampling frequency (8 also works well but is noisier). 500/13 cycles per  sample = 38.4 Khz

	ADMUX = 0;							//reset multiplexer settings
	ADMUX |= (1<<ADLAR);				//left-adjust result (8 bit conversion)
	ADMUX |= (1 << MUX1) | (1 << MUX0);	//control input
	ADCSRA |=  (1<<ADSC);				// start next conversion

	pinMode(1, INPUT);	//digital input (push button)
	pinMode(3, INPUT);	//N/A
	pinMode(2, INPUT);	//analog- CV in (I/O 4)
	pinMode(4, OUTPUT);	//output

	//disable digital input on pins that do analog conversion
	DIDR0 = (1 << ADC2D) | (1 << ADC3D);	//PB3, PB4

	//set clock source for PWM -datasheet p94
	PLLCSR |= (1 << PLLE); 				// Enable PLL (64 MHz)
	_delay_us(100);						// Wait for a steady state
	while (!(PLLCSR & (1 << PLOCK)));	// Ensure PLL lock
	PLLCSR |= (1 << PCKE);				// Enable PLL as clock source for timer 1

	cli();	// Interrupts OFF (disable interrupts globally)

	//PWM Generation -timer 1
	GTCCR  = (1 << PWM1B) | (1 << COM1B1);	// PWM, output on pb4, compare with OCR1B (see interrupt below), reset on match with OCR1C
	OCR1C  = 0xff;							// 255
	TCCR1  = (1 << CS10);					// no prescale

	//Timer Interrupt Generation -timer 0
	TCCR0A = (1 << WGM01);	// Clear Timer on Compare (CTC) with OCR0A
	TCCR0B = (1 << CS01);	// prescaled by 8
	OCR0A = 0x64;			// 0x64 = 100 //10000hz - 10000 ticks per second https://www.easycalculation.com/engineering/electrical/avr-timer-calculator.php
	TIMSK = (1 << OCIE0A);	// Enable Interrupt on compare with OCR0A

	//Pin Interrupt Generation
	GIMSK |= (1 << PCIE);	// Enable 
	PCMSK |= (1 << PCINT1);	// on pin 1

	sei();	// Interrupts ON (enable interrupts globally)

	digitalWrite(0, HIGH);	//turn LED on
}

//PIN Interrupt
ISR(PCINT0_vect) {					//PIN Interruption - has priority over Timer 0; this ensures that the switch will work
	inputButtonValue = PINB & 0x02;	//Reads button (digital input1, the second bit in register PINB. We check the value with & binary 10, so 0x02) 
}

//Timer0 interrupt
ISR(TIMER0_COMPA_vect) {	//10000 ticks per second
	CV;
}

// void loop() {
// 	int tempRead = analogRead(2);
// 	CV = map(tempRead, 0, 1023, 0, 1023);
// }

void loop() {
	CV = analogRead(2);
}

void checkVoltage() {						//voltage from 255 to 0; 46 is (approx)5v, 94 is 2.8, 104-106 is 2.5
											//we measure a fixed value of 1.1 against Vcc, so the lower the measurement, the higher Vcc
	ADMUX = (0 << REFS1) | (0 << REFS0);	//Vcc as reference
	ADMUX |= (1 << ADLAR);					//Left adjust result (8 bit conversion stored in ADCH)
	ADMUX |= (1 << MUX3) | (1 << MUX2);		//1.1v input
	delay(250);								// Wait for Vref to settle
	ADCSRA |= (1 << ADSC);					// Start conversion
	while (bit_is_set(ADCSRA, ADSC));		// wait while measuring
	if (ADCH > 103)							//approx 2.6
		flashLED(8, 100);
	else
		flashLED(1, 250);
}

void flashLEDSlow(int times) {
	for (int i = 0; i < times; i++){
		_delay_ms(70);
		digitalWrite(0, LOW);
		_delay_ms(70);
		digitalWrite(0, HIGH);
	}
}

void flashLED (int times, int gap) {	//for voltage check only (uses regular delay)
	for (int i = 0; i < times; i++)
	{
		digitalWrite(0, HIGH);
		delay(gap);
		digitalWrite(0, LOW);
		delay(gap);
	}
}