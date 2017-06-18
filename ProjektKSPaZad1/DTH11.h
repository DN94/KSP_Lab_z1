
#ifndef DTH11_H_
#define DTH11_H_

#include <avr/io.h> 

//#define F_CPU 11059200UL
#define F_CPU 16000000UL
#define DHT11_DDR DDRD
#define DHT11_PORT PORTD
#define DHT11_PIN PIND
#define DHT11_INPUTPIN PD7
#define DHT11_ERROR 255

int8_t dht11_gettemperature();
int8_t dht11_gethumidity();



#endif /* DTH11_H_ */