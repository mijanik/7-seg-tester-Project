/**
* @file main.c
* @authors Milosz Janik & Antoni Kijania
* @date Jan 2022
* @brief File containing the main function.
* @ver 1.2
*/				

#include "MKL05Z4.h"
#include "fsm.h"
#include "buttons.h"
#include "led.h"
#include "uart0.h"

void SysTick_Handler(void) {
	nextFSMState();											
}

int main (void){
	
	//Initialization
	LED_Init();
	buttonsInitialize();
	UART0_Init();
	fsmInitialize();
	SysTick_Config(SystemCoreClock / /*20*/20);
	
	while(1){
	__WFI(); // Wait for interrupt
	}
}
