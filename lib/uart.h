/**
 * ******************************************************************************
 * @file    : uart.h
 * @brief   : UART module header file
 * @details : UART initialization and interaction
 * 
 * @author 
 * @date 
 * ******************************************************************************
*/

#ifndef _UART_H_
#define _UART_H_

/**
 * @brief Initialize UART0
*/
void UART0_init(void);


/**
 * @brief Put a character over UART0
 * @param[in] ch - Character to print
*/
void UART0_putchar(char ch);


/**
 * @brief Retrieve a single character from UART0
*/
char UART0_getchar(void);


/**
 * @brief Send a full character string over UART0
 * @param[in] ptr_str - Pointer to the string to print
*/
void UART0_put(char *ptr_str);

enum UART_RECEIVE_STATE{
	WAITING = 0,
	READY = 1
};

//Non-blocking method of checking if ready to receive data
enum UART_RECEIVE_STATE UART0_peek_receive(void);



/**
 * @brief Initialize UART0
*/
void UART1_init(void);


/**
 * @brief Put a character over UART0
 * @param[in] ch - Character to print
*/
void UART1_putchar(char ch);


/**
 * @brief Retrieve a single character from UART0
*/
char UART1_getchar(void);


/**
 * @brief Send a full character string over UART0
 * @param[in] ptr_str - Pointer to the string to print
*/
void UART1_put(char *ptr_str);

enum UART_RECEIVE_STATE UART1_peek_receive(void);
#endif // _UART_H_
