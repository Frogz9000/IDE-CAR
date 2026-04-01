/**
 * ******************************************************************************
 * @file    : uart.c
 * @brief   : UART module source file
 * @details : UART initialization and interaction
 * 
 * @author Samuel Pepperman
 * @date 01/28/26
 * ******************************************************************************
*/
#include <ti/devices/msp/msp.h>
#include "lab2/uart.h"
#include "sysctl.h"
#include <math.h>
#ifndef UART0_baud_rate
	#define UART0_baud_rate 9600
#endif

#ifndef UART1_baud_rate
	#define UART1_baud_rate 9600
#endif


void UART0_init(void){
	if(!(UART0->GPRCM.PWREN & 1U)){
		//reset peripheral
		UART0->GPRCM.RSTCTL |= (UART_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		UART0->GPRCM.PWREN |= (UART_PWREN_KEY_UNLOCK_W | 1U);
		int i = 0;
		while (i < 4){
			i++;//wait ~4 upclock cycles for enable to propogate
		}
	}
	IOMUX->SECCFG.PINCM[IOMUX_PINCM21] |= (0x80 | IOMUX_PINCM21_PF_UART0_TX); //set PF and connected
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM22] |= (0x80 | IOMUX_PINCM22_PF_UART0_RX); //set PF and connected
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM22] |= IOMUX_PINCM_INENA_ENABLE; //enable input for RX
	
	UART0->CLKSEL = UART_CLKSEL_BUSCLK_SEL_ENABLE; //set clock to busclk using equal to ensire no other clocks selected
	UART0->CLKDIV |= UART_CLKDIV_RATIO_DIV_BY_1;    //set clock division to 1, no division
	
	UART0->CTL0 |= UART_CTL0_ENABLE_DISABLE;  	//disable uart0
	
	UART0->CTL0 |= UART_CTL0_HSE_OVS16;					//set 16x oversampling
	UART0->CTL0 |= UART_CTL0_RXE_ENABLE;				//enable receive
	UART0->CTL0 |= UART_CTL0_TXE_ENABLE;				//enable transit
	UART0->CTL0 |= UART_CTL0_FEN_ENABLE;	      //enable FIFO
	
	double BRD = (SYSCTL_SYSCLK_getULPCLK())/(16*UART0_baud_rate);
	double int_part, frac_part;
	frac_part = modf(BRD, &int_part);
	UART0->IBRD = ((unsigned int)int_part & UART_IBRD_DIVINT_MASK);						  //integer part
	UART0->FBRD = ((unsigned int)((frac_part*64)+0.5) & UART_FBRD_DIVFRAC_MASK); //fractional part is multiplied by 64 to get in the 6 digit range, 
																																			//and 0.5 is added to minimize rounding error (TRM pg 1351)
	
	UART0->LCRH |= UART_LCRH_PEN_DISABLE;    //set 0 parity
	UART0->LCRH |= UART_LCRH_WLEN_DATABIT8;  //Set 8 bit messages
	UART0->LCRH |= UART_LCRH_STP2_DISABLE;   //Set 1 stop bit
	
	UART0->CTL0 |= UART_CTL0_ENABLE_ENABLE;  //reenable uart
}

void UART0_putchar(char ch){
	while ((UART0->STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET);//hold here until TX buffer not full
	UART0->TXDATA = (ch & UART_TXDATA_DATA_MASK); //set transmit register to character
}

char UART0_getchar(void){
	while ((UART0->STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET);//hold here until RX buffer not empty
	return (char)(UART0->RXDATA & UART_RXDATA_DATA_MASK);
}

void UART0_put(char *ptr_str){
	char* local_string = ptr_str; //local copy to prevent unintential mutation of input
	//loop through all characters in the string that are not the null terminator
	while (*local_string != '\0'){
		UART0_putchar(*local_string);
		local_string++;
	}
}

enum UART_RECEIVE_STATE UART0_peek_receive(void){
	if((UART0->STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET){//if receive fifo empty
		return WAITING;
	}
	return READY;
}

void UART1_init(void){
	if(!(UART1->GPRCM.PWREN & 1U)){
		//reset peripheral
		UART1->GPRCM.RSTCTL |= (UART_RSTCTL_KEY_UNLOCK_W | 1U);
		//enable peripheral
		UART1->GPRCM.PWREN |= (UART_PWREN_KEY_UNLOCK_W | 1U);
		int i = 0;
		while (i < 4){
			i++;//wait ~4 upclock cycles for enable to propogate
		}
	}
	IOMUX->SECCFG.PINCM[IOMUX_PINCM19] |= (0x80 | IOMUX_PINCM19_PF_UART1_TX); //set PF and connected
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM20] |= (0x80 | IOMUX_PINCM20_PF_UART1_RX); //set PF and connected
	
	IOMUX->SECCFG.PINCM[IOMUX_PINCM20] |= IOMUX_PINCM_INENA_ENABLE; //enable input for RX
	
	UART1->CLKSEL = UART_CLKSEL_BUSCLK_SEL_ENABLE; //set clock to busclk using equal to ensire no other clocks selected
	UART1->CLKDIV |= UART_CLKDIV_RATIO_DIV_BY_1;    //set clock division to 1, no division
	
	UART1->CTL0 |= UART_CTL0_ENABLE_DISABLE;  	//disable uart0
	
	UART1->CTL0 |= UART_CTL0_HSE_OVS16;					//set 16x oversampling
	UART1->CTL0 |= UART_CTL0_RXE_ENABLE;				//enable receive
	UART1->CTL0 |= UART_CTL0_TXE_ENABLE;				//enable transit
	UART1->CTL0 |= UART_CTL0_FEN_ENABLE;	      //enable FIFO
	
	double BRD = (SYSCTL_SYSCLK_getULPCLK())/(16*UART1_baud_rate);
	double int_part, frac_part;
	frac_part = modf(BRD, &int_part);
	UART1->IBRD = ((unsigned int)int_part & UART_IBRD_DIVINT_MASK);						  //integer part
	UART1->FBRD = ((unsigned int)((frac_part*64)+0.5) & UART_FBRD_DIVFRAC_MASK); //fractional part is multiplied by 64 to get in the 6 digit range, 
																																			//and 0.5 is added to minimize rounding error (TRM pg 1351)
	
	UART1->LCRH |= UART_LCRH_PEN_DISABLE;    //set 0 parity
	UART1->LCRH |= UART_LCRH_WLEN_DATABIT8;  //Set 8 bit messages
	UART1->LCRH |= UART_LCRH_STP2_DISABLE;   //Set 1 stop bit
	
	UART1->CTL0 |= UART_CTL0_ENABLE_ENABLE;  //reenable uart
}

void UART1_putchar(char ch){
	while ((UART1->STAT & UART_STAT_TXFF_MASK) == UART_STAT_TXFF_SET);//hold here until TX buffer not full
	UART1->TXDATA = (ch & UART_TXDATA_DATA_MASK); //set transmit register to character
}

char UART1_getchar(void){
	while ((UART1->STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET);//hold here until RX buffer not empty
	return (char)(UART1->RXDATA & UART_RXDATA_DATA_MASK);
}

void UART1_put(char *ptr_str){
	char* local_string = ptr_str; //local copy to prevent unintential mutation of input
	//loop through all characters in the string that are not the null terminator
	while (*local_string != '\0'){
		UART1_putchar(*local_string);
		local_string++;
	}
}

enum UART_RECEIVE_STATE UART1_peek_receive(void){
	if((UART1->STAT & UART_STAT_RXFE_MASK) == UART_STAT_RXFE_SET){//if receive fifo empty
		return WAITING;
	}
	return READY;
}
