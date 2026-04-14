#include <ti/devices/msp/msp.h>
#include "servo.h"
#include "sysctl.h"
#include "timers.h"
#include "DC_motor.h"
#include "switches.h"
#include "i2c.h"
#include "oled.h"
#include "uart.h"
#include "uart_extras.h"
#include "adc12.h"
#include "camera.h"
#include <stdlib.h>
#include <stdio.h>
//macros, constants, enums
enum test{
	SERVO_TEST,
	BLE_TEST,
	OLED_TEST,
	CAMERA_TEST,
	BLE_SERVO_TEST,
};

//main prototypes
void delay(int time_ms);
void servo_test(void);
void OLED_test(void);
void bluetooth_test(void);
void camera_test(void);
void safe_startup_inits(void);
void test_suite(enum test test_todo);
void norm_trace(uint16_t* data, uint16_t* norm_data);
void edge_detector(const uint16_t input[128], uint16_t output[128]);
int left_max_search(uint16_t* data);
int right_max_search(uint16_t* data);
void index_to_turn(int left_index, int right_index);
void bluetooth_servo_test(void);
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

void bluetooth_servo_test(){
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
				double turn = atof(display);
				TIMA1_PWM_DutyCycle(0,turn);
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
		if(Camera_isDataReady()){	
			OLED_DisplayCameraData(Camera_getData());
		}
}

void safe_startup_inits(){
	OLED_Init();
	UART1_init();
	UART0_init();
	ADC0_init();
	Camera_Freq_init();
	init_servo_motor(50, 0.075);
}

void test_suite(enum test test_todo){
	if(test_todo == SERVO_TEST){while(1){servo_test();}}
	if(test_todo == BLE_TEST){while(1){bluetooth_test();}}
	if(test_todo == OLED_TEST){OLED_test();}
	if(test_todo == CAMERA_TEST){while(1)camera_test();}
}

void norm_trace(uint16_t* data, uint16_t* norm_data)
{
	norm_data[0] = data[0];
	norm_data[1] = data[1];
	for (int i = 2; i < 126; i++)
	{
		norm_data[i] = data[i] + data[i-1] + data[i-2] +data[i+1] + data[i+2];
		norm_data[i] /= 5;
	}
	norm_data[126] = data[126];
	norm_data[127] = data[127];
}

void edge_detector(const uint16_t input[128], uint16_t output[128]){
    output[0] = 0;
    output[127] = 0;
    
    for(int i=1; i<127;i++){
        int32_t diff = (int32_t)input[i+1] - (int32_t)input[i-1];
        diff = abs(diff);
				
        output[i] = (uint16_t)diff;
    }
}

int left_max_search(uint16_t* data)
{
	int max = 64;
	for (int i = 5; i <= 64; i++) {
    if (data[i] > data[max]) {
        max = i;
    }
	}
	if (data[max] < 100)
	{
		return -1;
	}
	return max;
}

int right_max_search(uint16_t* data)
{
	int max = 64;
	for (int i = 64; i < 123; i++){
		if (data[i] > data[max]){
			max = i;
		}
	}
	if (data[max] < 100){
		return -1;
	}
	return max;
}

#define speed 0.30
void index_to_turn(int left_index, int right_index){
	int center = (left_index+right_index)/2;
	int offset = center - 64;
	if (offset<0){
		//turn left
		//naive impl
		if (offset > -15){
			//small left
			TIMA1_PWM_DutyCycle(0,0.075);
			motors_forward(speed);
		}else{
			//larger left
			TIMA1_PWM_DutyCycle(0, 0.1);
			motors_forward(speed);
		}
	}
	else if (offset>0){
		//turn right
		if (offset < 15){
			//small right
			TIMA1_PWM_DutyCycle(0, 0.055);
			motors_forward(speed);
		}else{
			//larger right
			TIMA1_PWM_DutyCycle(0, 0.04);
			motors_forward(speed);
		}
	}
	else{
		//go straight
		TIMA1_PWM_DutyCycle(0, 0.065);
		motors_forward(speed);
	}
}

int main(){	
	SYSCTL_SYSCLK_set(SYSCLK_80MHZ);
	safe_startup_inits();
	uint16_t normData[128];
	uint16_t derivData[128];
	init_dc_motors(10000,speed);
	int last_left = 64;
	int last_right = 64;
	while(1){
		if(Camera_isDataReady()){	
			uint16_t* cameraData = Camera_getData();
			norm_trace(cameraData, normData);
			edge_detector(normData, derivData);
			//OLED_DisplayCameraData(derivData);
			int left_max = left_max_search(derivData);
			int right_max = right_max_search(derivData);
			if (left_max == -1 && right_max == -1)
			{
				motors_forward(0.0);
			}else{
				if (left_max != -1){
					last_left = left_max;} 
				else {
					left_max = last_left;
				}
				if (right_max != -1){
					last_right = right_max;} 
				else {
					right_max = last_right;
				}
				index_to_turn(left_max,right_max);		
			}
			//char buffer1[64];
			//snprintf(buffer1,63,"L:%d (%d) R:%d (%d)\r\n", 
			//left_max, derivData[left_max], 
			//right_max, derivData[right_max]);
			//UART1_put(buffer1);
		}
	}
	return 0;
}