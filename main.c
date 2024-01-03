#include "Uart.h"
#include "Gpio.h"
#include "stdio.h"

extern uint8_t state;
extern uint8_t incomingPy;

int main()
{
	Serial_Init(14400);
	Serial_println("Start Program");
	Serial_println("------123123123----");
	Serial_println("<<BBbbFF");
	char charArray[10];

    // Convert uint8_t to char array
    
	for(;;)
  {
		if(incomingPy)
		{
			sprintf(charArray, "%hhu", incomingPy);
			Serial_println("\nincoming: ");
			Serial_println(charArray);
			incomingPy = 0;
		}
	}
	
	
}
