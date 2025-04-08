/**
    \file cmdproc.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date April 8, 2025  
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "cmdproc.h"

/* Internal variables */
/* Used as part of the UART emulation */
static unsigned char UARTRxBuffer[UART_RX_SIZE];	//Buffer de receção
static unsigned char rxBufLen = 0; 					//Comprimento do buffer de receção

static unsigned char UARTTxBuffer[UART_TX_SIZE];	//Buffer de transmissão	
static unsigned char txBufLen = 0; 					//Comprimento do buffer de receção

//Inicilizar buffer Rx
RxBuffer Rx_Buf = {
    .contador = 0,
    .indice_fim = 0,
    .p_dados = Rx_Buf.buffer
};

	/*Sensor de Temperatura*/
static int8_t temperature_history[MAX_HISTORY];
static int temp_history_index = 0;

// Tabela de valores de temperatura
const int8_t simulated_temps[NUM_SAMPLES] = {
    	-50, -40, -35, -30, -25, -20,
		-15, -10, -5, 0, 5, 10, 15, 
		18, 20, 22, 24, 26, 28, 30,
    	32, 35, 38, 40, 45, 50, 55,
		58, -40, 60
};

int temps_index = 0;


int8_t getNextTemperature()					//ref:https://stackoverflow.com/questions/3621956/how-do-i-code-a-simple-integer-circular-buffer-in-c-c
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
	temps_index = 0;
}

	/*Sensor de Humidade */
static int8_t humidity_history[MAX_HISTORY];
static int humidity_history_index = 0;

// Tabela de valores de humidade simulados 
const int8_t simulated_hums[NUM_SAMPLES] = {
    50, 5, 10, 15, 20, 25,
    30, 35, 40, 45, 50, 55,
    60, 65, 70, 75, 80, 85,
    90, 95, 100, 95, 90, 85,
    80, 75, 70, 65, 60, 55
};

int humidity_index = 0;

int8_t getNextHumidity()				//ref:https://stackoverflow.com/questions/3621956/how-do-i-code-a-simple-integer-circular-buffer-in-c-c
{
    int8_t hum = simulated_hums[humidity_index];
    humidity_index = (humidity_index + 1) % NUM_SAMPLES;

    // Atualiza histórico
    humidity_history[humidity_history_index] = hum;
    humidity_history_index = (humidity_history_index + 1) % MAX_HISTORY;

    return hum;
}

void resetHumidityHistory()
{
    memset(humidity_history, 0, sizeof(humidity_history));
    humidity_history_index = 0;
	humidity_index = 0;
}

/* Sensor de CO2 */
static int16_t co2_history[MAX_HISTORY];
static int co2_history_index = 0;

// Tabela de valores simulados de CO2 
const int16_t simulated_co2[NUM_SAMPLES] = {
    400, 500, 600, 700, 800, 900,
    1000, 1200, 1400, 1600, 1800, 2000,
    2500, 3000, 4000, 5000, 6000, 8000,
    10000, 12000, 15000, 17000, 18000, 19000,
    19500, 19800, 19900, 19950, 19980, 20000
};

int co2_index = 0;

int16_t getNextCO2()							//ref:https://stackoverflow.com/questions/3621956/how-do-i-code-a-simple-integer-circular-buffer-in-c-c
{
    int16_t co2 = simulated_co2[co2_index];
    co2_index = (co2_index + 1) % NUM_SAMPLES;

    // Atualiza histórico
    co2_history[co2_history_index] = co2;
    co2_history_index = (co2_history_index + 1) % MAX_HISTORY;

    return co2;
}

void resetCO2History()
{
    memset(co2_history, 0, sizeof(co2_history));
    co2_history_index = 0;
	co2_index = 0;
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

            case 'A': {

				
				if (rxBufLen < i + 6)
				{
					return -1;
				}

				// Verifica checksum de entrada ,apenas para 'A'
				if (!calcChecksum(&UARTRxBuffer[i + 1], 1))
				{
					return -3;
				}

			    if (UARTRxBuffer[i + 5] != EOF_SYM)
				{
        			return -4;
				}

				// Ler sensores
				int8_t temp = getNextTemperature();
				int8_t hum  = getNextHumidity();
				int16_t co2 = getNextCO2();

				// Resposta
				unsigned char resposta[32];
				int pos = 0;

				resposta[pos++] = 'A';

				//  Temperatura 
				resposta[pos++] = 't';
				if (temp < 0) {

					resposta[pos++] = '-';
					temp = -temp;

				} else {

					resposta[pos++] = '+';

				}

				resposta[pos++] = '0' + (temp / 10);
				resposta[pos++] = '0' + (temp % 10);

				// Humidade 
				resposta[pos++] = 'h';
				if (hum < 0) {
					resposta[pos++] = '-';
					hum = -hum;
				} else {
					resposta[pos++] = '+';
				}
				resposta[pos++] = '0' + (hum / 10);
				resposta[pos++] = '0' + (hum % 10);

				//  CO2 -
				resposta[pos++] = 'c';
				resposta[pos++] = '+'; // sempre positivo

				char co2_str[7]; // 5 digitos + '\0'
				snprintf(co2_str, sizeof(co2_str), "%05d", co2);
				for (int j = 0; j < 5; ++j)
					resposta[pos++] = co2_str[j];

				//  Checksum da resposta 
				int soma = 0;
				for (int j = 0; j < pos; ++j)
					soma += resposta[j];

				int cs = soma % 256;
				char cs_str[5];
				snprintf(cs_str, sizeof(cs_str), "%03d", cs);

				// Transmitir a resposta completa
				txChar('#');
				for (int j = 0; j < pos; ++j)
					txChar(resposta[j]);

				txChar(cs_str[0]);
				txChar(cs_str[1]);
				txChar(cs_str[2]);
				txChar('!');

				return 0;
			
			}

			case 'P': {  

				if (rxBufLen < i + 7)
				{
        			return -1;
				};

				if (!calcChecksum(&UARTRxBuffer[i + 1], 2))  	
				{					 
        				return -3;
				};

				// Verifica se o comando termina com '!'
				if (UARTRxBuffer[i + 6] != EOF_SYM)
				{
					return -4;
				}


				sid = UARTRxBuffer[i + 2];  // 't', 'h', ou 'c'

				if (sid == 't') {

					int8_t temp = getNextTemperature();

					unsigned char resposta[10];
					int posicao = 0;

					resposta[posicao++] = 'P';
					resposta[posicao++] = 't';

					if (temp < 0) {
						resposta[posicao++] = '-';
						temp = -temp;
					} else {
						resposta[posicao++] = '+';
					}

					//Converter para ASCII					https://stackoverflow.com/questions/29077138/conversion-of-integer-to-char-array-in-c
					resposta[posicao++] = '0' + (temp / 10);
					resposta[posicao++] = '0' + (temp % 10);

					// Calcular checksum da resposta
					int soma = 0;
					for (int j = 0; j < posicao; ++j)
					{
						soma = soma + resposta[j];
					};

					int cs = soma % 256;
					char cs_str[5];

					snprintf(cs_str, sizeof(cs_str), "%03d", cs);

					// Enviar a resposta real
					txChar('#');
					for (int j = 0; j < posicao; ++j)
					{
						txChar(resposta[j]);
					};

					txChar(cs_str[0]);
					txChar(cs_str[1]);
					txChar(cs_str[2]);
					txChar('!');
					return 0;

				} else if (sid == 'h') {

					int8_t hum = getNextHumidity();

					unsigned char resposta[10];
					int posicao = 0;

					resposta[posicao++]= 'P';
					resposta[posicao++]= 'h';

					if (hum < 0) {

						resposta[posicao++] = '-';
						hum = -hum;

					} else {

						resposta[posicao++] = '+';
					}
					
					//Converter para ASCII				//Ref:https://stackoverflow.com/questions/29077138/conversion-of-integer-to-char-array-in-c
					resposta[posicao++] = '0' + (hum / 10);
					resposta[posicao++] = '0' + (hum % 10);


					// Calcular checksum
					int soma = 0;
					for (int j = 0; j < posicao; ++j)
					{
						soma = soma +  resposta[j];
					};

					int cs = soma % 256;
					char cs_str[5];
					snprintf(cs_str, sizeof(cs_str), "%03d", cs);

					// Enviar resposta com checksum
					txChar('#');
					for (int j = 0; j < posicao; ++j)
					{
						txChar(resposta[j]);
					};

					txChar(cs_str[0]);
					txChar(cs_str[1]);
					txChar(cs_str[2]);
					txChar('!');

					return 0;

				} else if (sid == 'c') {

					int16_t co2 = getNextCO2(); 

					char co2_str[7];
					snprintf(co2_str, sizeof(co2_str), "%05d", co2); // 5 dígitos

					unsigned char resposta[16];
					int posicao = 0;

					resposta[posicao++] = 'P';
					resposta[posicao++] = 'c';

					resposta[posicao++] = '+';  // sempre positivo

					// Copia os 5 dígitos do CO2
					for (int i = 0; i < 5; ++i)
					{
						resposta[posicao++] = co2_str[i];
					};

					// Calcular checksum
					int soma = 0;
					for (int j = 0; j < posicao; ++j)
					{
						soma = soma + resposta[j];
					};

					int cs = soma % 256;
					char cs_str[6];
					snprintf(cs_str, sizeof(cs_str), "%03d", cs);

					// Enviar resposta com checksum
					txChar('#');
					for (int j = 0; j < posicao; ++j)
					{
						txChar(resposta[j]);
					};

					txChar(cs_str[0]);
					txChar(cs_str[1]);
					txChar(cs_str[2]);
					txChar('!');


					return 0;

			case 'L':	{

				// Validação do formato do comando
				if (rxBufLen < i + 6) 
				{
					return -1;
				}

				if (!calcChecksum(&UARTRxBuffer[i + 1], 1))
				{
					return -3;
				} 

				if (UARTRxBuffer[i + 5] != EOF_SYM)
				{
						return -4;
				} 
				
				char cs_str[5];
				int soma, cs, pos;

				//  Temperatura 
				unsigned char resposta_t[128];
				pos = 0;

				resposta_t[pos++] = 'L';
				resposta_t[pos++] = 't';

				for (int j = 0; j < MAX_HISTORY; ++j) {

					int idx = (temp_history_index + j) % MAX_HISTORY;
					int8_t val = temperature_history[idx];

					if (val < 0) {

						resposta_t[pos++] = '-';
						val = -val;

					} else {

						resposta_t[pos++] = '+';

					}

					resposta_t[pos++] = '0' + (val / 10);
					resposta_t[pos++] = '0' + (val % 10);

				}

				soma = 0;

				for (int j = 0; j < pos; ++j)
				{
					soma = soma + resposta_t[j];

				}

				cs = soma % 256;
				snprintf(cs_str, sizeof(cs_str), "%03d", cs);

				txChar('#');

				for (int j = 0; j < pos; ++j)
				{
					txChar(resposta_t[j]);
				}

				for (int i = 0; i < 3; ++i)
				{	
					txChar(cs_str[i]);
				}

				txChar('!');

				//  Humidade 
				unsigned char resposta_h[128];
				pos = 0;

				resposta_h[pos++] = 'L';
				resposta_h[pos++] = 'h';

				for (int j = 0; j < MAX_HISTORY; ++j) {

					int idx = (humidity_history_index + j) % MAX_HISTORY;
					int8_t val = humidity_history[idx];

					if (val < 0) {

						resposta_h[pos++] = '-';
						val = -val;

					} else {

						resposta_h[pos++] = '+';

					}

					resposta_h[pos++] = '0' + (val / 10);
					resposta_h[pos++] = '0' + (val % 10);
				}

				soma = 0;

				for (int j = 0; j < pos; ++j)
				{
					soma = soma +  resposta_h[j];
				}

				cs = soma % 256;
				snprintf(cs_str, sizeof(cs_str), "%03d", cs);

				txChar('#');
				for (int j = 0; j < pos; ++j)
				{
					txChar(resposta_h[j]);
				}

				for (int i = 0; i < 3; ++i)
				{
					txChar(cs_str[i]);
				} 
				
				txChar('!');

				//  CO2 
				unsigned char resposta_c[1024];
				pos = 0;
				resposta_c[pos++] = 'L';
				resposta_c[pos++] = 'c';

				for (int j = 0; j < MAX_HISTORY; ++j) {
					int idx = (co2_history_index + j) % MAX_HISTORY;
					int16_t val = co2_history[idx];

					resposta_c[pos++] = '+';

					char val_str[7];
					snprintf(val_str, sizeof(val_str), "%05d", val);

					for (int k = 0; k < 5; ++k)
					
						resposta_c[pos++] = val_str[k];
				}

				soma = 0;
				for (int j = 0; j < pos; ++j)
				{
					 soma = soma + resposta_c[j];
				}
				cs = soma % 256;
				snprintf(cs_str, sizeof(cs_str), "%03d", cs);

				txChar('#');

				for (int j = 0; j < pos; ++j)
				{
					txChar(resposta_c[j]);
				}

				for (int i = 0; i < 3; ++i)
				{
					txChar(cs_str[i]);
				}

				txChar('!');

				return 0;

			}


            case 'R':{

					// Validação básica
				if (rxBufLen < i + 6)
				{
					return -1;
				}

				if (!calcChecksum(&UARTRxBuffer[i + 1], 1))
				{	
					return -3;

				}

				if (UARTRxBuffer[i + 5] != EOF_SYM)
				{
					return -4;
				}

				// Reset dos históricos
				resetTemperatureHistory();
				resetHumidityHistory();
				resetCO2History();

				// Preparar resposta #Rxxx!
				unsigned char resposta[4];
				resposta[0] = 'R';

				// Calcular checksum da resposta
				int soma = resposta[0];  		// só R

				int cs = soma % 256;
				char cs_str[4];
				snprintf(cs_str, sizeof(cs_str), "%03d", cs);

				txChar('#');

				txChar(resposta[0]);

				txChar(cs_str[0]);
				txChar(cs_str[1]);
				txChar(cs_str[2]);

				txChar('!');

				return 0;

				}


			} else {
				return -2;  // Tipo de sensor inválidocd
			}

				break;  
			}



            default:
                return -2;  
        }
    }

    // Caso não  encontre o SOF ou o comando esteja mal formatado
    return -4;
}
	

/*
 * rxChar
 */
int rxChar(unsigned char car)
{
    if (car == '\0') {
        return -1;  // Caracter inválido
    }

    if (rxBufLen < UART_RX_SIZE) {
        UARTRxBuffer[rxBufLen] = car;
        rxBufLen += 1;

        Rx_Buf.buffer[Rx_Buf.indice_fim++] = car;
        Rx_Buf.contador++;

        if (Rx_Buf.indice_fim >= RX_BUF_SIZE)
            Rx_Buf.indice_fim = 0;

        return 0;
    }
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
	// memset: https://www.tutorialspoint.com/c_standard_library/c_function_memset.htm
	memset(UARTRxBuffer, 0, UART_RX_SIZE);
	rxBufLen = 0;	

	//  limpar o Rx_Buf  que utilizamos  para os testes 
    memset(Rx_Buf.buffer, 0, sizeof(Rx_Buf.buffer));

	Rx_Buf.contador = 0;
    Rx_Buf.indice_fim = 0;

	return;
}

/*
 * resetTxBuffer
 */
void resetTxBuffer(void)
{
	memset(UARTTxBuffer, 0, UART_TX_SIZE);
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
	/* of these digits/characters to the ones in the RxBuffer,  */
	/* positions nbytes, nbytes + 1 and nbytes +2.              */
	
	/* That is your work to do. In this example I just assume   */
	/* that the checksum is always OK.                          */      

	// First: Sum first buffer nbytes
	int sum = 0;
	for(int i = 0; i < nbytes; i++){
			sum +=buf[i];
	}

	// Second: Compute the checksum as the modulo 256 of the sum.
	int checksum = 0;
	checksum = sum % 256;

	// Third: Convert string 3 digits to ASCII
	char checksumStr[10];
	snprintf(checksumStr, sizeof(checksumStr), "%03d", checksum);

    // Compara os 3 dígitos com os caracteres no buffer original
    if (buf[nbytes]     == checksumStr[0] &&
        buf[nbytes + 1] == checksumStr[1] &&
        buf[nbytes + 2] == checksumStr[2]) {
        return 1;  // válido
    }

    return 0;  // inválido            
}
