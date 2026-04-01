#include <ti/devices/msp/msp.h>
#include "servo.h"
#include "sysctl.h"
#include "timers.h"
#include "DC_motor.h"
void delay(int time_ms);

void delay(int time_ms){
	unsigned long cycles_per_ms = SYSCTL_SYSCLK_getMCLK()/1000;
	unsigned long time_to_clk_cycles = (unsigned long)time_ms*cycles_per_ms;
	
	for (volatile unsigned long i=time_to_clk_cycles; i>0; i--);
}

int main(){	

	init_dc_motor0(10000,0.20);
	init_dc_motor1(10000,0.20);
	init_servo_motor(50, 0.075);
	delay(10);
	//init_servo_motor(50, 0.05);
	//TIMA1_PWM_DutyCycle(0, 0.05);
	delay(10);
	init_servo_motor(50, 0.1);
	//TIMA1_PWM_DutyCycle(0, 0.075);
	double duty_cycle_iter = 0.0;
	while(1);
	while(1){
		//go 0 to 100 forward
		while(duty_cycle_iter<1){
			dc0_forward(duty_cycle_iter);
			duty_cycle_iter+=0.01;
			

			TIMA1_PWM_DutyCycle(0, 0.1);
		}
		//go 100 to 0 forward
		while (duty_cycle_iter>0){
			dc0_forward(duty_cycle_iter);
			duty_cycle_iter-=0.01;
			delay(10);
			TIMA1_PWM_DutyCycle(0, 0.05);
		}
		
		//go 0 to 100 backwards
		while(duty_cycle_iter<1){
			dc0_backwards(duty_cycle_iter);
			duty_cycle_iter+=0.01;
			delay(10);
			TIMA1_PWM_DutyCycle(0, 0.1);
		}
		//go 100 to 0 backwards
		while (duty_cycle_iter>0){
			dc0_backwards(duty_cycle_iter);
			duty_cycle_iter-=0.01;
			delay(10);
			TIMA1_PWM_DutyCycle(0, 0.05);
		}
	}
}