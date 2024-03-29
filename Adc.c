#include "Adc.h"
#include "Uart.h"
#include "Gpio.h"

#define ADC_CHANNEL (8) // PORT B PIN 0

extern uint8_t is_set_red;
extern uint8_t is_set_yellow;
extern uint8_t is_set_green;

extern uint8_t incomingPy;

uint8_t is_sending;

void ADC0_Init() {
	
	// Activarea semnalului de ceas pentru modulul periferic ADC
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
	// Functia de calibrare
	ADC0_Calibrate();
	
	ADC0->CFG1 = 0x00;

	// Selectarea modului de conversie pe 8 biti single-ended --> MODE
	// Selectarea sursei de ceas pentru generarea ceasului intern --> ADICLK
	// Selectarea ratei de divizare folosit de periferic pentru generarea ceasului intern --> ADIV
	// Set ADC clock frequency fADCK less than or equal to 4 MHz (PG. 494)
	ADC0->CFG1 |= ADC_CFG1_MODE(0) |
							 ADC_CFG1_ADICLK(1) |
							 ADC_CFG1_ADIV(3);
	
	// DIFF = 0 --> Conversii single-ended (PG. 464)
	ADC0->SC1[0] = 0x00;
	ADC0->SC3 = 0x00;

	// Selectarea modului de conversii continue, 
	// pentru a-l putea folosi in tandem cu mecanismul de intreruperi
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;
	
	// Activarea subsistemului de conversie prin aproximari succesive pe un anumit canal (PG.464)
	ADC0->SC1[0] |= ADC_SC1_ADCH(ADC_CHANNEL);
	// Enables conversion complete interrupts
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;
	
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_SetPriority(ADC0_IRQn, 5);
	NVIC_EnableIRQ(ADC0_IRQn);	
	
	is_sending = 0;
}

int ADC0_Calibrate() {
	
	// ===== For best calibration results =====
	
	ADC0_CFG1 |= ADC_CFG1_MODE(0)  |  	// 8 bits mode
                ADC_CFG1_ADICLK(1)|  // Input Bus Clock divided by 2
                ADC_CFG1_ADIV(3);   // Clock divide by 8
	
	// The calibration will automatically begin if the SC2[ADTRG] is 0. (PG. 495)
	ADC0->SC2 &= ~ADC_SC2_ADTRG_MASK;
	
	// Set hardware averaging to maximum, that is, SC3[AVGE]=1 and SC3[AVGS]=0x11 for an average of 32 (PG. 494)
	ADC0->SC3 |= (ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3));
	
	// To initiate calibration, the user sets SC3[CAL] (PG. 495)
	ADC0->SC3 |= ADC_SC3_CAL_MASK;
	
	// At the end of a calibration sequence, SC1n[COCO] will be set (PG. 495)
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	
	// At the end of the calibration routine, if SC3[CALF] is not
	// set, the automatic calibration routine is completed successfully. (PG. 495)
	if(ADC0->SC3 & ADC_SC3_CALF_MASK){
		return (1);
	}
	
	// ====== CALIBRATION FUNCTION (PG.495) =====
	
	// 1. Initialize or clear a 16-bit variable in RAM.
	uint16_t calibration_var = 0x0000;
	
	// 2. Add the plus-side calibration results CLP0, CLP1, CLP2, CLP3, CLP4, and CLPS to the variable.
	calibration_var += ADC0->CLP0;
	calibration_var += ADC0->CLP1;
	calibration_var += ADC0->CLP2;
	calibration_var += ADC0->CLP3;
	calibration_var += ADC0->CLP4;
	calibration_var += ADC0->CLPS;
	
	// 3. Divide the variable by two.
	calibration_var /= 2;
	
	// 4. Set the MSB of the variable. 
	calibration_var |= 0x8000;
	
	// 5. Store the value in the plus-side gain calibration register PG.
	ADC0->PG = ADC_PG_PG(calibration_var);
	
	// 6. Repeat the procedure for the minus-side gain calibration value.
	calibration_var = 0x0000;
	
	calibration_var += ADC0->CLM0;
	calibration_var += ADC0->CLM1;
	calibration_var += ADC0->CLM2;
	calibration_var += ADC0->CLM3;
	calibration_var += ADC0->CLM4;
	calibration_var += ADC0->CLMS;
	
	calibration_var /= 2;
	
	calibration_var |= 0x8000;
	
	ADC0->MG = ADC_MG_MG(calibration_var);
	
	// Incheierea calibrarii
	ADC0->SC3 &= ~ADC_SC3_CAL_MASK;
	
	return (0);
}


uint16_t ADC0_Read(){
	
	// A conversion is initiated following a write to SC1A, with SC1n[ADCH] not all 1's (PG. 485)
	ADC0->SC1[0] |= ADC_SC1_ADCH(ADC_CHANNEL);
	
	// ADACT is set when a conversion is initiated
	// and cleared when a conversion is completed or aborted.
	while(ADC0->SC2 & ADC_SC2_ADACT_MASK);
	
	// A conversion is completed when the result of the conversion is transferred 
	// into the data result registers, Rn (PG. 486)
	
	// If the compare functions are disabled, this is indicated by setting of SC1n[COCO]
	// If hardware averaging is enabled, the respective SC1n[COCO] sets only if
	// the last of the selected number of conversions is completed (PG. 486)
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	
	return (uint16_t) ADC0->R[0];

}

void Output_Digital(uint16_t analog_input)
{
	if(analog_input >= 84)
	{	
		if(!is_set_yellow)
		{
			GPIOB->PTOR |= (1<<OUT_YELLOW_PIN);
			is_set_yellow = 1;
		}		
		if(analog_input >= 166 && !is_set_red)
		{
			GPIOB->PTOR |= (1<<OUT_RED_PIN);
			is_set_red = 1;
		}
		else if(analog_input < 166 && is_set_red)
		{
			GPIOB->PTOR |= (1<<OUT_RED_PIN);
			is_set_red = 0;
		}
	}	
	else if(analog_input < 84 && is_set_yellow)
	{
		GPIOB->PTOR |= (1<<OUT_YELLOW_PIN);
		is_set_yellow = 0;
	}
}

void ADC0_IRQHandler(){
	uint16_t analog_input = (uint16_t) ADC0->R[0];	

	if(is_sending && !incomingPy)
	{		
		uint8_t nr1=analog_input /100; 
		uint8_t nr2=analog_input %100 /10; 
		uint8_t nr3=analog_input %10;
			
		UART_print("Sensor: ");
		
		UART_printChar(nr1+48);
		UART_printChar(nr2+48);
		UART_printChar(nr3+48);
		UART_printChar(0x0D);
		UART_printChar(0x0A);
	}
	
	Output_Digital(analog_input);
}
