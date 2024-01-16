//code for servo arm

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



#define PWM_TOP (39999u)
#define SERVO_MIN (1999u)
#define SERVO_MAX  (4999u)
#define MAX_DEGREE (180)
#define START (30)
#define END (90)

void pwm_init(void);
void int_init(void);
void servo_set_1(uint16_t deg);
void servo_set_2(uint16_t deg);

volatile uint8_t buttonWasPressed = 0;


int main(void)
{
	pwm_init();
	int_init();
	sei();
	
	servo_set_1(30); // controls servo that goes front and back
	_delay_ms(100);
	servo_set_2(40); // controls servo that goes up and down
	_delay_ms(100);
	
	int16_t i = 0;
	

	while(1)
	{
		if(buttonWasPressed == 1){
			while (buttonWasPressed == 1)
			{
			
				for(i = START ; i <= END; i++){
					servo_set_1(i);
					if ( i <60)
					{
						_delay_ms(20);
						continue;
					}
					servo_set_2((i-30));
					_delay_ms(20);
				}
				_delay_ms(3000);
				for(i = END ; i >= START; i--){
					servo_set_1(i);
					if ( i <60)
					{
						_delay_ms(20);
						continue;
					}
					servo_set_2((i-30));
					_delay_ms(20);
				}
				_delay_ms(3000);
				}
		}
		
	}
	
	

	
}

volatile static uint8_t update_pwm_ready = 0;

ISR(TIMER1_OVF_vect){
	
	update_pwm_ready = 0;
}

ISR(TIMER1_COMPA_vect){
	
}
ISR(INT0_vect){
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

ISR(TIMER1_COMPB_vect){
	
}


void int_init(void){
	DDRD &= ~(1<<2); // PIND 2 input for INT0
	PORTD |=  ( 1 << 2   );
	
	
	EICRA = (1<<ISC00)|(1<<ISC01);
	
	EIMSK = (1<<INT0);
	
}


void pwm_init(void){
	
	DDRB |= (1 << DDB1) | (1 << DDB2);
	
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
	
	TIMSK1 = (1 << TOIE1) | (1 << OCIE1B) | (1 << OCIE1A);
	
	//Because OCR1A is 16 bits, the first 8 bits correspond to H and the other 8 bits to L, we must
	//assign the left digits of servo_min to H by multiplied servo_min by 1111111100000000 and assigning it
	//to OCR1AH shifter left
	OCR1AH = (SERVO_MIN & 0xFF00) >> 8;
	OCR1AL = (SERVO_MIN & 0x00FF);
	
	OCR1BH = (SERVO_MIN & 0xFF00) >> 8;
	OCR1BL = (SERVO_MIN & 0x00FF);
	
	ICR1H = (PWM_TOP & 0xFF00) >> 8;
	ICR1L = (PWM_TOP & 0x00FF);
	
	
	
	
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
	
}

static void update_pwm(uint16_t i, uint16_t x){
	update_pwm_ready = 1;
	while(update_pwm_ready != 0);
	if (x == 1)
	{
		OCR1AH = (i & 0xFF00) >> 8;
		OCR1AL = (i & 0x00FF);
	}
	if (x == 2)
	{
		OCR1BH = (i & 0xFF00) >> 8;
		OCR1BL = (i & 0x00FF);
	}

}


void servo_set_1(uint16_t deg){
	
	float set = (float)deg / (float)MAX_DEGREE;
	
	set = (((float)SERVO_MAX-(float)SERVO_MIN)*set) + (float)SERVO_MIN;
	
	uint16_t piont = (uint16_t)set;
	
	update_pwm(piont, 1);
	
}

void servo_set_2(uint16_t deg){
	
	float set = (float)deg / (float)MAX_DEGREE;
	
	set = (((float)SERVO_MAX-(float)SERVO_MIN)*set) + (float)SERVO_MIN;
	
	uint16_t piont = (uint16_t)set;
	
	update_pwm(piont, 2);
	
}