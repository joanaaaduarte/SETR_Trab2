/**
    \file main.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
 */


#include <stdio.h>
#include <string.h>
#include "cmdproc.h"
#include <stdint.h>


int main(void) 
{
    int i, len;
    unsigned char ans[30];
	unsigned char inputBuffer[10];

    printf("\n Smart Sensor interface emulation \n");
    printf(" \t - simple illustration of interface and use \n\n\r");

	/*************************************************** */
	/* Teste 1: Envia o comando "A" para ler todos os sensores */
    printf("Test 1 - Enviar comando 'A' para ler todos os sensores (#At-50h+50c+00400!) \n");

    /* Inicializa os buffers da UART */
    resetTxBuffer();
    resetRxBuffer();

	// Envia o comando "A"

	rxChar('#');
	rxChar('A');
	rxChar('!');
	

	snprintf((char *)inputBuffer, sizeof(inputBuffer), "#A");
	int checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
	snprintf((char *)&inputBuffer[2], sizeof(inputBuffer) - 2, "%03d!", checksum);

	// Enviar caracteres individualmente para rxChar()
	for (i = 0; i < strlen((char *)inputBuffer); i++) {
   		rxChar(inputBuffer[i]);
	}

	// Processar comando
	cmdProcessor();
	getTxBuffer(ans, &len);

	// Imprimir resposta recebida
	printf("Resposta recebida ASCII somado: ");
	int sum = 0;
	for (i = 1; i < len - 1; i++) {
  	  sum += ans[i];
	}
	printf("\n%d\n", sum);

	/**************************************************************************************/

	/* Teste 2: Envia o comando "P" para ler o sensor de temperatura */
    printf("\nTest 2 - Enviar comando 'P' para ler sensor de temperatura (#Pt-40!)\n");

    resetTxBuffer();
    resetRxBuffer();

    // Envia o comando "P" com sensor 't'

    rxChar('#');
    rxChar('P');
    rxChar('t');
    rxChar('!');


    snprintf((char *)inputBuffer, sizeof(inputBuffer), "#Pt");
    checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
    snprintf((char *)inputBuffer + 2, sizeof(inputBuffer) - 2, "%03d!", checksum);

	for (i = 0; i < strlen((char *)inputBuffer); i++) {
        rxChar(inputBuffer[i]);
		printf("Enviado para rxChar: %c\n", inputBuffer[i]); 
    }

	cmdProcessor();
    getTxBuffer(ans, &len);

	sum = 0;
    printf("Resposta recebida ASCII somado: ");
    for (i = 1; i < len-1; i++) {
        sum += ans[i];
    }
	printf("%d", sum);
    printf("\n");

	/************************************************************************* */
	/* Teste 2.1: Envia o comando "P" para ler o sensor de humidade */
	printf("\nTest 2.1 - Enviar comando 'P' para ler sensor de humidade (#Ph+05!)\n");

	resetTxBuffer();
	resetRxBuffer();

	// Envia o comando "P" com sensor 'h'
	rxChar('#');
	rxChar('P');
	rxChar('h');
	rxChar('!');

	cmdProcessor();

	getTxBuffer(ans, &len);

	printf("Resposta recebida: ");
	for (i = 0; i < len; i++) {
		printf("%c", ans[i]);
	}
	printf("\n");

	/* Teste 2.2: Envia comando 'P' para ler o sensor de CO2  */
	printf("\nTest 2.3 - Enviar comando 'P' para ler sensor de CO2 (#Pc+00500!) \n");

	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('P');
	rxChar('c');
	rxChar('!');

	cmdProcessor();

	getTxBuffer(ans, &len);

	printf("Resposta recebida: ");
	for (i = 0; i < len; i++) {
		printf("%c", ans[i]);
	}
	printf("\n");


	/* Much more tests are needed. Unity shoul be used for it. */
	
	return 0;
}
