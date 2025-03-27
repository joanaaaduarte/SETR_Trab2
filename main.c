/* ******************************************************/
/* SETR 23/24, Paulo Pedreiras                          */
/* 		Sample code for Assignment 2					*/
/*   	A few tests to the cmdProcessor to illustrate	*/
/*      how the the tests can be carried out.         	*/
/*														*/
/* Note that I'm not using Unity! That is part of your 	*/
/*		work. 											*/
/*                                                      */
/* Compile with: gcc cmdproc.c main.c -o main           */
/*	Feel free to use flags such as -Wall -Wpedantic ...	*/
/*	and it is a good idea to create a makefile			*/
/*                                                      */
/* ******************************************************/
#include <stdio.h>
#include <string.h>
#include "cmdproc.h"


int main(void) 
{
    int i, len;
    unsigned char ans[30];

    printf("\n Smart Sensor interface emulation \n");
    printf(" \t - simple illustration of interface and use \n\n\r");

    /* Inicializa os buffers da UART */
    resetTxBuffer();
    resetRxBuffer();

    /* Teste 1: Envia o comando "A" para ler todos os sensores */
    printf("Test 1 - Enviar comando 'A' para ler todos os sensores\n");

    // Envia o comando "A"
    rxChar('#');
    rxChar('A');
    rxChar('!');
    
    cmdProcessor();  // Processa o comando

    // Obtém o que foi enviado pela UART
    getTxBuffer(ans, &len);
    printf("Resposta recebida: ");
    for (i = 0; i < len; i++) {
        printf("%c", ans[i]);
    }

	/**************************************************************************************/

	/* Teste 2: Envia o comando "P" para ler o sensor de temperatura */
    printf("\nTest 2 - Enviar comando 'P' para ler sensor de temperatura\n");

    resetTxBuffer();
    resetRxBuffer();

    // Envia o comando "P" com sensor 't'
    rxChar('#');
    rxChar('P');
    rxChar('t');
    rxChar('!');

    cmdProcessor();

    getTxBuffer(ans, &len);

    printf("Resposta recebida: ");
    for (i = 0; i < len; i++) {
        printf("%c", ans[i]);
    }
    printf("\n");


	/************************************************************************* */
	    /* Teste 3: Envia o comando "P" para ler o sensor de humidade */
    printf("\nTest 3 - Enviar comando 'P' para ler sensor de humidade\n");

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

	/**************************************************************************************+ */
	/* Teste 4: Envia o comando "P" para ler o sensor de CO2 */
    printf("\nTest 4 - Enviar comando 'P' para ler sensor de CO2\n");

    resetTxBuffer();
    resetRxBuffer();

    // Envia o comando "P" com sensor 'c'
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
