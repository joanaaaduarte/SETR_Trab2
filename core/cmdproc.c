/**
    \file cmdproc.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cmdproc.h"

/* Internal variables */
/* Used as part of the UART emulation */
static unsigned char UARTRxBuffer[UART_RX_SIZE];	//Buffer de receção
static unsigned char rxBufLen = 0; 					//Comprimento do buffer de receção

static unsigned char UARTTxBuffer[UART_TX_SIZE];	//Buffer de transmissão	
static unsigned char txBufLen = 0; 					//Comprimento do buffer de receção


// Histórico de temperatura últimas MAX_HISTORY = 20 amostras
static int8_t temperature_history[MAX_HISTORY];
static int temp_history_index = 0;

// Tabela de valores de temperatura
const int8_t simulated_temps[NUM_SAMPLES] = {
    	-50, -40, -35, -30, -25, -20,
		-15, -10, -5, 0, 5, 10, 15, 
		18, 20, 22, 24, 26, 28, 30,
    	32, 35, 38, 40, 45, 50, 55,
		58, 59, 60
};

int temps_index = 0;


int8_t getNextTemperature()
{
    int8_t temp = simulated_temps[temps_index];
    temps_index = (temps_index + 1) % NUM_SAMPLES;

    // Atualiza histórico
    temperature_history[temp_history_index] = temp;
    temp_history_index = (temp_history_index + 1) % MAX_HISTORY;

    return temp;
}

void resetTemperatureHistory()
{
    memset(temperature_history, 0, sizeof(temperature_history));
    temp_history_index = 0;
}
 
// Função para processar os comandos recebidos
int cmdProcessor(void) {
    int i;
    unsigned char sid;

    // Verifica se o comando está vazio
    if (rxBufLen == 0)
        return -1;

    // Encontra o índice do SOF 
    for (i = 0; i < rxBufLen; i++) {
        if (UARTRxBuffer[i] == SOF_SYM) {
            break;
        }
    }

    // Se encontrou o SOF, começa a processar o comando
    if (i < rxBufLen) {
        switch (UARTRxBuffer[i + 1]) {

            case 'A':		// Comando "A" para ler todos os sensores
                if (UARTRxBuffer[i + 2] != EOF_SYM) {
                    return -4;
                }
				
                // Processa e envia a resposta para todos os sensores
                txChar('#');
                txChar('A');
				txChar('t');
				int8_t temp = getNextTemperature();
				txChar((unsigned char)temp);

                // Finaliza com EOF_SYM
                txChar('!');
                break;

            case 'P':	// Comando "P" para ler um sensor específico (t, h, c)
                sid = UARTRxBuffer[i + 2];  // 't', 'h', ou 'c'

                if (sid == 't') {

    				int8_t temp = getNextTemperature();
    				txChar('#');
    				txChar('P');
    				txChar('t');
    				txChar((unsigned char)temp);  // valor binário
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

            case 'L':		// Comando "L" para as últimas 20 amostras de cada sensor

                txChar('#');
                txChar('L');
				txChar('t');  

				// Enviar os valores do histórico
				for (int j = 0; j < MAX_HISTORY; ++j) {
					int idx_temp = (temp_history_index + j) % MAX_HISTORY;
					txChar((unsigned char)temperature_history[idx_temp]);
				}

				txChar('!');
				break;

            case 'R':
                // Comando "R" para resetar o histórico
				resetTemperatureHistory();
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