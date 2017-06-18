
#include "DTH11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#define UART_BAUD_RATE 115200
#define UART_BAUD_RATE 500000
#include "uart.h"

volatile int8_t temperature = 0;
volatile int8_t humidity = 0;
volatile char loop = 20;
volatile int8_t times[5] = {999,999,999,999,999}; //tablica zawiera czas przez który ma dioda œwieci w milisekundach (999- oznacza nie œwiecenie)
char printbuff[30];
char inputbuff[30];

ISR (TIMER0_OVF_vect)
{
	for(int i = 0; i<5 ; i++)
	{
		if(times[i] != 999)
		{
			if(times[i] >= 0 && times[i] < 500){
				times[i] = times[i]+500;
			}else{
				PORTC &= ~(1 << i);
				times[i]=999;
			}
		}
	}
}

ISR(TIMER1_COMPA_vect) // przerwanie s³u¿y do obs³ugi wysy³ania danych co sekundê
{
	if(loop >= 10)
	{
		temperature=dht11_gettemperature();
		humidity = dht11_gethumidity();
		loop = 0;
	}
	loop++;
	uart_puts("T:");
	itoa(temperature, printbuff, 10);
	uart_puts(printbuff);
	uart_puts(" H:");
	itoa(humidity, printbuff, 10);
	uart_puts(printbuff);
	uart_puts("\r\n");		
}

void CTC0_init() //inciclalizacja timera1 pod wysy³anie
{
	// set up timer with prescaler = 1024 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value
	OCR1A = 17000; // 16MHZ to 110520 -> 10800;
	
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
}

void CTC1_init() //inciclalizacja timera1 pod wysy³anie 
{	
	 // set up timer with prescaler = 1024 and CTC mode
	 TCCR0 |= (1 << CS02)|(1 << CS00);	 
	 // initialize counter
	 TCNT0 = 0;	 	
	 // enable overflow interrupt
	 TIMSK |= (1 << TOIE0);
}

int main(void) 
{ 
	DDRC = 0x1F;
	PORTC = 0x1F;
	_delay_ms(1000);
	PORTC = 0x00;
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));
	CTC1_init();
	CTC0_init();
	sei();
	unsigned int receivedData, aa=0;	
	char out = 0;	
	while(1) 
	{    
		receivedData = uart_available();	
		if(receivedData >= 6)
		{						
			for(int i = 0; i <= receivedData; i++)
			{
				if(i < 29)	
				{			
					aa = uart_getc();					
					inputbuff[i]= aa & 0xFF;					
				}								    	
			}		
			uart_flush();
			out = 0;				
			if(inputbuff[0]== 'D')
			{
				for(int j = 1; j < 6; j++)
				{			
					out +=  (inputbuff[j]-0x30)<<(j-1);					
					times[j-1] = 0;					
				}
				PORTC = out;				
			}														
		}		
	} 
	return 0; 
}
