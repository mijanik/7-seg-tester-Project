#ifndef LED_H
#define LED_H

#include "frdm_bsp.h"

//Led State Types.
typedef enum {
	LED_OFF 			= (uint8_t)0,
	LED_ON 				= (uint8_t)1,
	LED_TOGGLE 		= (uint8_t)2
} LedState_Type;

//Structure containing info about pin.
typedef struct {
	GPIO_Type 	*gpio;        /* GPIO base pointer */
	PORT_Type 	*port;        /* PORT base pointer */
	uint32_t		clk_mask;     /* Mask for SCGC5 register */
	uint8_t  		pin;          /* Number of PIN */
} PinStruct_Type;

//LEDs initialization.
void LED_Init (void);

//Select display
void DISP_Select(int disp_num);

//Control choosen LED.
void LED_Ctrl(int segment, LedState_Type led_state);

#endif
