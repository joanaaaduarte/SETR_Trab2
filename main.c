/**
    \file main.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
 */

 #include <stdio.h>
 #include <string.h>
 #include "core/include/cmdproc.h"
 #include <stdint.h>
 
 int main(void) 
 {
	 int i, len;
	 unsigned char ans[30];
	 unsigned char inputBuffer[10];
	 int checksum, sum;
 
	 printf("\n Smart Sensor interface emulation \n");
	 printf(" \t - simple illustration of interface and use \n\n\r");
 
	 /*************************************************** */
	 /* Teste 1: Enviar comando "A" para ler todos os sensores */
	 printf("Test 1 - Enviar comando 'A' para ler todos os sensores (#At-50h+50c+00400!) \n");
 
	 /* Inicializa os buffers da UART */
	 resetTxBuffer();
	 resetRxBuffer();
	 
	 // Envia o comando base "A"
	 rxChar('#');
	 rxChar('A');
	 rxChar('!');
 
	 // Prepara o comando para inserir o checksum
	 // Para o comando "A", usamos "#A" como base
	 snprintf((char *)inputBuffer, sizeof(inputBuffer), "#A");
	 checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
	 // O checksum é formatado com 3 dígitos e concatenado antes do '!'
	 snprintf((char *)&inputBuffer[2], sizeof(inputBuffer) - 2, "%03d!", checksum);
	 
	 // Envia os caracteres do comando formatado individualmente
	 for (i = 0; i < strlen((char *)inputBuffer); i++) {
		 rxChar(inputBuffer[i]);
	 }
	 
	 // Processa o comando e obtém a resposta
	 cmdProcessor();
	 getTxBuffer(ans, &len);
	 
	 // Calcula a soma ASCII da resposta (ignorando o primeiro '#' e o último '!')
	 sum = 0;
	 for (i = 1; i < len - 1; i++) {
		 sum += ans[i];
	 }
	 printf("Resposta recebida ASCII somado: %d\n", sum);
 
	 /*************************************************** */
	 /* Teste 2: Enviar comando "P" para ler o sensor de temperatura */
	 printf("\nTest 2 - Enviar comando 'P' para ler sensor de temperatura (#Pt-40!)\n");
 
	 resetTxBuffer();
	 resetRxBuffer();
 
	 // Envia o comando base "Pt" para temperatura
	 rxChar('#');
	 rxChar('P');
	 rxChar('t');
	 rxChar('!');
 
	 // Monta o comando com checksum usando "#Pt" como base
	 snprintf((char *)inputBuffer, sizeof(inputBuffer), "#Pt");
	 checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
	 snprintf((char *)inputBuffer + 2, sizeof(inputBuffer) - 2, "%03d!", checksum);
 
	 for (i = 0; i < strlen((char *)inputBuffer); i++) {
		 rxChar(inputBuffer[i]);
	 }
 
	 cmdProcessor();
	 getTxBuffer(ans, &len);
 
	 sum = 0;
	 printf("Resposta recebida ASCII somado: ");
	 for (i = 1; i < len - 1; i++) {
		 sum += ans[i];
	 }
	 printf("%d\n", sum);
 
	 /*************************************************** */
	 /* Teste 2.1: Enviar comando "P" para ler sensor de humidade */
	 printf("\nTest 2.1 - Enviar comando 'P' para ler sensor de humidade (#Ph+05!)\n");
 
	 resetTxBuffer();
	 resetRxBuffer();
 
	 // Envia o comando base "Ph" para humidade
	 rxChar('#');
	 rxChar('P');
	 rxChar('h');
	 rxChar('!');
 
	 // Monta o comando com checksum usando "#Ph" como base
	 snprintf((char *)inputBuffer, sizeof(inputBuffer), "#Ph");
	 checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
	 snprintf((char *)inputBuffer + 2, sizeof(inputBuffer) - 2, "%03d!", checksum);
 
	 for (i = 0; i < strlen((char *)inputBuffer); i++) {
		 rxChar(inputBuffer[i]);
	 }
 
	 cmdProcessor();
	 getTxBuffer(ans, &len);
 
	 sum = 0;
	 printf("Resposta recebida ASCII somado: ");
	 for (i = 1; i < len - 1; i++) {
		 sum += ans[i];
	 }
	 printf("%d\n", sum);
 
	 /*************************************************** */
	 /* Teste 2.3: Enviar comando "P" para ler sensor de CO2 */
	 printf("\nTest 2.3 - Enviar comando 'P' para ler sensor de CO2 (#Pc+00500!) \n");
 
	 resetTxBuffer();
	 resetRxBuffer();
 
	 // Envia o comando base "Pc" para CO2
	 rxChar('#');
	 rxChar('P');
	 rxChar('c');
	 rxChar('!');
 
	 // Monta o comando com checksum usando "#Pc" como base
	 snprintf((char *)inputBuffer, sizeof(inputBuffer), "#Pc");
	 checksum = calcChecksum((unsigned char *)&inputBuffer[1], strlen((char *)&inputBuffer[1]));
	 snprintf((char *)inputBuffer + 2, sizeof(inputBuffer) - 2, "%03d!", checksum);
 
	 for (i = 0; i < strlen((char *)inputBuffer); i++) {
		 rxChar(inputBuffer[i]);
	 }
 
	 cmdProcessor();
	 getTxBuffer(ans, &len);
 
	 sum = 0;
	 printf("Resposta recebida ASCII somado: ");
	 for (i = 1; i < len - 1; i++) {
		 sum += ans[i];
	 }
	 printf("%d\n", sum);
 
	 /* Obs.: O cálculo da soma ASCII (excluindo o primeiro caractere '#' e o último '!') 
		deve resultar em 0 para indicar que a resposta está corretamente formada. */
 
	 /* Muito mais testes podem ser implementados (idealmente usando uma framework como Unity) */
 
	 return 0;
 }
 