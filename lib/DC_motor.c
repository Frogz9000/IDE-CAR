#include <ti/devices/msp/msp.h>
#include "DC_motor.h"
#include "timers.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void init_dc_motor0(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	//TIMA0_PWM_freq_init(1, frequency, 0.0);
	IOMUX->SECCFG.PINCM[IOMUX_PINCM29] |= (0x80 | 0x01); //set port to IO
	GPIOA->DOESET31_0 |= 0x0C;
	GPIOA->DOUTCLR31_0 |= 0x0C;  
	//forward driver pin
	TIMA0_PWM_freq_init(0, frequency, percentDutyCycle);
}

void motors_forward(double percentDutyCycle)
{
		dc0_forward(percentDutyCycle);
	  dc1_forward(percentDutyCycle);
}


void dc0_forward(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(0, percentDutyCycle);
}


void init_dc_motor1(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	//TIMA0_PWM_freq_init(3, frequency, 0.0);
	IOMUX->SECCFG.PINCM[IOMUX_PINCM30] |= (0x80 | 0x01); //set port to IO
	GPIOA->DOESET31_0 |= 0x0D;
	GPIOA->DOUTCLR31_0 |= 0x0D; 
	//forward driver pin
	TIMA0_PWM_freq_init(2, frequency, percentDutyCycle);
}
void dc1_forward(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(2, percentDutyCycle);
}


void motor_enable(){
	//enable GPIO A Peripheral
	if(!(GPIOA->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOA->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOA->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
	}
	//Enable GPIOB
	if(!(GPIOB->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
	}
	IOMUX->SECCFG.PINCM[IOMUX_PINCM45] |= (0x80 | 0x01); 
	IOMUX->SECCFG.PINCM[IOMUX_PINCM45] |= IOMUX_PINCM_DRV_DRVVAL1;
	GPIOB->DOESET31_0 |= (1 << 19); 
	GPIOB->DOUTSET31_0 |= (1 << 19); 
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM47] |= (0x80 | 0x01); //set red to IO
	IOMUX->SECCFG.PINCM[IOMUX_PINCM47] |= IOMUX_PINCM_DRV_DRVVAL1; //set to high drive strength
	GPIOA->DOESET31_0 |= (1 << 22); //enable output to PB26
	GPIOA->DOUTSET31_0 |= (1 << 22); //set low since led active high

}

void init_dc_motors(uint32_t frequency, double percentDutyCycle){
	//enable GPIO A Peripheral
	if(!(GPIOA->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOA->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOA->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
	}
	//Enable GPIOB
	if(!(GPIOB->GPRCM.PWREN & 1U)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | 1U);
	}
	
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM30] |= (0x80 | 0x01); //set port to IO
	IOMUX->SECCFG.PINCM[IOMUX_PINCM29] |= (0x80 | 0x01); //set port to IO
	GPIOB->DOESET31_0 |= (1 << 13); 
	GPIOB->DOUTCLR31_0 |= (1 << 13); 

	
	GPIOB->DOESET31_0 |= (1 << 12); 
	GPIOB->DOUTCLR31_0 |= (1 << 12); 
	TIMA0_PWM_freq_init(0, frequency, percentDutyCycle);
	TIMA0_PWM_freq_init(2, frequency, percentDutyCycle);


	motor_enable();
}