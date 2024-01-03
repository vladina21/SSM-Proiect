#include "MKL25Z4.h"

uint8_t Serial_read(void); 						// Reads Serial buffer
void Serial_Init(uint32_t baud_rate);	// Initialises serial transmission
void Serial_printChar(char ch); 			// Transmit 1 byte through serial UART
void Serial_print(char* string); 			// Transmit string through serial UART
void Serial_println(char* string); 		// Transmit string followed bt CR&LF through serial UART
