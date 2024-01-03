#include "Gpio.h"

#define LED_PIN (12) // PORT A
#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1) // PORT D

uint8_t state;

void OutputPIN_Init(void){
	
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LED_PIN] |= PORT_PCR_MUX(1);
	
	GPIOA->PDDR |= (1<<LED_PIN);
	GPIOA->PCOR |= (1<<LED_PIN);
	
	state = 0;
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
	
	PORTA_ISFR = (1<<LED_PIN);
}
void RGBLed_Init(void){
	
	// Activarea semnalului de ceas pentru pinii folositi �n cadrul led-ului RGB
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
}