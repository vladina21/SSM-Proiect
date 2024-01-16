#include "Uart.h"
#include "Gpio.h"
#include "Pit.h"
#include "Adc.h"

#define START_LED_SEQUENCE 1
#define STOP_LED_SEQUENCE 2
#define REVERSE_LED_SEQUENCE 3
#define START_SENDING_SENSOR_DATA 4
#define STOP_SENDING_SENSOR_DATA 5	

extern uint8_t reverse;
extern uint8_t is_sending;
extern uint8_t incomingPy;

int main()
{
	UART_Init(14400);
	OutputPIN_Init();
  ADC0_Init();
	
	for(;;)
  {
		switch(incomingPy)
		{
			case START_LED_SEQUENCE:
				RGBLed_Init();
				PIT_Init();
			  incomingPy = 0;
			  break;
			
			case STOP_LED_SEQUENCE:
			  RGBLed_Deinit();
			  PIT_Deinit();
		    incomingPy = 0;
			  break;
			
			case REVERSE_LED_SEQUENCE:
			  reverse = reverse ? 0 : 1;
			  incomingPy = 0;
			  break;
			
			case START_SENDING_SENSOR_DATA:
				is_sending = 1;
			  incomingPy = 0;	  
			  break;
			
			case STOP_SENDING_SENSOR_DATA:
				is_sending = 0;
			  incomingPy = 0;
			  break;
			
			default:
				continue;
		}
	}
}
