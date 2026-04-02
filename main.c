#include <ti/devices/msp/msp.h>
#include "servo.h"
#include "sysctl.h"
#include "timers.h"
#include "DC_motor.h"
#include "switches.h"
#include "i2c.h"
#include "oled.h"
#include "uart.h"
#include "adc12.h"
#include "camera.h"
//macros, constants, enums
enum test{
	SERVO_TEST,
	BLE_TEST,
	OLED_TEST,
	CAMERA_TEST
};

//main prototypes
void delay(int time_ms);
void servo_test(void);
void OLED_test(void);
void bluetooth_test(void);
void camera_test(void);
void safe_startup_inits(void);
void test_suite(enum test test_todo);



//main functions
void delay(int time_ms){
	unsigned long cycles_per_ms = SYSCTL_SYSCLK_getMCLK()/1000;
	unsigned long time_to_clk_cycles = (unsigned long)time_ms*cycles_per_ms;
	
	for (volatile unsigned long i=time_to_clk_cycles; i>0; i--);
}

void servo_test(){
	TIMA1_PWM_DutyCycle(0, 0.05);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.075);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.1);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.075);
	delay(50);
}

void OLED_test(){
	char* test_string = "Screen Works :)";
	OLED_display_clear();
	OLED_PrintLine(test_string);
}

void bluetooth_test(){
	char current_read;
	char display[15];
	uint8_t display_size = 0;
	while(1){
		if(UART1_peek_receive()){
			current_read = UART1_getchar();
			//end message
			if (current_read == '\r'){
				OLED_ClearLine();
				OLED_PrintLine(display);
				display_size = 0;		
				break;				
			}
			//populate string
			else{
				//message overflow
				if(display_size>14){
					OLED_ClearLine();
					OLED_PrintLine(display);
					display_size = 0;
					}
				display[display_size] = current_read;
				display[display_size+1] = '\0';
				display_size++;
			}
		}
	}
}


void camera_test(){
	while(1){
		if(Camera_isDataReady()){	
			OLED_DisplayCameraData(Camera_getData());
		}
	}
}

void safe_startup_inits(){
	OLED_Init();
	UART1_init();
	ADC0_init();
	Camera_init();
	init_servo_motor(50, 0.075);
}

void test_suite(enum test test_todo){
	if(test_todo == SERVO_TEST){while(1){servo_test();}}
	if(test_todo == BLE_TEST){while(1){bluetooth_test();}}
	if(test_todo == OLED_TEST){OLED_test();}
	if(test_todo == CAMERA_TEST){camera_test();}
}

int main(){	
	safe_startup_inits();
	test_suite(CAMERA_TEST);
	//init_dc_motor0(10000,0.20);
	//init_dc_motor1(10000,0.20)
	return 0;
}
