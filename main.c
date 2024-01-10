#include "Uart.h"
#include "Gpio.h"
#include "Pit.h"
#include "Adc.h"
#include "stdio.h"

extern uint8_t state;
extern uint8_t incomingPy;

int main()
{
	UART_Init(14400);
	//OutputPIN_Init();
	//RGBLed_Init();
	//PIT_Init();
	//ADC0_Init();
	
	//UART_println("test msbf");

	for(;;)
  {
		/*
		if(incomingPy)
		{
			sprintf(charArray, "%hhu", incomingPy);
			UART_println("\nincoming: ");
			UART_println(charArray);
			incomingPy = 0;
		}
		*/
	}
}
