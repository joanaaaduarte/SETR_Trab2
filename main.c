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

    printf("\n Smart Sensor interface emulation \n");
    printf(" \t - simple illustration of interface and use \n\n\r");

	/* Teste 1: Envia o comando "A" para ler todos os sensores */
    printf("Test 1 - Enviar comando 'A' para ler todos os sensores\n");

    /* Inicializa os buffers da UART */
    resetTxBuffer();
    resetRxBuffer();


		// Envia o comando "A"
	rxChar('#');
	rxChar('A');
	rxChar('!');

	cmdProcessor();  // Processa o comando

	getTxBuffer(ans, &len); 

	printf("Resposta recebida: ");
	for (i = 0; i < len; i++) {
		if (len >= 5 && ans[1] == 'A' && ans[2] == 't' && i == 3) {
			int8_t temp;
			memcpy(&temp, &ans[i], sizeof(temp));
			printf("%d", temp);
		} else {
			printf("%c", ans[i]);
		}
	}
	printf("\n");

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

	/************************************************************************** */ 
	/* Teste 5: Enviar comando 'P' para ler a temperatura e imprimir valor real */
	printf("\nTest 5 - Enviar comando 'P' para ler temperatura (valor real)\n");

	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('P');
	rxChar('t');
	rxChar('!');

	cmdProcessor();
	getTxBuffer(ans, &len);

	printf("Resposta recebida: ");
	for (i = 0; i < len; i++) {
		if (len >= 5 && ans[1] == 'P' && ans[2] == 't' && i == 3) {
			int8_t temp;
			memcpy(&temp, &ans[i], sizeof(temp));
			printf("%d", temp);
		} else {
			printf("%c", ans[i]);
		}
	}
	printf("\n");

	
	// Teste 5.1 - Preencher histórico de temperatura com 20 amostras
	printf("\nTest 5.1 - Preencher histórico de temperatura com 20 leituras\n");

	for (int j = 0; j < 20; ++j) {
	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('P');
	rxChar('t');
	rxChar('!');

	cmdProcessor();
	getTxBuffer(ans, &len);

	printf("Leitura %2d: ", j + 1);
	for (int i = 0; i < len; ++i) {
		if (len >= 5 && ans[1] == 'P' && ans[2] == 't' && i == 3) {
			int8_t temp;
			memcpy(&temp, &ans[i], sizeof(temp));
			printf("%d", temp);
		} else {
			printf("%c", ans[i]);
		}
	}
	printf("\n");
}
/****************************************************************************** */
	// Teste 6: comando 'L' - ler histórico de temperatura
	printf("\nTest 6 - Enviar comando 'L' para ver o histórico da temperatura\n");

	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('L');
	rxChar('!');

	cmdProcessor();
	getTxBuffer(ans, &len);

	printf("Histórico de temperatura recebido:\n");

	for (int i = 0; i < len; ++i) {
		if (i == 0 || i == len - 1) {
			printf("%c ", ans[i]);  // '#' e '!'
		} else if (i == 1 || i == 2) {
			printf("%c ", ans[i]);	// Imprime 'L' e 't' como caracteres
		} else {
			// Passar os valores binários para int8_t
			int8_t temp;
			memcpy(&temp, &ans[i], sizeof(temp));
			printf("%d ", temp);
		}
	}
	printf("\n");

	/*************************************************************** */
	// Teste 7 - Comando 'R' para resetar histórico de temperatura
	printf("\nTest 7 - Enviar comando 'R' para limpar histórico\n");

	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('R');
	rxChar('!');

	cmdProcessor();
	getTxBuffer(ans, &len);

	printf("Resposta ao reset: ");
	for (int i = 0; i < len; i++) {
		printf("%c", ans[i]);
	}
	printf("\n");

	// Verificar se histórico foi limpo
	printf("Test 7.1 - Verificar histórico após reset\n");

	resetTxBuffer();
	resetRxBuffer();

	rxChar('#');
	rxChar('L');
	rxChar('!');

	cmdProcessor();
	getTxBuffer(ans, &len);

	printf("Histórico de temperatura recebido:\n");

	for (int i = 0; i < len; ++i) {
		if (i == 0 || i == len - 1) {
			printf("%c ", ans[i]);  // '#' e '!'
		} else if (i == 1 || i == 2) {
			// Imprime 'L' e 't' como caracteres
			printf("%c ", ans[i]);
		} else {
			int8_t temp;
			memcpy(&temp, &ans[i], sizeof(temp));
			printf("%d ", temp);
		}
	}
	printf("\n");



	/* Much more tests are needed. Unity shoul be used for it. */
	
	return 0;
}
