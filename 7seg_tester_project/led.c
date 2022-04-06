#include "led.h"

//Private memory declarations

#ifdef FRDM_KL05Z

//Segment Init Array
# define LED_N   8
	static PinStruct_Type led[LED_N] = {
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 0}, //A
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 5}, //B
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 6}, //C
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 7}, //D
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 8}, //E
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 9}, //F
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 11}, //G
		{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 13}  //DP
	};
	
//Display Init Array
# define DISP_N   4
	static PinStruct_Type disp[DISP_N] = {
		{PTA, PORTA, SIM_SCGC5_PORTA_MASK, 5}, //Display 0
		{PTA, PORTA, SIM_SCGC5_PORTA_MASK, 6}, //Display 1
		{PTA, PORTA, SIM_SCGC5_PORTA_MASK, 7}, //Display 2
		{PTA, PORTA, SIM_SCGC5_PORTA_MASK, 8}  //Display 3
	};
#endif

void LED_Init(void) {  
  for(int i = 0; i < LED_N; i++) {
    SIM->SCGC5 |= led[i].clk_mask;              /* connect CLOCK to port */
	  led[i].port->PCR[led[i].pin] = PORT_PCR_MUX(1UL); /* set MUX to GPIO */
	  led[i].gpio->PDDR |= MASK(led[i].pin);            /* set as OUTPUT   */
	  led[i].gpio->PSOR |= MASK(led[i].pin);            /* OFF as default  */
  } /* do for all leds */
	
	 for(int i = 0; i < DISP_N; i++) {
    SIM->SCGC5 |= disp[i].clk_mask;              /* connect CLOCK to port */
	  disp[i].port->PCR[disp[i].pin] = PORT_PCR_MUX(1UL); /* set MUX to GPIO */
	  disp[i].gpio->PDDR |= MASK(disp[i].pin);            /* set as OUTPUT   */
	  disp[i].gpio->PSOR |= MASK(disp[i].pin);            /* OFF as default  */
  } /* do for all displays */
	 
	//PASS/FAIL LEDS init
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[9] |= PORT_PCR_MUX(1); 										//RED PTA9
	PORTA->PCR[11] |= PORT_PCR_MUX(1);										//GREEN PTA11
	PTA->PDDR |= (1<<9);
	PTA->PDDR |= (1<<11);
	
}

void LED_Ctrl(int segment, LedState_Type led_state) {  

  switch (led_state){
    case LED_OFF:     led[segment].gpio->PSOR |= MASK(led[segment].pin);
      break;
    case LED_ON:      led[segment].gpio->PCOR |= MASK(led[segment].pin);
      break;
    case LED_TOGGLE:  led[segment].gpio->PTOR |= MASK(led[segment].pin);
      break;
  } 
}

void DISP_Select(int disp_num) {
	
	switch (disp_num){
		case 0: 
			disp[0].gpio->PCOR |= MASK(disp[0].pin); //turn ON display 0
			disp[1].gpio->PSOR |= MASK(disp[1].pin); //disable display 1
			disp[2].gpio->PSOR |= MASK(disp[2].pin); //disable display 2
			disp[3].gpio->PSOR |= MASK(disp[3].pin); //disable display 3
			break;
		case 1:
			disp[0].gpio->PSOR |= MASK(disp[0].pin); //disable display 0
			disp[1].gpio->PCOR |= MASK(disp[1].pin); //turn ON display 1
			disp[2].gpio->PSOR |= MASK(disp[2].pin); //disable display 2
			disp[3].gpio->PSOR |= MASK(disp[3].pin); //disable display 3
			break;
		case 2:
			disp[0].gpio->PSOR |= MASK(disp[0].pin); //disable display 0
			disp[1].gpio->PSOR |= MASK(disp[1].pin); //disable display 1
			disp[2].gpio->PCOR |= MASK(disp[2].pin); //turn ON display 2
			disp[3].gpio->PSOR |= MASK(disp[3].pin); //disable display 3
			break;
		case 3:
			disp[0].gpio->PSOR |= MASK(disp[0].pin); //disable display 0
			disp[1].gpio->PSOR |= MASK(disp[1].pin); //disable display 1
			disp[2].gpio->PSOR |= MASK(disp[2].pin); //disable display 2
			disp[3].gpio->PCOR |= MASK(disp[3].pin); //turn ON display 3
			break;
	}
}
