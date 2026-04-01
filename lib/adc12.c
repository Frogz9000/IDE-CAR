/**
 * ******************************************************************************
 * @file    : adc12.h
 * @brief   : ADC module header file
 * @details : ADC initialization and interaction
 * @note    : ADC does not require IOMUX interaction
 * 
 * @author 
 * @date 
 * ******************************************************************************
*/
#include <ti/devices/msp/msp.h>
#include "lab5/adc12.h"

void ADC0_init(void)
{
	if ((ADC0->ULLMEM.GPRCM.PWREN & ADC12_PWREN_ENABLE_ENABLE) 
			!= ADC12_PWREN_ENABLE_ENABLE )
	{
		ADC0->ULLMEM.GPRCM.RSTCTL = ADC12_RSTCTL_KEY_UNLOCK_W | 
												ADC12_RSTCTL_RESETSTKYCLR_CLR;
		ADC0->ULLMEM.GPRCM.PWREN = ADC12_PWREN_KEY_UNLOCK_W |
											ADC12_PWREN_ENABLE_ENABLE;  
	}	
	
	ADC0->ULLMEM.GPRCM.CLKCFG = ADC12_CLKCFG_SAMPCLK_ULPCLK |
															ADC12_CLKCFG_KEY_UNLOCK_W;

	ADC0->ULLMEM.CLKFREQ = ADC12_CLKFREQ_FRANGE_RANGE40TO48;
	
	ADC0->ULLMEM.CTL0 = (ADC12_CTL0_SCLKDIV_DIV_BY_8 |
												ADC12_CTL0_PWRDN_MANUAL);
	ADC0->ULLMEM.CTL1 = (ADC12_CTL1_CONSEQ_SINGLE | 
												ADC12_CTL1_SAMPMODE_AUTO);
	ADC0->ULLMEM.MEMCTL[0] = ADC12_MEMCTL_CHANSEL_CHAN_0;
	ADC0->ULLMEM.SCOMP0 = 0;
	ADC0->ULLMEM.CPU_INT.IMASK = 0; 
}

uint32_t ADC0_getVal(void)
{
	
	ADC0->ULLMEM.CTL0 |= ADC12_CTL0_ENC_ON;
	ADC0->ULLMEM.CTL1 |= ADC12_CTL1_SC_START;
	while(ADC0->ULLMEM.STATUS & ADC12_STATUS_BUSY_ACTIVE);
	ADC0->ULLMEM.CTL1 |= ADC12_CTL1_SC_STOP;
	
	return ADC0->ULLMEM.MEMRES[0];
}
