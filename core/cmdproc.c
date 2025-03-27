/* ****************************** */
/* See cmdProc.h for indications  */
/* ****************************** */
#include <stdio.h>
#include <string.h>

#include "cmdproc.h"

/* Internal variables */
/* Used as part of the UART emulation */
static unsigned char UARTRxBuffer[UART_RX_SIZE];	//Buffer de receção
static unsigned char rxBufLen = 0; 					//Comprimento do buffer de receção

static unsigned char UARTTxBuffer[UART_TX_SIZE];	//Buffer de transmissão	
static unsigned char txBufLen = 0; 					//Comprimento do buffer de receção

 
// Função para processar os comandos recebidos
int cmdProcessor(void) {
    int i;
    unsigned char sid;

    // Verifica se o comando está vazio
    if (rxBufLen == 0)
        return -1;

    // Encontra o índice do SOF (Start of Frame)
    for (i = 0; i < rxBufLen; i++) {
        if (UARTRxBuffer[i] == SOF_SYM) {
            break;
        }
    }

    // Se encontrou o SOF, começa a processar o comando
    if (i < rxBufLen) {
        switch (UARTRxBuffer[i + 1]) {
            case 'A':
                // Comando "A" para ler todos os sensores
                if (UARTRxBuffer[i + 2] != EOF_SYM) {
                    return -4;
                }

                // Processa e envia a resposta para todos os sensores
                txChar('#');
                txChar('A');

                // Temperatura simulada
                txChar('t');
                txChar('+');
                txChar('2');
                txChar('5'); // Temperatura simulada 25°C

                // Humidade simulada
                txChar('h');
                txChar('+');
                txChar('5');
                txChar('0'); // Hmidade simulada 50%

                // CO2 simulado
                txChar('c');
                txChar('+');
                txChar('4');
                txChar('0'); // CO2 simulado 4000 
				txChar('0');
				txChar('0');

                // Finaliza com EOF_SYM
                txChar('!');
                break;

            case 'P':
                // Comando "P" para ler um sensor específico (t, h, c)
                sid = UARTRxBuffer[i + 2];  // 't', 'h', ou 'c'

                if (sid == 't') {
                    // Envia o valor de temperatura simulado
                    txChar('#');
                    txChar('P');
                    txChar('t');

                    txChar('+');
                    txChar('3');
                    txChar('0'); // Temperatura simulada 30°C
                    txChar('!');


                } else if (sid == 'h') {
                    // Envia o valor de Humidade simulado
                    txChar('#');
                    txChar('P');
                    txChar('h');
                    txChar('+');
                    txChar('1');
                    txChar('0'); // Humidade simulada 100%
					txChar('0');
                    txChar('!');


                } else if (sid == 'c') {
                    // Envia o valor de CO2 simulado
                    txChar('#');
                    txChar('P');
                    txChar('c');
                    txChar('+');
                    txChar('2');
                    txChar('0'); // CO2 simulado 20000
					txChar('0');
					txChar('0');
					txChar('0');
                    txChar('!');

                } else {
                    return -2;  // Tipo de sensor inválido
                }
                break;

            case 'L':
                // Comando "L" para as últimas 20 amostras de cada sensor
                txChar('#');
                txChar('L');



                
                txChar('!');
                break;

            case 'R':
                // Comando "R" para resetar o histórico
 				txChar('#');
                txChar('R');

				
                
                txChar('!');

                break;

            default:
                return -2;  // Comando inválido
        }
    }

    // Caso não tenha encontrado o SOF ou o comando esteja mal formatado
    return -4;
}

	
		

/*
 * rxChar
 */
int rxChar(unsigned char car)
{
	/* If rxbuff not full add char to it */
	if (rxBufLen < UART_RX_SIZE) {
		UARTRxBuffer[rxBufLen] = car;
		rxBufLen += 1;
		return 0;		
	}	
	/* If cmd string full return error */
	return -1;
}

/*
 * txChar
 */
int txChar(unsigned char car)
{
	/* If rxbuff not full add char to it */
	if (txBufLen < UART_TX_SIZE) {
		UARTTxBuffer[txBufLen] = car;
		txBufLen += 1;
		return 0;		
	}	
	/* If cmd string full return error */
	return -1;
}

/*
 * resetRxBuffer
 */
void resetRxBuffer(void)
{
	rxBufLen = 0;		
	return;
}

/*
 * resetTxBuffer
 */
void resetTxBuffer(void)
{
	txBufLen = 0;		
	return;
}

/*
 * getTxBuffer
 */
void getTxBuffer(unsigned char * buf, int * len)
{
	*len = txBufLen;
	if(txBufLen > 0) {
		memcpy(buf,UARTTxBuffer,*len);
	}		
	return;
}


/* 
 * calcChecksum
 */ 
int calcChecksum(unsigned char * buf, int nbytes) {
	/* Here you are supposed to compute the modulo 256 checksum */
	/* of the first n bytes of buf. Then you should convert the */
	/* checksum to ascii (3 digitas/chars) and compare each one */
	/* of these digits/characters to the ones in the RxBuffer,	*/
	/* positions nbytes, nbytes + 1 and nbytes +2. 				*/
	
	/* That is your work to do. In this example I just assume 	*/
	/* that the checksum is always OK.							*/	
	return 1;		
}