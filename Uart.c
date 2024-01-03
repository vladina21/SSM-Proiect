#include "serial.h"

uint8_t incomingPy = 0;

uint8_t Serial_read(void)
{
	//Punem in asteptare pana cand registrul de receptie nu este plin
	while(!(UART0->S1 & UART0_S1_RDRF_MASK)){}
	return UART0->D;
}

void reverseBitsCh(char* ch)
{
	*ch = (char)(((*ch & 0x01) << 7) | ((*ch & 0x02) << 5) | ((*ch & 0x04) << 3) | ((*ch & 0x08) << 1) |
                ((*ch & 0x10) >> 1) | ((*ch & 0x20) >> 3) | ((*ch & 0x40) >> 5) | ((*ch & 0x80) >> 7));
}

void flipBitsCh(char* ch)
{
	*ch = ~(*ch);
}

void Serial_printChar(char ch)
{
	//Verificam daca bitul MSBF este setat
  if (UART0->S2 & UART0_S2_MSBF_MASK) {
      //Inversam ordinea bitilor pentru printare
			reverseBitsCh(&ch);
  }
		
	//Punem in asteptare pana cand registrul de transmisie a datelor nu este gol
	while(!(UART0->S1 & UART0_S1_TDRE_MASK)){}
	UART0->D = ch;
}

void UART0_IRQHandler(void) {

		char c = Serial_read();
	  Serial_printChar(c);
	
		if (UART0->S2 & UART0_S2_MSBF_MASK)
		{
		  //reverseBitsCh(&c);
		}
		if (UART0->S2 & UART0_S2_RXINV_MASK)
		{
			//flipBitsCh(&c);
		}
		
		//incomingPy = c - '0';
		incomingPy = c;
}

void Serial_Init(uint32_t baud_rate)
{
	
	//Setarea sursei de ceas pentru modulul UART
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	
	//Activarea semnalului de ceas pentru modulul UART
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	
	//Activarea semnalului de ceas pentru portul A
	//intrucat dorim sa folosim pinii PTA1, respectiv PTA2 pentru comunicarea UART
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	//Fiecare pin pune la dispozitie mai multe functionalitati 
	//la care avem acces prin intermediul multiplexarii
	PORTA->PCR[1] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare RX pentru UART0
	PORTA->PCR[2] = ~PORT_PCR_MUX_MASK;
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Configurare TX pentru UART0

	
	UART0->C2 &= ~((UART0_C2_RE_MASK) | (UART0_C2_TE_MASK)); 
	
	//Configurare Baud Rate
	uint32_t osr = 3; // Over-Sampling Rate (numarul de esantioane luate per bit-time)
	
	//SBR - vom retine valoarea baud rate-ului calculat pe baza frecventei ceasului de sistem
	// 	 SBR  -		b16 b15 b14 [b13 b12 b11 b10 b09		b08 b07 b06 b05 b04 b03 b02 b01] &
	// 0x1F00 -		0		0   0    1   1   1   1   1      0   0   0   0   0   0   0   0
	//            0   0   0    b13 b12 b11 b10 b09    0   0   0   0   0   0   0   0 >> 8
	//   BDH  -   0   0   0    b13 b12 b11 b10 b09
	//   BDL  -   b08 b07 b06  b05 b04 b03 b02 b01
	
	uint16_t sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK)/(baud_rate * (osr+13)));
  //uint16_t sbr = 136;
	uint8_t temp = UART0->BDH & ~(UART0_BDH_SBR(0x1F));
	UART0->BDH = temp | UART0_BDH_SBR(((sbr & 0x1F00)>> 8));
	UART0->BDL = (uint8_t)(sbr & UART_BDL_SBR_MASK);
	UART0->C4 |= UART0_C4_OSR(osr);
	
	
	//Setare numarul de biti de date la 8 si fara bit de paritate
	UART0->C1 = 0;
	
	//Dezactivare intreruperi la transmisie
	UART0->C2 |= UART0_C2_TIE(0);
	UART0->C2 |= UART0_C2_TCIE(0);
	
	//Activare intreruperi la receptie
	UART0->C2 |= UART0_C2_RIE(1);
	
	UART0->C2 |= ((UART_C2_RE_MASK) | (UART_C2_TE_MASK));
	
	NVIC_EnableIRQ(UART0_IRQn);
	
	//Setare data frame - msb first
	//UART0->S2 |= UART0_S2_MSBF_MASK;
	//UART0->S2 |= UART0_S2_MSBF_MASK | UART0_S2_RXINV_MASK;

	UART0->S2 |= UART0_S2_RXINV_MASK;
	//Inversare rx
	//UART0->S2 = 16;
}

void Serial_print(char* string)
{
	int i = 0;
	while(string[i] != '\0'){
		Serial_printChar(string[i]);
		i++;
	}
}
void Serial_println(char* string)
{
	Serial_print(string);
	Serial_printChar('\n');
	Serial_printChar('\r');
}