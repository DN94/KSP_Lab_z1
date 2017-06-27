
#include "DTH11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE 115200
//#define UART_BAUD_RATE 500000
#include "uart.h"

volatile int8_t temperature = 0;
volatile char loop = 20;
//volatile char sendEnable = 0;
char printbuff[30];
char inputbuff[30];

ISR(TIMER1_COMPA_vect) // przerwanie s³u¿y do obs³ugi wysy³ania danych co sekundê
{
	if(loop >= 10)
	{
		temperature=dht11_gettemperature();
		loop = 0;
	}
	loop++;
	uart_puts("T:");
	itoa(temperature, printbuff, 10);
	uart_puts(printbuff);
	uart_puts("\r\n");
	//sendEnable = 0;
}

void CTC1_init() //inciclalizacja timera1 pod wysy³anie 
{	
	// set up timer with prescaler = 1024 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value
	OCR1A = 10800; // 16MHZ to 110520 -> 10800;		
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
}

int main(void) 
{ 
	DDRC = 0x1F;
	PORTC = 0x1F;
	_delay_ms(1000);
	PORTC = 0x00;
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	CTC1_init();	
	sei();
	unsigned int receivedData, aa=0;	
	char out = 0;	
	while(1) 
	{    
		receivedData = uart_available();	
		if(receivedData >= 6)
		{	
			out = 0;									
			for(int i = 0; i < 6; i++)
			{					
				aa = uart_getc();					
				inputbuff[i]= aa & 0xFF;
				if(inputbuff[0]== 'D' && i !=0)
				{
					out +=  (inputbuff[i]-0x30)<<(i-1);
				}											    	
			}		
			uart_flush();
			PORTC = out;
			_delay_ms(50);
			PORTC = 0x00;																
		}
		/*if(sendEnable == 1)
		{
			if(loop >= 10)
			{
				temperature=dht11_gettemperature();
				loop = 0;
			}
			loop++;
			uart_puts("T:");
			itoa(temperature, printbuff, 10);
			uart_puts(printbuff);
			uart_puts("\r\n");
			sendEnable = 0;
		}	*/	
	} 
	return 0; 
}
