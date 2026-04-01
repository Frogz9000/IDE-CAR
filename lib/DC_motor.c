#include <ti/devices/msp/msp.h>
#include "DC_motor.h"
#include "lab6/timers.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void init_dc_motor0(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	TIMA0_PWM_freq_init(2, frequency, 0.0);
	//forward driver pin
	TIMA0_PWM_freq_init(0, frequency, percentDutyCycle);
	
	
}
void dc0_forward(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(0, percentDutyCycle);
	TIMA0_PWM_DutyCycle(2, 0.0);
}

void dc0_backwards(double percentDutyCycle){
	TIMA0_PWM_DutyCycle(0, 0.0);
	TIMA0_PWM_DutyCycle(2, percentDutyCycle);
}

void init_dc_motor1(uint32_t frequency, double percentDutyCycle){
	//reverse driver pin, starts with zero
	TIMA1_PWM_freq_init(3, frequency, 0.0);
	//forward driver pin
	TIMA1_PWM_freq_init(2, frequency, percentDutyCycle);
}
void dc1_forward(double percentDutyCycle){
	TIMA1_PWM_DutyCycle(2, percentDutyCycle);
	TIMA1_PWM_DutyCycle(3, 0.0);
}

void dc1_backwards(double percentDutyCycle){
	TIMA1_PWM_DutyCycle(2, 0.0);
	TIMA1_PWM_DutyCycle(3, percentDutyCycle);
}
