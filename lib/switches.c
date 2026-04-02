/**
 * ******************************************************************************
 * @file    : switches.c
 * @details : Switches initialization and interaction
 * 
 * @author Vassily Dudkin
 * @date 1/21/2026
 * ******************************************************************************
*/

#include <stdio.h>
#include <ti/devices/msp/msp.h>
#include "arm_compat.h"
#include "core_cm0plus.h"
#include "switches.h"
void S1_init(void)
{
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40]|= (0x80 | 0x01);
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40] |= 0x00040000;
	
	//enable pull down resistor
	IOMUX->SECCFG.PINCM[IOMUX_PINCM40] |= IOMUX_PINCM_PIPD_ENABLE;
}

int S1_pressed(void)
{
	//check if pressed	
	if ((GPIOA->DIN31_0 & GPIO_DIN31_0_DIO18_MASK ))
		{
			return 1;
		}			
		else
		{
			return 0;
		}
}

void S2_init(void)
{
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49]|= (0x80 | 0x01);
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= 0x00040000;
	
	// Invert the internal logic of the I/O
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= 0x04000000;
	
	//enable pull up resistor
	IOMUX->SECCFG.PINCM[IOMUX_PINCM49] |= IOMUX_PINCM_PIPU_ENABLE;
}

int S2_pressed(void)
{
		if ((GPIOB->DIN31_0 & GPIO_DIN31_0_DIO21_MASK)) 
		{
			return 1;
		}
		else
		{
			return 0;
		}
}

/**
 * @brief Switch 1 interrupt initialization
 * @note Use NVIC_EnableIRQ() to register IRQn with the NVIC
 *       Check out `cmsis_armclang.h`
 * @hint Keep the polarity in mind
*/
void S1_init_interrupt(void)
{
	__disable_irq();
	S1_init();
	GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
	GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;
	GPIOA->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
	NVIC_EnableIRQ(GPIOA_INT_IRQn);
	__enable_irq();
}


/**
 * @brief Switch 2 interrupt initialization
 * @note Use NVIC_EnableIRQ() to register IRQn with the NVIC
 *       Check out `cmsis_armclang.h`
*/
void S2_init_interrupt(void)
{
  __disable_irq();
	S2_init();
	GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO21_CLR;
	GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO21_SET;
	GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO21_FALL;
	NVIC_EnableIRQ(GPIOB_INT_IRQn);
	__enable_irq();
}
