
#include "DTH11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE 115200
#include "uart.h"

volatile int8_t temperature = 0;
volatile int8_t humidity = 0;
char printbuff[30];
char inputbuff[30];

ISR(TIMER1_COMPA_vect) // przerwanie s³u¿y do obs³ugi multipleksowania
{
	temperature=dht11_gettemperature();
	humidity = dht11_gethumidity();
	uart_puts("T:");
	itoa(temperature, printbuff, 10);
	uart_puts(printbuff);
	uart_puts("H:");
	itoa(humidity, printbuff, 10);
	uart_puts(printbuff); 
	uart_puts("\r\n");	
}

void PWM_init() //inciclalizacja timera0 pod mulitpleksowanie
{	
	 // set up timer with prescaler = 64 and CTC mode
	 TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);	 
	 // initialize counter
	 TCNT1 = 0;
	 // initialize compare value
	 OCR1A = 10800;	 
	 // enable compare interrupt
	 TIMSK |= (1 << OCIE1A);
}

void blinkDiode(unsigned int numDiode)
{
	PORTC |= (1 << numDiode); 
	_delay_ms(500);
	PORTC &= ~(1 << numDiode);
}

int main(void) 
{ 
	DDRC = 0x1F;
	PORTC = 0x00; 
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	PWM_init();
	sei();
	unsigned int loop, aa=0;	
	while(1) 
	{    
		loop = uart_available();	
		if(loop > 2)
		{						
			for(int i = 0; i <= loop; i++)
			{
				if(i < 29)	
				{			
					aa = uart_getc();					
					inputbuff[i]= aa & 0xFF;					
				}								    	
			}		
			uart_flush();	
			if(inputbuff[0]== 'D' && inputbuff[1]== ':')
				blinkDiode(inputbuff[2]-0x30); //ascii to dec									
		}		
	} 
	return 0; 
}
