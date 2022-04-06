#include "uart0.h"

void UART0_Init(void)
{
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;										//UART0 connected to clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;										//Port B connected to clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);					//Clock MCGFLLCLK=41943040Hz (CLOCK_SETUP=0)
	PORTB->PCR[1] = PORT_PCR_MUX(2);											//PTB1=TX_D
	PORTB->PCR[2] = PORT_PCR_MUX(2);											//PTB2=RX_D
	
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );	//Reciever and transmitter locked
	UART0->BDH = 0;
	UART0->BDL =91;																				//For CLOCK_SETUP=0 BR=28800	BDL=91	:	CLOCK_SETUP=1	BR=230400		BDL=13
	UART0->C4 &= ~UART0_C4_OSR_MASK;
	UART0->C4 |= UART0_C4_OSR(15);												//For CLOCK_SETUP=0 BR=28800	OSR=15	:	CLOCK_SETUP=1	BR=230400		OSR=15
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK;									//Reciever sampling on both clock edge types
	UART0->C2 |= UART0_C2_TE_MASK;												//Enable transmitter
}

