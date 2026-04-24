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
// macros, constants, enums
enum test
{
	SERVO_TEST,
	BLE_TEST,
	OLED_TEST,
	CAMERA_TEST,
	BLE_SERVO_TEST,
};

// main prototypes
void delay(int time_ms);
void servo_test(void);
void OLED_test(void);
void bluetooth_test(void);
void camera_test(void);
void safe_startup_inits(void);
void test_suite(enum test test_todo);
void norm_diff_and_peaks(uint16_t *input, uint16_t *output, int* left_max, int* right_max);

void index_to_turn(int left_index, int right_index);
void bluetooth_servo_test(void);
void update_kp_from_uart(double* kp_pointer);
void differential(double servoPosition);
// main functions
void delay(int time_ms)
{
	unsigned long cycles_per_ms = SYSCTL_SYSCLK_getMCLK() / 1000;
	unsigned long time_to_clk_cycles = (unsigned long)time_ms * cycles_per_ms;

	for (volatile unsigned long i = time_to_clk_cycles; i > 0; i--)
		;
}

void servo_test()
{
	TIMA1_PWM_DutyCycle(0, 0.05);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.075);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.1);
	delay(50);
	TIMA1_PWM_DutyCycle(0, 0.075);
	delay(50);
}

void OLED_test()
{
	char *test_string = "Screen Works :)";
	OLED_display_clear();
	OLED_PrintLine(test_string);
}

void bluetooth_test()
{
	char current_read;
	char display[15];
	uint8_t display_size = 0;
	while (1)
	{
		if (UART1_peek_receive())
		{
			current_read = UART1_getchar();
			// end message
			if (current_read == '\r')
			{
				OLED_ClearLine();
				OLED_PrintLine(display);
				display_size = 0;
				break;
			}
			// populate string
			else
			{
				// message overflow
				if (display_size > 14)
				{
					OLED_ClearLine();
					OLED_PrintLine(display);
					display_size = 0;
				}
				display[display_size] = current_read;
				display[display_size + 1] = '\0';
				display_size++;
			}
		}
	}
}

void bluetooth_servo_test()
{
	char current_read;
	char display[15];
	uint8_t display_size = 0;
	while (1)
	{
		if (UART1_peek_receive())
		{
			current_read = UART1_getchar();
			// end message
			if (current_read == '\r')
			{
				OLED_ClearLine();
				OLED_PrintLine(display);
				display_size = 0;
				double turn = atof(display);
				TIMA1_PWM_DutyCycle(0, turn);
				break;
			}
			// populate string
			else
			{
				// message overflow
				if (display_size > 14)
				{
					OLED_ClearLine();
					OLED_PrintLine(display);
					display_size = 0;
				}
				display[display_size] = current_read;
				display[display_size + 1] = '\0';
				display_size++;
			}
		}
	}
}

void camera_test()
{
	if (Camera_isDataReady())
	{
		OLED_DisplayCameraData(Camera_getData());
	}
}

void safe_startup_inits()
{
	OLED_Init();
	UART1_init();
	ADC0_init();
	Camera_Freq_init();
	init_servo_motor(50, 0.03);
}

void test_suite(enum test test_todo)
{
	if (test_todo == SERVO_TEST)
	{
		while (1)
		{
			servo_test();
		}
	}
	if (test_todo == BLE_TEST)
	{
		while (1)
		{
			bluetooth_test();
		}
	}
	if (test_todo == OLED_TEST)
	{
		OLED_test();
	}
	if (test_todo == CAMERA_TEST)
	{
		while (1)
			camera_test();
	}
}

void norm_diff_and_peaks(uint16_t *input, uint16_t *output, int* left_max, int* right_max)
{
    output[0] = output[1] = output[2] = 0;
    output[125] = output[126] = output[127] = 0;
    int sum_m1 = input[0] + input[1] + input[2] + input[3] + input[4];
    int sum_p1 = input[2] + input[3] + input[4] + input[5] + input[6];

    
    int temp_lindex = 64;
    int temp_rindex = 64;
    uint16_t temp_rmax = 0;
    uint16_t temp_lmax = 0;
    for (int i = 3; i < 125; i++)
    {
        int norm_m1  = sum_m1 / 5;
        int norm_p1 = sum_p1 / 5;

        int32_t diff = (int32_t)norm_p1 - (int32_t)norm_m1;

        // fast abs
        int32_t mask = diff >> 31;
        diff = (diff ^ mask) - mask;

        uint16_t val = (uint16_t)diff;
        output[i] = val;
        
        //get peaks
        if(i>=5 && i<=64){
            //left peak
            if(val > temp_lmax){
                temp_lmax = val;
                temp_lindex = i;
            }
        }else if (i>64 && i<123){
            //right peak
            if(val > temp_rmax){
                temp_rmax = val;
                temp_rindex = i;
            }
        }
        // slide windows
        sum_m1  += input[i + 2] - input[i - 3];
        sum_p1 += input[i + 4] - input[i-1];
    }
    if (temp_lmax < 75){
        *left_max = -1;
    }else{
        *left_max = temp_lindex;
    }
    if (temp_rmax < 75){
        *right_max = -1;
    }else{
        *right_max = temp_rindex;
    }
}

#define speed 0.25
//worked with kp = 0.001 and kd - 0.0008
static double kp =  0.00075;   //previous 0.0006 too little, 0.001 too much0.0006
static double ki =  0.000;
static double kd =  0.000;//003;//0.0006;//0.0008;    //previous 0.00057

static double error_old = 0;
static double integral = 0;

#define kdest 64

void index_to_turn(int left_index, int right_index)
{

  //determine PID for steering
	int center = (left_index+right_index)/2;
	int offset = kdest - center;
	//integral += offset; 
	double servoPos = 0.03 + (kp * (double) offset)
										+ (ki * integral) +
										(kd * (offset - error_old));
   error_old = offset;	//+ kdest*(offset - old_offset);
	
	if (servoPos < 0.01)
	{
		servoPos = 0.01;
	}
	if (servoPos > 0.05)
	{
		servoPos = 0.05;
	}
	
	TIMA1_PWM_DutyCycle(0, servoPos);

	//determine Differential Steering
	double abs_offset;
	if (offset < 0)
	{
		abs_offset = -offset;
	}
	else
	{
		abs_offset = offset;
	}
	
	//double reduce_speed = abs_offset * TURN_SCALE;
	double dutyCycleOuter = 0.35;
	double dutyCycleInner = 0.25;
	//double dutyCycleOuter = speed + (reduce_speed);// * 0.5);  //last number closer to 1 greater turn, to zero less janky
	//double dutyCycleInner = speed - reduce_speed;
	
	//clamp to safe duty cycle
	if (dutyCycleInner > 0.475)
	{
		dutyCycleInner = 0.475;
	}
	if (dutyCycleInner < 0.25)
	{
		dutyCycleInner = 0.25;
	}
	if (dutyCycleOuter > 0.475)
	{
		dutyCycleOuter = 0.475;
	}
	if (dutyCycleOuter < 0.25)
	{
		dutyCycleOuter = 0.25;
	}
	
	if (abs_offset <= 15)   //10 too much oscillation
	{
		dc1_forward(speed);
		dc0_forward(speed);
	}
	else if (offset < 0)
	{
		dc1_forward(dutyCycleOuter);
		dc0_forward(dutyCycleInner);
	}
	else
	{
		dc1_forward(dutyCycleInner);
		dc0_forward(dutyCycleOuter);
	}
}



void update_kp_from_uart(double* kp_pointer){
	char current_read;
	char kp_val[15] = {0};
	uint8_t kp_size = 0;
	uint8_t processing = 1;
	
	while(processing){
		current_read = UART1_getchar();
		// end message
		if (current_read == '\r')
		{
			double read = strtod(kp_val, NULL);
			*kp_pointer = read;
			processing = 0;
		}
		// populate string
		else
		{
			if (kp_size < 14) {
				kp_val[kp_size++] = current_read;
			}
			kp_val[kp_size] = '\0';
		}
	}
}

int main()
{
	//SYSCTL_SYSCLK_set(SYSCLK_80MHZ);
	safe_startup_inits();	

	init_dc_motors(10000, speed);
	int last_left = 64;
	int last_right = 64;
	int left_max = 0;
	int right_max = 0;
	uint16_t array[128];
	while (1)
	{
		/*if (UART1_peek_receive())
		{
			update_kp_from_uart(&kp);
			UART1_printFloat(kp);
		}*/
		
		if (Camera_isDataReady())
		{
			uint16_t *cameraData = Camera_getData();
			norm_diff_and_peaks(cameraData, array, &left_max, &right_max);
			OLED_DisplayCameraData(cameraData);
			if (left_max == -1 && right_max == -1)
			{
					motors_forward(0.0);
			}
			else
			{
				if (left_max != -1)
				{
					last_left = left_max;
				}
				else
				{
					left_max = last_left;
				}
				if (right_max != -1)
				{
					last_right = right_max;
				}
				else
				{
					right_max = last_right;
				}
				index_to_turn(left_max, right_max);
			}
			 //char buffer1[64];
			 //snprintf(buffer1,63,"L:%d (%d) R:%d (%d)\r\n",
			 //left_max, derivData[left_max],
			 //right_max, derivData[right_max]);
			 //UART1_put(buffer1);
		}
	}
	//return 0;

}

