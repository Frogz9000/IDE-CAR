#include <ti/devices/msp/msp.h>
#include "lab1/switches.h"

void S1_init(void){
	//enable GPIO A Peripheral
	if(!(GPIOA->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOA->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOA->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
		int i = 0;
		while (i < 4){
			i++;//wait ~4 upclock cycles for enable to propogate
		}
	}
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40] |= (0x80 | 0x01); //set pin to IO
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40] |= IOMUX_PINCM_INENA_ENABLE; //set to input
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40] |= (IOMUX_PINCM_PIPD_ENABLE); //activate pull-down resister
}

void S2_init(void){
	//Enable GPIOB Peripheral
	if(!(GPIOB->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
		int i = 0;
		while (i < 4){
			i++;//wait ~4 upclock cycles for enable to propogate
		}
	}
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= (0x80 | 0x01); //set pin to IO
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= IOMUX_PINCM_INENA_ENABLE; //set to input
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= IOMUX_PINCM_INV_ENABLE; //set invert logic
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= (IOMUX_PINCM_PIPU_ENABLE); //activate pull-up resister
	
	
}

int S1_pressed(void){
	return (GPIOA->DIN31_0 >> 18) & 1;
}

int S2_pressed(void){
	return (GPIOB->DIN31_0 >> 21) & 1;
}
