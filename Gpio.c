#include "Gpio.h"
#include "Pit.h"

uint8_t state;
uint8_t is_set_red;
uint8_t is_set_yellow;
uint8_t is_set_green;

void OutputPIN_Init(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Initializare PTB1
	PORTB->PCR[OUT_RED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[OUT_RED_PIN] |= PORT_PCR_MUX(1);
	
	GPIOB->PDDR |= (1<<OUT_RED_PIN);
	GPIOB->PCOR |= (1<<OUT_RED_PIN);
	
	// Initializare PTB2
	PORTB->PCR[OUT_YELLOW_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[OUT_YELLOW_PIN] |= PORT_PCR_MUX(1);
	
	GPIOB->PDDR |= (1<<OUT_YELLOW_PIN);
	GPIOB->PCOR |= (1<<OUT_YELLOW_PIN);
	
	// Initializare PTB3
	PORTB->PCR[OUT_GREEN_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[OUT_GREEN_PIN] |= PORT_PCR_MUX(1);
	
	GPIOB->PDDR |= (1<<OUT_GREEN_PIN);
	GPIOB->PCOR |= (1<<OUT_GREEN_PIN);
	
	// Activare PTB1 (led verde)
	GPIOB->PTOR = (1 << OUT_GREEN_PIN);
	
	is_set_red = 0;
	is_set_yellow = 0;
	is_set_green = 1;
}

void PORTA_IRQHandler() {
	
	
	// Prin utilizarea variabilei state, putem realiza un FSM
	// si sa configuram fiecare tranzitie in parte prin 
	// stingerea ledului anterior si aprinderea ledului curent
	if(state == 0) {
		GPIOB_PTOR |= (1<<RED_LED_PIN);
		GPIOD_PTOR |= (1<<BLUE_LED_PIN);
		state = 1;
	} else if (state == 1) {
		GPIOB_PTOR |= (1<<RED_LED_PIN);
		state = 2;
	} else if (state == 2) {
		GPIOB_PTOR |= (1<<RED_LED_PIN);
		GPIOB_PTOR |= (1<<GREEN_LED_PIN);
		state = 3;
	} else if (state == 3) {
		GPIOB_PTOR |= (1<<GREEN_LED_PIN);
		GPIOD_PTOR |= (1<<BLUE_LED_PIN);
		GPIOB_PTOR |= (1<<RED_LED_PIN);
		state = 0;
	}
	
	PORTA_ISFR |= (1<<LED_PIN);	
}
void RGBLed_Init(void){
	
	// Activarea semnalului de ceas pentru pinii folositi în cadrul led-ului RGB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// --- RED LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<RED_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PCOR |= (1<<RED_LED_PIN);
	
	// --- GREEN LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<GREEN_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<GREEN_LED_PIN);
	
	// --- BLUE LED ---
	
		// Utilizare GPIO ca varianta de multiplexare
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOD_PDDR |= (1<<BLUE_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOD_PSOR |= (1<<BLUE_LED_PIN);
	
  state = 0;
}
