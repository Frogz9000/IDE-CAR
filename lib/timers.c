/**
 * ******************************************************************************
 * @file    : timers.c
 * @details : Timer initializations
 * 
 * @author Samuel Pepperman
 * @date 2/11/2026
 * ******************************************************************************
*/
#include <ti/devices/msp/msp.h>
#include "lab6/timers.h"
#include "arm_compat.h"
#include "core_cm0plus.h"
#include "sysctl.h"


static uint16_t global_period_tim0 = 0;
static uint16_t global_period_tim1 = 0;
/**
 * @brief Timer G0 module initialization. General purpose timer
 * @note Timer G0 is in Power Domain 0. Check page 3 of the Data Sheet
*/
void TIMG0_init(uint32_t period, uint32_t prescaler){
	//enable peripheral if not enabled
	if(!(TIMG0->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMG0->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMG0->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	//TIMCLK CONFIGURATION
	//Power Domain 0 uses ULPCLK as BUSCLK = 40MHz
	TIMG0->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	//set CLKDIV to 8
	TIMG0->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;
	//set prescaler
	TIMG0->COMMONREGS.CPS = (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMG0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMG0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMG0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMG0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMG0->COUNTERREGS.LOAD = period;
	
	//INTERUPT HANDLING
	//disable interupts
	__disable_irq();
	//clear zero event
	TIMG0->CPU_INT.ICLR = GPTIMER_GEN_EVENT1_ICLR_Z_CLR;
	//enable zero event
	TIMG0->CPU_INT.IMASK = GPTIMER_GEN_EVENT0_IMASK_Z_SET;
	//enable the timer
	TIMG0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
	//register NVIC
	NVIC_EnableIRQ(TIMG0_INT_IRQn);
	//enable interrupts
	__enable_irq();
}


/**
 * @brief Timer G6 module initialization. General purpose timer
*/
void TIMG6_init(uint32_t period, uint32_t prescaler){
	//enable peripheral if not enabled
	if(!(TIMG6->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMG6->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMG6->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	//TIMCLK CONFIGURATION
	//Power Domain 1 uses MCLK as BUSCLK = 80MHz
	TIMG6->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	//set CLKDIV to 8
	TIMG6->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;
	//set prescaler
	TIMG6->COMMONREGS.CPS = (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMG6->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMG6->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMG6->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMG6->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMG6->COUNTERREGS.LOAD = period;
	
	//INTERUPT HANDLING
	//disable interupts
	__disable_irq();
	//clear zero event
	TIMG6->CPU_INT.ICLR = GPTIMER_GEN_EVENT1_ICLR_Z_CLR;
	//enable zero event
	TIMG6->CPU_INT.IMASK = GPTIMER_GEN_EVENT0_IMASK_Z_SET;
	//enable the timer
	TIMG6->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
	//register NVIC
	NVIC_EnableIRQ(TIMG6_INT_IRQn);
	//enable interrupts
	__enable_irq();
}


/**
 * @brief Timer G12 module initialization. General purpose timer
 * @note Timer G12 has no prescaler
*/
void TIMG12_init(uint32_t period){
	//enable peripheral if not enabled
	if(!(TIMG12->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMG12->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMG12->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	//TIMCLK CONFIGURATION
	//uses MCLK as BUSCLK = 80MHz
	TIMG12->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	//set CLKDIV to 8
	TIMG12->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;
	//Enable TIMCLK
	TIMG12->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMG12->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMG12->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMG12->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMG12->COUNTERREGS.LOAD = period;
	
	//INTERUPT HANDLING
	//disable interupts
	__disable_irq();
	//clear zero event
	TIMG12->CPU_INT.ICLR = GPTIMER_GEN_EVENT1_ICLR_Z_CLR;
	//enable zero event
	TIMG12->CPU_INT.IMASK = GPTIMER_GEN_EVENT0_IMASK_Z_SET;
	//enable the timer
	TIMG12->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
	//register NVIC
	NVIC_EnableIRQ(TIMG12_INT_IRQn);
	//enable interrupts
	__enable_irq();
}

/**
 * @brief Timer A0 module PWM initialization
 * @param[in] pin - Timer PWM output pin / channel
 * @param[in] period - Timer load value
 * @param[in] prescaler - Timer prescale value
 * @param[in] percenetDutyCycle - PWM duty cycle positive
 * @note Store period to be able to adjust duty cycle percentage later
*/
void TIMA0_PWM_init(uint8_t pin, uint32_t period, uint32_t prescaler, double percentDutyCycle)
{
		//enable peripheral if not enabled
	if(!(TIMA0->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMA0->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMA0->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	
	/*//enable GPIO B Peripheral
	if(!(GPIOB->GPRCM.PWREN & GPIO_PWREN_ENABLE_ENABLE)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETASSERT_ASSERT);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
	}*/
	//TIMCLK CONFIGURATION
	//uses MCLK as BUSCLK = 80MHz
	TIMA0->CLKSEL |= GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	//set CLKDIV to 8
	TIMA0->CLKDIV |= GPTIMER_CLKDIV_RATIO_DIV_BY_1;
	//set prescaler
	TIMA0->COMMONREGS.CPS |= (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMA0->COMMONREGS.CCLKCTL |= GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMA0->COUNTERREGS.LOAD = period;
	
	
	//CONFIGURATION BASED OF CHANNEL
	if (pin == 0)//configure GPIOB PB8 channel 0 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM25]|= (0x80 | IOMUX_PINCM25_PF_TIMA0_CCP0 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_01[0] = (uint32_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_01[0] |= (0 & GPTIMER_CCCTL_01_COC_MASK);
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP0_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_01[0] |= (GPTIMER_CCACT_01_LACT_CCP_HIGH & GPTIMER_CCACT_01_LACT_MASK)
											| (GPTIMER_CCACT_01_CDACT_CCP_LOW & GPTIMER_CCACT_01_CDACT_MASK); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_01[0] = ((GPTIMER_OCTL_01_CCPO_FUNCVAL & GPTIMER_OCTL_01_CCPOINV_MASK) |
							(GPTIMER_IFCTL_01_INV_NOINVERT & GPTIMER_IFCTL_01_INV_MASK) 
						| (GPTIMER_ODIS_C0CCP1_CCP_OUTPUT_LOW & GPTIMER_ODIS_C0CCP1_MASK )); //add non inverting and low and and with mask

	}
	
	if (pin == 1)//configure GPIOB PB12 channel 1 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM29]|= (0x80 | IOMUX_PINCM29_PF_TIMA0_CCP1 );  
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_01[1] |= (period * (1 - (uint32_t) percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_01[1] = 0;
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD = GPTIMER_CCPD_C0CCP1_OUTPUT ;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_01[1] = (GPTIMER_CCACT_01_LACT_CCP_HIGH | GPTIMER_CCACT_01_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_01[1] |= GPTIMER_OCTL_01_CCPO_FUNCVAL;
	}
		
	if (pin == 2)//configure GPIOB PB17 channel 2 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM43]|= (0x80 | IOMUX_PINCM43_PF_TIMA0_CCP2 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_23[0] |= (period * (1 - (uint32_t) percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_23[0] = 0;
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD = GPTIMER_CCPD_C0CCP2_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_23[0] = (GPTIMER_CCACT_23_LACT_CCP_HIGH | GPTIMER_CCACT_23_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_23[0] |= GPTIMER_OCTL_23_CCPO_FUNCVAL;
	}	
	
	if (pin == 3)//configure GPIOB PB13 channel 3 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM30]|= (0x80 | IOMUX_PINCM30_PF_TIMA0_CCP3 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_23[1] |=  (uint32_t)(period * (1 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_23[1] = 0;
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD = GPTIMER_CCPD_C0CCP3_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_23[1] = (GPTIMER_CCACT_23_LACT_CCP_HIGH | GPTIMER_CCACT_23_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_23[1] |= GPTIMER_OCTL_23_CCPO_FUNCVAL;
	}
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}



void TIMA0_PWM_freq_init(uint8_t pin, uint32_t frequency, double percentDutyCycle){
		//enable peripheral if not enabled
	if(!(TIMA0->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMA0->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMA0->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	
	/*//enable GPIO B Peripheral
	if(!(GPIOB->GPRCM.PWREN & GPIO_PWREN_ENABLE_ENABLE)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETASSERT_ASSERT);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
	}*/
	//TIMCLK CONFIGURATION
	//uses MCLK as BUSCLK = 80MHz
	TIMA0->CLKSEL |= GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	
	uint32_t sys_clk = SYSCTL_SYSCLK_getMCLK();//USE THIS TO DETERMINE FREQ
	uint32_t period = sys_clk/frequency;
	global_period_tim0 = (uint16_t)period;
	uint32_t prescaler = 0;

	//set CLKDIV to 1
	TIMA0->CLKDIV |= GPTIMER_CLKDIV_RATIO_DIV_BY_1;
	//set prescaler
	TIMA0->COMMONREGS.CPS |= (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMA0->COMMONREGS.CCLKCTL |= GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMA0->COUNTERREGS.LOAD = period;
	
	
	//CONFIGURATION BASED OF CHANNEL
	if (pin == 0)//configure GPIOB PB8 channel 0 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM25]|= (0x80 | IOMUX_PINCM25_PF_TIMA0_CCP0 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_01[0] = (uint16_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_01[0] &= ~(GPTIMER_CCCTL_01_COC_MASK);
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP0_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_01[0] |= (GPTIMER_CCACT_01_LACT_CCP_HIGH | GPTIMER_CCACT_01_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_01[0] = (GPTIMER_OCTL_01_CCPO_FUNCVAL);
		TIMA0->COUNTERREGS.IFCTL_01[0] = GPTIMER_IFCTL_01_INV_NOINVERT; 
		TIMA0->COMMONREGS.ODIS =  GPTIMER_ODIS_C0CCP1_CCP_OUTPUT_LOW;

	}
	
	if (pin == 1)//configure GPIOB PB12 channel 1 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM29]|= (0x80 | IOMUX_PINCM29_PF_TIMA0_CCP1 );  
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_01[1] = (uint16_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_01[1] &= ~(GPTIMER_CCCTL_01_COC_MASK);
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP1_OUTPUT ;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_01[1] |= (GPTIMER_CCACT_01_LACT_CCP_HIGH | GPTIMER_CCACT_01_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_01[1] = (GPTIMER_OCTL_01_CCPO_FUNCVAL);
		TIMA0->COUNTERREGS.IFCTL_01[1] = GPTIMER_IFCTL_01_INV_NOINVERT; 
		TIMA0->COMMONREGS.ODIS =  GPTIMER_ODIS_C0CCP0_CCP_OUTPUT_LOW;
	}	
	if (pin == 2)//configure GPIOB PB17 channel 2 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM43]|= (0x80 | IOMUX_PINCM43_PF_TIMA0_CCP2 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_23[0] = (uint16_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_23[0] &= ~(GPTIMER_CCCTL_23_COC_MASK);
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP2_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_23[0] |= (GPTIMER_CCACT_23_LACT_CCP_HIGH | GPTIMER_CCACT_23_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_23[0] = GPTIMER_OCTL_23_CCPO_FUNCVAL;
		TIMA0->COUNTERREGS.IFCTL_23[0] = GPTIMER_IFCTL_23_INV_NOINVERT; 
		TIMA0->COMMONREGS.ODIS =  GPTIMER_ODIS_C0CCP3_CCP_OUTPUT_LOW;
	}	
	
	if (pin == 3)//configure GPIOB PB13 channel 3 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM30]|= (0x80 | IOMUX_PINCM30_PF_TIMA0_CCP3 );
		//set value to configure duty cycle
		TIMA0->COUNTERREGS.CC_23[1] |=  (uint16_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA0->COUNTERREGS.CCCTL_23[1] &= ~(GPTIMER_CCCTL_23_COC_MASK);
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA0->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP3_OUTPUT;
		//CCP output action settings”
		TIMA0->COUNTERREGS.CCACT_23[1] |= (GPTIMER_CCACT_23_LACT_CCP_HIGH | GPTIMER_CCACT_23_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA0->COUNTERREGS.OCTL_23[1] = GPTIMER_OCTL_23_CCPO_FUNCVAL;
		TIMA0->COUNTERREGS.IFCTL_23[1] = GPTIMER_IFCTL_23_INV_NOINVERT; 
		TIMA0->COMMONREGS.ODIS =  GPTIMER_ODIS_C0CCP2_CCP_OUTPUT_LOW;
	}
	TIMA0->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}

void TIMA1_PWM_freq_init(uint8_t pin, uint32_t frequency, double percentDutyCycle){
		//enable peripheral if not enabled
	if(!(TIMA1->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMA1->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMA1->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	
	//TIMCLK CONFIGURATION
	//uses MCLK as BUSCLK = 80MHz
	TIMA1->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	uint32_t sys_clk = SYSCTL_SYSCLK_getMCLK();//USE THIS TO DETERMINE FREQ
	uint32_t period = sys_clk/frequency;
	global_period_tim1 = (uint16_t)period;
	uint32_t prescaler = 0;
	
	//set CLKDIV to 1
	TIMA1->CLKDIV |= GPTIMER_CLKDIV_RATIO_DIV_BY_1;
	//set prescaler
	TIMA1->COMMONREGS.CPS |= (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMA1->COMMONREGS.CCLKCTL |= GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMA1->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMA1->COUNTERREGS.LOAD = period;
	
	
	//CONFIGURATION BASED OF CHANNEL
	if (pin == 0)//configure GPIOB PB4 channel 0 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM17]|= (0x80 | IOMUX_PINCM17_PF_TIMA1_CCP0 );
		//set value to configure duty cycle
		TIMA1->COUNTERREGS.CC_01[0] = (uint16_t)((double)period * (double)(1.0 - percentDutyCycle));
		//set COC for compare mode
		TIMA1->COUNTERREGS.CCCTL_01[0] &= ~(GPTIMER_CCCTL_01_COC_MASK); //&=
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA1->COMMONREGS.CCPD |= GPTIMER_CCPD_C0CCP0_OUTPUT;
		//CCP output action settings”
		TIMA1->COUNTERREGS.CCACT_01[0] |= (GPTIMER_CCACT_01_LACT_CCP_HIGH | GPTIMER_CCACT_01_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA1->COUNTERREGS.OCTL_01[0] |= GPTIMER_OCTL_01_CCPO_FUNCVAL;
		TIMA1->COUNTERREGS.IFCTL_01[0] = GPTIMER_IFCTL_01_INV_NOINVERT; 
	}
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}

/**
 * @brief Timer A1 module PWM initialization
 * @param[in] pin - Timer PWM output pin / channel
 * @param[in] period - Timer load value
 * @param[in] prescaler - Timer prescale value
 * @param[in] percenetDutyCycle - PWM duty cycle positive
 * @note Store period to be able to adjust duty cycle percentage later
*/
void TIMA1_PWM_init(uint8_t pin, uint32_t period, uint32_t prescaler, double percentDutyCycle)
{
	//enable peripheral if not enabled
	if(!(TIMA1->GPRCM.PWREN & GPTIMER_PWREN_ENABLE_ENABLE)){
			//assert reset
			TIMA1->GPRCM.RSTCTL |= (GPTIMER_RSTCTL_KEY_UNLOCK_W | GPTIMER_RSTCTL_RESETASSERT_ASSERT);
			//enable
			TIMA1->GPRCM.PWREN |= (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);
	}
	
	//enable GPIO B Peripheral
	if(!(GPIOB->GPRCM.PWREN & GPIO_PWREN_ENABLE_ENABLE)){
		//reset peripheral
		GPIOB->GPRCM.RSTCTL |= (GPIO_RSTCTL_KEY_UNLOCK_W | GPIO_RSTCTL_RESETASSERT_ASSERT);
		//enable peripheral
		GPIOB->GPRCM.PWREN |= (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
	}
	
	//TIMCLK CONFIGURATION
	//uses MCLK as BUSCLK = 80MHz
	TIMA1->CLKSEL = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE;
	//set CLKDIV to 8
	TIMA1->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;
	//set prescaler
	TIMA1->COMMONREGS.CPS = (prescaler & GPTIMER_CPS_PCNT_MASK);
	//Enable TIMCLK
	TIMA1->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;
	
	//COUNTING MODE CONTROL
	//set down counting
	TIMA1->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CM_DOWN;
	//set perdiodic
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_REPEAT_REPEAT_1;
	//counter value after enable
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_CVAE_LDVAL;
	//Set LOAD to period
	TIMA1->COUNTERREGS.LOAD = period;
	
	
	//CONFIGURATION BASED OF CHANNEL
	if (pin == 0)//configure GPIOB PB4 channel 0 
	{
		IOMUX->SECCFG.PINCM[IOMUX_PINCM17]|= (0x80 | IOMUX_PINCM25_PF_TIMA0_CCP0 );
		//set value to configure duty cycle
		TIMA1->COUNTERREGS.CC_01[0] |= (uint32_t)(period * (1 - percentDutyCycle));
		//set COC for compare mode
		TIMA1->COUNTERREGS.CCCTL_01[0] = 0;
		//Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
		//TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1
		TIMA1->COMMONREGS.CCPD = GPTIMER_CCPD_C0CCP0_OUTPUT;
		//CCP output action settings”
		TIMA1->COUNTERREGS.CCACT_01[0] = (GPTIMER_CCACT_01_LACT_CCP_HIGH | GPTIMER_CCACT_01_CDACT_CCP_LOW); 
    //set CCPO = 0 to select the signal generator output.
		TIMA1->COUNTERREGS.OCTL_01[0] |= GPTIMER_OCTL_01_CCPO_FUNCVAL;

	}
	TIMA1->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}


/**
 * @brief Change PWM duty cycle for all Timer A0 channels
 * @param[in] pin - Timer PWM output pin / channel
 * @param[in] percentDutyCycle - Duty cycle to change to
*/
void TIMA0_PWM_DutyCycle(uint8_t pin, double percentDutyCycle)
{
	 if (pin == 0)
	 {
		 TIMA0->COUNTERREGS.CC_01[0] = (uint16_t)((double)global_period_tim0 * (double)(1.0 - percentDutyCycle));
	 }
	 if (pin == 1)
	 {
		 TIMA0->COUNTERREGS.CC_01[1] = (uint16_t)((double)global_period_tim0 * (double)(1.0 - percentDutyCycle));
	 }
	 if (pin == 2)
	 {
		 TIMA0->COUNTERREGS.CC_23[0] = (uint16_t)((double)global_period_tim0 * (double)(1.0 - percentDutyCycle));
	 }
	 if (pin == 3)
	 {
		 TIMA0->COUNTERREGS.CC_23[1] = (uint16_t)((double)global_period_tim0 * (double)(1.0 - percentDutyCycle));
	 }
 }

/**
 * @brief Change PWM duty cycle for all Timer A1 channels
 * @param[in] pin - Timer PWM output pin / channel
 * @param[in] percentDutyCycle - Duty cycle to change to
*/
void TIMA1_PWM_DutyCycle(uint8_t pin, double percentDutyCycle)
{
	 if (pin == 0)
	 {
		 TIMA1->COUNTERREGS.CC_01[0] = (uint16_t)((double)global_period_tim1 * (double)(1.0 - percentDutyCycle));
	 }
}

