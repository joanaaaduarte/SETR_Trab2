/**
    \file cmdproc.h
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
 */



#ifndef CMD_PROC_H_
#define CMD_PROC_H_

#include <stdint.h>

/* Some defines */
/* Other defines should be return codes of the functions */
/* E.g. #define CMD_EMPTY_STRING -1                      */
#define UART_RX_SIZE 64 	/* Maximum size of the RX buffer */ 
#define UART_TX_SIZE 64 	/* Maximum size of the TX buffer */
#define MAX_HISTORY 20      
#define NUM_SAMPLES 30      


#define RX_BUF_SIZE 128  // Definition of RX_BUF_SIZE 

typedef struct {
    char buffer[RX_BUF_SIZE];
    int count;    // Counter for the number of elements in the buffer
    int tail;     // Index for the end of the buffer
    char *data;   // Pointer to the data in the buffer
} RxBuffer;

// Initialize Rx_Buf
extern RxBuffer Rx_Buf; 

#define SOF_SYM '#'	        /* Start of Frame Symbol */
#define EOF_SYM '!'          /* End of Frame Symbol */

/* Function prototypes */

int8_t getNextTemperature();
void resetTemperatureHistory();

int8_t getNextHumidity();
void resetHumidityHistory();

int16_t getNextCO2();
void resetCO2History();


// Declare the UART_putc_Rx function
int UART_putc_Rx(char c);


void history_reset(char mode);

/* ************************************************************ */
/* Processes the chars in the RX buffer looking for commands 	*/
/* Returns:                                                     */
/*  	 0: if a valid command was found and executed           */
/* 		-1: if empty string or incomplete command found         */
/* 		-2: if an invalid command was found                     */
/* 		-3: if a CS error is detected (command not executed)    */
/* 		-4: if string format is wrong                           */
/* ************************************************************ */
int cmdProcessor(void);

/* ******************************** */
/* Adds a char to the RX buffer 	*/
/* I.e., the reception of commands 	*/
/* Returns: 				        */
/*  	 0: if success 		        */
/* 		-1: if buffer full	 	    */
/* ******************************** */
int rxChar(unsigned char car);

/* ************************************ */
/* Adds a char to the TX buffer 		*/
/* I.e., the tranmsisison of answers 	*/
/* Returns: 				        	*/
/*  	 0: if success 		        	*/
/* 		-1: if buffer full	 	    	*/
/* ************************************ */
int txChar(unsigned char car);

/* ************************* */
/* Resets the RX buffer		 */  
/* ************************* */
void resetRxBuffer(void);

/* ************************* */
/* Resets the TX buffer		 */  
/* ************************* */
void resetTxBuffer(void);

/* ************************************************ */
/* Return the data that would be sent by the sensor */  
/* ************************************************ */
void getTxBuffer(unsigned char * buf, int * len);

/* ************************************************ */
/* Computes the checksum of a given number of chars */
/* ************************************************ */ 
int calcChecksum(unsigned char * buf, int nbytes);

#endif
