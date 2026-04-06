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
void norm_trace(uint16_t* data, uint16_t* norm_data);
void edge_detector(const uint16_t input[128], uint16_t output[128]);
int left_max_search(uint16_t* data);
int right_max_search(uint16_t* data);

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
	UART0_init();
	ADC0_init();
	Camera_Freq_init();
	init_servo_motor(50, 0.075);
}

void test_suite(enum test test_todo){
	if(test_todo == SERVO_TEST){while(1){servo_test();}}
	if(test_todo == BLE_TEST){while(1){bluetooth_test();}}
	if(test_todo == OLED_TEST){OLED_test();}
	if(test_todo == CAMERA_TEST){camera_test();}
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

int main(){	
	safe_startup_inits();
	uint16_t normData[128];
	uint16_t derivData[128];
	//init_dc_motors(10000,0.2);
	while(1){
		if(Camera_isDataReady()){	
			uint16_t* cameraData = Camera_getData();

			norm_trace(cameraData, normData);


			edge_detector(normData, derivData);

			OLED_DisplayCameraData(derivData);
			int left_max = left_max_search(derivData);
			int right_max = right_max_search(derivData);
			
			char buffer1[32];
			char buffer2[32];
			snprintf(buffer1, 16, "left max: %d\n\r", left_max);
			snprintf(buffer2, 16, "right max: %d\n\r", right_max);
			UART1_put(buffer1);
			UART1_put(buffer2);

			
		}
			//apply edge filter
		

			//if edges, try to center
			
			//if no edges go straight
			
			//if only left edge(edge < 64) turn right
			
			//if only right edge(edge > 64) turn left
		}
	
	
	
	//test_suite(CAMERA_TEST);
	//init_dc_motors(10000,0.2);
	//dc0_forward(0.40);
	//dc1_forward(0.40);
	//test_suite(SERVO_TEST);
	
	return 0;
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
	int i;
	int max = 64;
	for (i = 64; i > 0; i--)
	{
		if (data[i] > data[max])
		{
			max = i;
		}
		if (data[i - 1] < data[i])
		{
			break;
		}
	}
	return max;
}

int right_max_search(uint16_t* data)
{
	int i;
	int max = 0;
	for (i = 64; i < 128; i++)
	{
		if (data[i] > data[max])
		{
			max = i;
		}
		if (data[i + 1] < data[i])
		{
			break;
		}
	}
	return max;
}
