#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"
#define LED_PIN (12) // PORT A
uint32_t timer_value;
uint8_t led_state;

void PIT_Init(void) {
	
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	// Setarea valoarea numaratorului de pe canalul 0 la o perioada de 1 secunda
	//PIT->CHANNEL[0].LDVAL = 0x9FFFFF;
	PIT->CHANNEL[0].LDVAL = 0xAF3332;
	
  // Activarea întreruperilor pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 0
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	// Setarea valoarea numaratorului de pe canalul 1 la o perioada de 1095 ms
	PIT->CHANNEL[1].LDVAL = 0xAF3332;
	
	// Activara întreruperilor pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	
	// Activarea întreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,5);
	NVIC_EnableIRQ(PIT_IRQn);
}

uint8_t UTILS_PrintTimer(uint32_t value){
	uint8_t minutes = value/60;
	uint8_t seconds = value%60;
	
	UART0_Transmit(0x0D);
	UART0_Transmit(minutes/10 + 0x30);
	UART0_Transmit(minutes%10 + 0x30);
	UART0_Transmit(0x3A);
	UART0_Transmit(seconds/10 + 0x30);
	UART0_Transmit(seconds%10 + 0x30);
	
	if(timer_value == 99*60+59){
		timer_value = 0;
	}
	
}

void PIT_IRQHandler(void) {
	
	if(PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		UTILS_PrintTimer(timer_value);
		timer_value++;
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		if(led_state) {
			GPIOA->PCOR |= (1<<LED_PIN);
			led_state = 0;
		}
	}
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		GPIOA->PSOR |= (1<<LED_PIN);
		UART0_Transmit(0x0A);
		led_state = 1;
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
		PORTA_IRQHandler();
	}
}