//Code for DC Motor


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define PWM1H OCR1AH
#define PWM1L OCR1AL
#define PWM2H OCR1BH
#define PWM2L OCR1BL
#define SPEED (170)

void int_init(void);
void InitPWM(void);
void clockwiseMotorMove(void);
void antiClockwiseMotorMove(void);
void moveClockwiseAfterAnticlockwise(void);


volatile uint8_t  buttonWasPressed = 0;
static uint8_t start=0;

int main(void)
{
	int_init();
	InitPWM();
	sei();

	while (1)
	{
		if(buttonWasPressed == 1){
					if (start == 0)
					{
					clockwiseMotorMove();
					start = 1;
					}
					antiClockwiseMotorMove();
					moveClockwiseAfterAnticlockwise();
		}
	}

}

void int_init(void){
	DDRD &= ~(1<<2); // PIND 2 input for INT0
	PORTD |=  ( 1 << 2 );
	
	
	EICRA = (1<<ISC00)|(1<<ISC01);
	
	EIMSK = (1<<INT0);
	
}



void InitPWM(void){
	//motor
	PORTB = 0x00;

	DDRB = (1<< 2) | (1 << 1); //PORTB 1 and 2 for the motor

	TCCR1A = 0XA3;   //0b01110101

	TCCR1B = 0X04;                //0b00000101;

	TCNT1H=0x00;    TCNT1L = 0x00;
	ICR1H = 0x00;    ICR1L = 0x00;
	OCR1AH = 0x00;    OCR1AL= 0x00;
	OCR1BH = 0x00;    OCR1BL = 0x00;

}



ISR(INT0_vect){ 
	//button interrupt
	if ( buttonWasPressed == 0 ) //if button hadn't been pressed before
	{
		// update button state
		buttonWasPressed = 1;
	}
	else // The button is not pressed
	{
		buttonWasPressed = 0;       /// Update the button state
	}
}

void clockwiseMotorMove(void){ //clockwise around 4s wait; the servo arm goes forward, does this 30 times (there are 30 pins on loom)
	PWM2H = 0;    PWM2L = 0;  //PWM2H OCR1BH; PWM2L OCR1BL
	PWM1H = (SPEED>>8);		 //decrease value of (170) to slow it down, increase value to make it faster 
	PWM1L = (SPEED & 0xFF);	//decrease value of (170) to slow it down, increase value to make it faster 
	_delay_ms(750);			//how much time the DC motor is moving forward
PWM1H = 0;    PWM1L = 0;        //}While motor is off
PWM2H = 0;    PWM2L = 0;        //}
_delay_ms(4000);			//this should wait 4 seconds for the servo to go forward
}

void antiClockwiseMotorMove(void){  //clockwise around 4s wait; the servo arm goes back, does this 30 times (there are 30 pins on loom)
	PWM1H = 0;    PWM1L = 0;	//PWM1H OCR1AH; PWM1L OCR1AL
	PWM2H = (SPEED>>8);			//decrease value of (170) to slow it down, increase value to make it faster 
	PWM2L = (SPEED & 0xFF);		//decrease value of (170) to slow it down, increase value to make it faster 
	_delay_ms(650);				//how much time the DC motor is moving forward 
PWM1H = 0;    PWM1L = 0;        //}While motor is off
PWM2H = 0;    PWM2L = 0;        //}
_delay_ms(4000);				//this should wait 4 seconds for the servo to go backwards
}

void moveClockwiseAfterAnticlockwise(void){ //clockwise around 6s wait; the servo arm goes forward, does this 30 times (there are 30 pins on loom)
	PWM2H = 0;    PWM2L = 0; //PWM2H OCR1BH; PWM2L OCR1BL
	PWM1H = (SPEED>>8);		//decrease value of (170) to slow it down, increase value to make it faster
	PWM1L = (SPEED & 0xFF);	//decrease value of (170) to slow it down, increase value to make it faster 
	_delay_ms(1200);			//how much time the DC motor is moving forward to the next pin on loom
PWM1H = 0;    PWM1L = 0;        //}While motor is off
PWM2H = 0;    PWM2L = 0;        //}
_delay_ms(6000);			//this should wait 6 seconds for the next servo movement 
}