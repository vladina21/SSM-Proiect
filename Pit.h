#include "MKL25Z4.h"

#define LED_PIN (12) // PORT A

void PIT_Init(void); // Functie de initializarea a modulului periferic
void PIT_Deinit(void);
void PIT_IRQHandler(void); // Functia de tratarea a intreruperilor pentru perifericul PIT
uint8_t UTILS_PrintTimer(uint32_t value); // Functie menita sa printeze o valoare numarata in formatul MM:SS