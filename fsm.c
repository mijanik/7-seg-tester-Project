#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "fsm.h"
#include "led.h"
#include "ADC.h"
#include "lcd1602.h"
#include <stdio.h>
#include "buttons.h"
#include "uart0.h"

float volt_coeff = ((float)(((float)2.91) / 4095) );			//ADC to voltage conversion
uint8_t wynik_ok=0;
uint16_t temp;
float	wynik;
char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

//Automatic mode - ADC
void ADC0_IRQHandler(){	
	temp = ADC0->R[0];						// Read value
	if(!wynik_ok){								// Check if the result was read by the main loop
		wynik = temp;								// Send new data to main loop
		wynik_ok=1;									// Set "result updated" flag
	}
	NVIC_EnableIRQ(ADC0_IRQn);
}


//Current FSM state flags
typedef enum {
	State_0_clear,								// FSM state - clear variables
	State_1_Wait_for_button,			// FSM state - Wait for user to press the button to start
  State_2_LED_ON,								// FSM state - Power on chosen segment on chosen display (dependent on flag)
  State_3_ADC,									// FSM state - Read 10 samples from ADC, calculate average
	State_4_Compare,							// FSM state - Compare average values with defined values
	State_5_LED_OFF,							// FSM state - Power off chosen segment on chosen display (dependent on flag)
	State_6_Inc_disp_seg,					// FSM state - Increment segment or display flag + Check segment and display flag values and take action
	State_7_UART						 		  // FSM state - Send all measurments to PC
	
} FSMStateType;
static FSMStateType fsmState;		//variable containing current FSM state



//Function declarations
void nextFSMState(void);
FSMStateType state_0(void);
FSMStateType state_1(void);
FSMStateType state_2(void);
FSMStateType state_3(void);
FSMStateType state_4(void);
FSMStateType state_5(void);
FSMStateType state_6(void);
FSMStateType state_7(void);

//FSM control variables

static unsigned int count_display = 0;
static unsigned int count_segment = 0;

double measurements[100];
double avg_meas;
double measurement_memory[32];
int count_measurement;
double ref_brightness[32] = {0.008, 0.011, 0.011, 0.013, 0.022, 0.016, 0.010, 0.0007107, 0.010, 0.013, 0.011, 0.010, 0.010, 0.011, 0.009, 0.0007107, 0.008, 0.011, 0.011, 0.012, 0.013, 0.014, 0.009, 0.0007107, 0.006, 0.026, 0.031, 0.008, 0.011, 0.011, 0.008, 0.0007107};
int flag_error[32];
int count_failed = 0;
static unsigned int flag_button_pressed = 0;

//FSM, LCD initialization
void fsmInitialize() {
	fsmState = State_0_clear;
	uint8_t	kal_error;
	LCD1602_Init();
	LCD1602_Backlight(TRUE);
		
	//ADC Calibration
	kal_error=ADC_Init();
	if(kal_error){	
		while(1);
	}
	
	//First run ADC - channel 0
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0);
	
	
	//Display on LCD
	sprintf(display,"Press button to");
	LCD1602_SetCursor(0,0);
	LCD1602_Print(display);	
	sprintf(display,"start the test");
	LCD1602_SetCursor(0,1);
	LCD1602_Print(display);	
}


/*----------------------------------------------------------------------------
 Function that moves FSM to phase 0 - clear variables
 *----------------------------------------------------------------------------*/

FSMStateType state_0(void){
	
		
	for(int i=0; i<32; i++){
		measurement_memory[i]=0;
		flag_error[i]=0;
	}
	
	count_failed = 0;
	count_display = 0;
	count_segment = 0;
	count_measurement=0;
	avg_meas = 0;

	
	return State_1_Wait_for_button;
}
/*----------------------------------------------------------------------------
 Function that moves FSM to phase 1 - Wait for user to press the button to start
 *----------------------------------------------------------------------------*/
FSMStateType state_1(void){
	//flag_button_pressed = 1;
	if (flag_button_pressed == 0){ 					//Read START button
		return State_1_Wait_for_button;
	}
	else

	sprintf(display,"Testing ...     ");
	LCD1602_SetCursor(0,0);
	LCD1602_Print(display);	
	sprintf(display,"                ");
	LCD1602_SetCursor(0,1);
	LCD1602_Print(display);	
	
	PTA->PDOR &= ~(1<<11);//GREEN LED OFF
	PTA->PDOR &= ~(1<<9);//RED LED OFF
	
		return State_2_LED_ON;
}

/*----------------------------------------------------------------------------
 Function that moves FSM to phase 2 - Power on chosen segment on chosen display (dependent on flag)
 *----------------------------------------------------------------------------*/
FSMStateType state_2(void){
	flag_button_pressed = 0;
	DISP_Select(count_display);
	LED_Ctrl(count_segment, LED_ON);
	
	return State_3_ADC;			
}

/*----------------------------------------------------------------------------
 Function that moves FSM to phase 3 - Read 10 samples from ADC, calculate average
 *----------------------------------------------------------------------------*/
FSMStateType state_3(void){
	
	for(int i=0; i<100;i++){
		measurements[i]=0;
  }

	for(int i=0; i<100;){
		if(wynik_ok){
		wynik = wynik*volt_coeff;						// Adjust voltage
		measurements[i] = wynik;
		wynik_ok=0;                 				//set flag - confirm read ADC result
		i++;
		}
	}
	return State_4_Compare;
}

/*----------------------------------------------------------------------------
 Function that moves FSM to phase 4 - Compare average values with defined values
 *----------------------------------------------------------------------------*/
FSMStateType state_4(void){
	
	avg_meas = 0;
	//Calulate and save average brightness value
	for(int i=1; i<100;i++){
		avg_meas += measurements[i];
	}
	avg_meas /= 99;
	measurement_memory[count_measurement]=avg_meas;
	
	//Compare display brightness with reference brightness
	if(measurement_memory[count_measurement]<ref_brightness[count_measurement]){
		flag_error[count_measurement]=1;
	}
	
	count_measurement++;
	return State_5_LED_OFF;
}

/*----------------------------------------------------------------------------
 Function that moves FSM to phase 5 - Power off chosen segment on chosen display (dependent on flag)
 *----------------------------------------------------------------------------*/

FSMStateType state_5(void){
	
	DISP_Select(count_display);
	LED_Ctrl(count_segment, LED_OFF);
	
	return State_6_Inc_disp_seg;
}


/*----------------------------------------------------------------------------
 Function that moves FSM to phase 6 - Display and segment switching
 *----------------------------------------------------------------------------*/


FSMStateType state_6(void){
	
	count_segment++;
	if(count_segment>7){
			count_display++;
			count_segment=0;
			if(count_display>3){
				count_measurement = 0;
				return State_7_UART;
			}
	}
		
	return State_2_LED_ON;		
}

/*----------------------------------------------------------------------------
 Function that moves FSM to phase 7 - Sending result to UART and LCD
 *----------------------------------------------------------------------------*/

FSMStateType state_7(void){
	
	
	for(int i=0; i<32; i++){
		count_failed += flag_error[i];
	}
	
	//Display on LCD
	sprintf(display,"Bad segments: %d ",count_failed);
	LCD1602_SetCursor(0,0);
	LCD1602_Print(display);
	
	if(count_failed == 0) {
		sprintf(display,"    --PASS--    ");
		PTA->PDOR|=(1<<11); //GREEN LED ON
	}
	else{
		sprintf(display,"    --FAIL--    ");
		PTA->PDOR|=(1<<9); //RED LED ON
	}
	LCD1602_SetCursor(0,1);
	LCD1602_Print(display);
	
 
	char rx_buf[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20, 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	

	sprintf(rx_buf,"Test Results: [0 = PASS, 1 = FAIL]%c%c", 0xd, 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	

	sprintf(rx_buf,"  %d         %d         %d         %d%c", flag_error[0], flag_error[8], flag_error[16], flag_error[24], 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"%d   %d     %d   %d     %d   %d     %d   %d%c", flag_error[5], flag_error[1], flag_error[13], flag_error[9], flag_error[21], flag_error[17], flag_error[29], flag_error[25], 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"  %d         %d         %d         %d%c", flag_error[6], flag_error[14], flag_error[22], flag_error[30], 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"%d   %d     %d   %d     %d   %d     %d   %d%c", flag_error[4], flag_error[2], flag_error[12], flag_error[10], flag_error[20], flag_error[18], flag_error[28], flag_error[26], 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"  %d    %d    %d    %d    %d    %d    %d    %d%c", flag_error[3], flag_error[7], flag_error[11], flag_error[15], flag_error[19], flag_error[23], flag_error[27], flag_error[31], 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"%cFailed segments: %d%c", 0xd, count_failed, 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	if(count_failed == 0) sprintf(rx_buf,"TEST PASSED%c", 0xd);
	else sprintf(rx_buf,"TEST FAILED%c", 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	sprintf(rx_buf,"---------------------------------------%c", 0xd);
	for(int i=0;rx_buf[i]!=0;i++){
		while(!(UART0->S1 & UART0_S1_TDRE_MASK));
		UART0->D = rx_buf[i];
	}
	
	return State_0_clear;
}
	

/*----------------------------------------------------------------------------
 Function that makes FSM transition to the next state  
 *----------------------------------------------------------------------------*/
void nextFSMState (void) {	
	
	switch(fsmState){
		case State_0_clear:
			fsmState = state_0();
			break;
		case State_1_Wait_for_button:
			fsmState = state_1();
			break;
		case State_2_LED_ON:
			fsmState = state_2();
		  break;
		case State_3_ADC:
			fsmState = state_3();
		  break;
		case State_4_Compare:
			fsmState = state_4();
		  break;
		case State_5_LED_OFF:
			fsmState = state_5();
		  break;
		case State_6_Inc_disp_seg:
			fsmState = state_6();
		  break;
		case State_7_UART:
			fsmState = state_7();
		  break;
	}
}

//Global function - Set button flag
void set_flag_button_pressed(void){
	flag_button_pressed = 1;
}

