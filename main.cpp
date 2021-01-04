#define F_CPU 16000000

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

void initSleep();
void initTimer1();
void initPwm();
void motorPwmAndDirection(int, int);
void sleepMode();

ISR(TIMER1_OVF_vect)
{
	//blink LED
	DDRB |= (1<<PORTB5);
	PORTB ^= (1<<PORTB5);
	TCNT1 = 49916;
}

ISR(WDT_vect)
{}

int main()
{
	initTimer1();
	//initSleep();
	initPwm();

	while(1)
	{
		motorPwmAndDirection(1,75);
		//sleepMode();
	}
}

void initSleep()
{
	//watchdog timer
	WDTCSR = (0b00011000); //sets watchdog system reset enable and watchdog change enable
	WDTCSR = (0b00100001); //sets prescaler to 8s
	WDTCSR |= (1<<WDIE);// sets watchdog interrupt enable

	//sleep
	SMCR |= (1<<SM1); //sets
	SMCR |= (1<<SE); //sets
}

void initTimer1()
{
	//init timer 1
	TCNT1 = 49916; //sets initial timer value
	TCCR1A = 0x00; //timer normal mode
	TCCR1B = (1<<CS10) | (1<<CS12); //sets prescaler clk/1024
	TIMSK1 = (1<<TOIE1);// enable overflow interrupt
	sei(); // interrupts on
}

void initPwm()
{
	// Init Timer 0
	TCCR0A = 0;
	TCCR0B = 0;
	// FastPWM 8-bit mode
	TCCR0A |=  (1<<WGM01) | (1<<WGM00); // set OC0A/OC0B on compare match, clear at BOTTOM
	TCCR0A |= (1<<COM0A1); // PD6
	TCCR0A |= (1<<COM0B1); // PD5
	
	TCCR0B |= (1<<CS02); // prescaler 1024
	
	DDRD |= (1<<PORTD6) | (1<<PORTD5); // set OC0A & OC0B to output
}

// direction: 1=forward 2=reverse; pwmValue: 0-255 motor speed
void motorPwmAndDirection(int direction, int pwmValue)
{
	if (direction == 1)
	{
		OCR0A = 0;
		OCR0B = pwmValue;
	}
	if (direction == 2)
	{
		OCR0A = pwmValue;
		OCR0B = 0;
	}
}

void sleepMode()
{
	//-------SLEEP MODE------------
	for(int i=0;i<4;i++) // sleep for 32 sec/ 8 secs per i
	{
	//BOD disable
	MCUCR |= (3<<BODSE); //sets both BODS and BODSE
	MCUCR &= ~(1<<BODSE); //clear BODSE pinS
	//sleep command
	__asm__ __volatile__("sleep");
	}
}