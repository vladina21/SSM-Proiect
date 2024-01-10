#include "MKL25Z4.h"

uint8_t UART_read(void); 						// Reads Serial buffer
void UART_Init(uint32_t baud_rate);	// Initialises serial transmission
void UART_printChar(char ch); 			// Transmit 1 byte through serial UART
void UART_print(char* string); 			// Transmit string through serial UART
void UART_println(char* string); 		// Transmit string followed bt CR&LF through serial UART
