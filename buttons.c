#include "MKL05Z4.h"
#include "buttons.h"
#include "fsm.h"

#define BUT1 10           										//PTB1 button

//IRQ Handler
void PORTA_IRQHandler(void){
		if( PORTA->ISFR & (1 << BUT1) ){          //Check if button pressed
		set_flag_button_pressed();                //ISR routine
    while((FPTA->PDIR&(1<<BUT1))==0);         //Wait to realese
		PORTA->PCR[BUT1] |= PORT_PCR_ISF_MASK;    //Clear flag
  }
}

//Initialization
void buttonsInitialize(void){
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK; 				// Enable clock for port B
	PORTA->PCR[BUT1] |= PORT_PCR_MUX(1);      	// Pin PTB8 is GPIO

	
	//Activate pull up and interrupt
	PORTA->PCR[BUT1] |=  PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[BUT1] |= 	PORT_PCR_IRQC(0x0A);
	
		
	//NVIC configuration
	NVIC_ClearPendingIRQ(PORTA_IRQn);				  	// Clear NVIC pending interrupts
	NVIC_EnableIRQ(PORTA_IRQn);							  	// NVIC interrupts source
	NVIC_SetPriority (PORTA_IRQn, 3);			    	// Interrupt priority level
}

