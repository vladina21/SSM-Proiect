#include "Pit.h"
#include "Uart.h"
#include "Gpio.h"

uint8_t led_state;

void PIT_Init(void) {
	
	// Activarea semnalului de ceas pentru perifericul PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	// Utilizarea semnalului de ceas pentru tabloul de timere
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	// Oprirea decrementarii valorilor numaratoarelor in modul debug
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Setarea valoarea numaratorului de pe canalul 1 la o perioada de 1095 ms
	PIT->CHANNEL[1].LDVAL = 0xAF3332;
	
	// Activara Óntreruperilor pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	// Activarea timerului de pe canalul 1
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TEN_MASK;
	
	
	// Activarea Óntreruperii mascabile si setarea prioritatiis
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_SetPriority(PIT_IRQn,0);
	NVIC_EnableIRQ(PIT_IRQn);
}

void PIT_Deinit(void) {
    // Dezactivez timerul de pe canalul 1
    PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TEN_MASK;

    //Dezactivez intreruperile de pe canalul 
    PIT->CHANNEL[1].TCTRL &= ~PIT_TCTRL_TIE_MASK;

    //Dezactivez pit clock
    SIM->SCGC6 &= ~SIM_SCGC6_PIT_MASK;
	
    NVIC_ClearPendingIRQ(PIT_IRQn);
}

void PIT_IRQHandler(void) {
	if(PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		GPIOA->PSOR |= (1<<LED_PIN);
		led_state = 1;
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
		PORTA_IRQHandler();
	}
}