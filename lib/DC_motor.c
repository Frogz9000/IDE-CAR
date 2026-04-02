#include <ti/devices/msp/msp.h>
#include "DC_motor.h"
#include "timers.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void init_dc_motor0(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	TIMA0_PWM_freq_init(1, frequency, 0.0);
	//forward driver pin
	TIMA0_PWM_freq_init(0, frequency, percentDutyCycle);
	
	
}
void dc0_forward(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(0, percentDutyCycle);
	TIMA0_PWM_DutyCycle(1, 0.0);
}

void dc0_backwards(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(0, 0.0);
	TIMA0_PWM_DutyCycle(1, percentDutyCycle);
}

void init_dc_motor1(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	TIMA0_PWM_freq_init(3, frequency, 0.0);
	//forward driver pin
	TIMA0_PWM_freq_init(2, frequency, percentDutyCycle);
}
void dc1_forward(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(2, percentDutyCycle);
	TIMA0_PWM_DutyCycle(3, 0.0);
}

void dc1_backwards(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(2, 0.0);
	TIMA0_PWM_DutyCycle(3, percentDutyCycle);
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
	GPIOA->DOUTCLR31_0 |= (1 << 22); //set low since led active high

}

void init_dc_motors(uint32_t frequency, double percentDutyCycle){
	TIMA0_PWM_freq_init(1, frequency, 0.0);
	TIMA0_PWM_freq_init(3, frequency, 0.0);
	TIMA0_PWM_freq_init(0, frequency, percentDutyCycle);
	TIMA0_PWM_freq_init(2, frequency, percentDutyCycle);
	motor_enable();
}
