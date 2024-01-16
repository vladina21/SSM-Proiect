#include "MKL25Z4.h"

#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1) // PORT D

#define OUT_RED_PIN (1) // PORT B
#define OUT_YELLOW_PIN (2) // PORT B
#define OUT_GREEN_PIN (3) // PORT B

void OutputPIN_Init(void);
void RGBLed_Init(void);
void RGBLed_Deinit(void);
void PORTA_IRQHandler(void);