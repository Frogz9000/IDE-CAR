#include <ti/devices/msp/msp.h>
#include "servo.h"
#include "lab6/timers.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

void init_servo_motor(uint32_t frequency, double percentDutyCycle){
	//forward driver pin
	TIMA1_PWM_freq_init(0, frequency, percentDutyCycle);
	//reverse driver pin, starts with zero
	//TIMA1_PWM_init(1, period, prescaler, 0.0);
}


