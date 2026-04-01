#include <ti/devices/msp/msp.h>
#include "lab5/camera.h"
#include "lab5/adc12.h"
#include "lab5/timers.h"
static uint8_t cameraData_complete = 0;//0 = not ready, 1 = ready
static uint16_t cameraData[128];
static unsigned pixel_counter = 0;
//timer phase defines as 0.5ms then scaled by this macro scale factor(15 = 7.5ms)
#define integration_time_mult 15


/**
 * @brief Initialize camera associated components
*/
void Camera_init(void){
	//enable GPIO A Peripheral
	if(!(GPIOA->GPRCM.PWREN & GPIO_PWREN_ENABLE_ENABLE)){
		//reset peripheral
		GPIOA->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETASSERT_ASSERT);
		//enable peripheral
		GPIOA->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
	}
	
	//configure GPIOA PA28 SI 
	IOMUX->SECCFG.PINCM[IOMUX_PINCM3]|= (0x80 | 0x01 );
	GPIOA->DOESET31_0 |= GPIO_DOESET31_0_DIO28_SET; 
	
	//turn off
	GPIOA->DOUTCLR31_0 |= GPIO_DOUTCLR31_0_DIO28_CLR;   
		
	//configure GPIOA PA12 CLK
	IOMUX->SECCFG.PINCM[IOMUX_PINCM34]|= (0x80 | 0x01 );
	GPIOA->DOESET31_0 |= GPIO_DOESET31_0_DIO12_SET; 
	
	//turn off
	GPIOA->DOUTCLR31_0 |= GPIO_DOUTCLR31_0_DIO12_CLR;  
	//ADC Init
	ADC0_init();
	//TIMG0 init at 100kHz. 40Mhz, clk div 8, prescale 50 = (40*10^6)/8*50 = 100kHz clk 
	TIMG0_init(1,49);
	//Disable TIMG0
	TIMG0->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);
	//TIMG6 init at integration time
	TIMG6_init(2500*integration_time_mult,0);//80Mhz Busclk/(8) = 10MHz.
}


/**
 * @brief Checks whether camer data is ready to retrieve
 * @note Make sure to check all data is available
 * @return True(1)/False(0) if camera data is ready
*/
uint8_t Camera_isDataReady(void){
	return cameraData_complete;
}


/**
 * @brief Retrieves pointer to camera data array
 * @return Pointer to global data array stored locally in this file
*/
uint16_t* Camera_getData(){
	return cameraData;
}

void mark_data_processed(void){
	cameraData_complete = 0;
	return;
}

//ISR DEF FOR CAMERA MODULE
void TIMG6_IRQHandler(void){
	//print_and_reset_ms_count(); //UNCOMMENT TO VERIFY TIMG6 TIME
	//ensure clk timer disabled
	if(TIMG0->COUNTERREGS.CTRCTL & GPTIMER_CTRCTL_EN_ENABLED){
		TIMG0->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);//disable timer
	}
	//check if there is no data to process
	if (cameraData_complete == 1){
		cameraData_complete = 0;
	//set SI high on clk falling edge
	GPIOA->DOUTSET31_0 |= GPIO_DOUTSET31_0_DIO28_SET;
	//set clk high with SI high for 1 rising edge
	GPIOA->DOUTSET31_0 |= GPIO_DOUTSET31_0_DIO12_SET;
	//Set SI off
	GPIOA->DOUTCLR31_0 |= GPIO_DOUTCLR31_0_DIO28_CLR;
	//Set clk falling edge
	GPIOA->DOUTCLR31_0 |= GPIO_DOUTCLR31_0_DIO12_CLR;
	}
	//enable clk timer
	TIMG0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}


void TIMG0_IRQHandler(void){
	//pulse clk pin
	GPIOA->DOUTSET31_0 |= GPIO_DOUTSET31_0_DIO12_SET;
	GPIOA->DOUTCLR31_0 |= GPIO_DOUTCLR31_0_DIO12_CLR;
	
	//read ADC0
	uint32_t read_val = ADC0_getVal();
	//Populate cameraData and incriment index
	cameraData[pixel_counter] = (uint16_t)read_val;
	pixel_counter++;
	//If data is full
	if (pixel_counter >= 128){
		cameraData_complete = 1;
		//disable clk timer
		TIMG0->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);
		//reset index
		pixel_counter = 0;
	}
}

