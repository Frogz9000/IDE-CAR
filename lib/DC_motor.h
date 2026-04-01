#ifndef _DC_MOTOR_H_
#define _DC_MOTOR_H_

#include <stdint.h>



void init_dc_motor0(uint32_t frequency, double percentDutyCycle);
void init_dc_motor1(uint32_t frequency, double percentDutyCycle);

void dc0_forward(double percentDutyCycle);

void dc0_backwards(double percentDutyCycle);

void dc1_forward(double percentDutyCycle);

void dc1_backwards(double percentDutyCycle);


#endif // _TIMERS_H_
