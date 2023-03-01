#include "ADC.h"

uint8_t ADC_Init(void)
{
	uint16_t kalib_temp;
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;      // Enable clock for ADC0
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;			// Enable clock for PORTA
	PORTA->PCR[12] &= ~(PORT_PCR_MUX(0));		// Set PTA12 as analog input

	
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_4) | ADC_CFG1_ADLSMP_MASK;	// ADCK clock = 2.62MHz (2621440Hz)
	ADC0->CFG2 = ADC_CFG2_ADHSC_MASK;																														// Enable support for high clock frequency 
	ADC0->SC3  = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);																						// Averageing 32 samples 
	
	//Calibration
	ADC0->SC3 |= ADC_SC3_CAL_MASK;
	while(ADC0->SC3 & ADC_SC3_CAL_MASK);
	if(ADC0->SC3 & ADC_SC3_CALF_MASK){
	  ADC0->SC3 |= ADC_SC3_CALF_MASK;
	  return(1);
	}

	kalib_temp = 0x00;
	kalib_temp += ADC0->CLP0;
	kalib_temp += ADC0->CLP1;
	kalib_temp += ADC0->CLP2;
	kalib_temp += ADC0->CLP3;
	kalib_temp += ADC0->CLP4;
	kalib_temp += ADC0->CLPS;
	kalib_temp += ADC0->CLPD;
	kalib_temp /= 2;
	kalib_temp |= 0x8000;
	ADC0->PG = ADC_PG_PG(kalib_temp);          
	//ADC0->OFS = 0;														
	ADC0->SC1[0] = ADC_SC1_ADCH(31);
	ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK;
	
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_1) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(MODE_12);	// ADCK clock 10.49MHz (10485760Hz), 12 bit resolution, long sampling time
	ADC0->SC3 |= ADC_SC3_ADCO_MASK;								// Continous processing
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_EnableIRQ(ADC0_IRQn);
	
	return(0);
}

