/**
    \file cmdproc.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
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

RxBuffer Rx_Buf = {
    .count = 0,
    .tail = 0,
    .data = Rx_Buf.buffer
};



void history_reset(char mode)
{
    (void)mode;  // se o modo ainda não está a ser usado

    // Limpa Rx_Buf para testes
    memset(Rx_Buf.buffer, 0, sizeof(Rx_Buf.buffer));

    Rx_Buf.count = 0;
    Rx_Buf.tail = 0;
    Rx_Buf.data = Rx_Buf.buffer;

    // Limpa buffer interno também
    resetRxBuffer();
}


	/*Sensor de Tmeperatura*/
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

	/*Sensor de Humidade */
// Histórico de humidade - últimas MAX_HISTORY = 20 amostras
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

int8_t getNextHumidity()
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
}

/* Sensor de CO₂ */
// Histórico de CO₂ - últimas MAX_HISTORY = 20 amostras
static int16_t co2_history[MAX_HISTORY];
static int co2_history_index = 0;

// Tabela de valores simulados de CO₂ (400 a 20000 ppm)
const int16_t simulated_co2[NUM_SAMPLES] = {
    400, 500, 600, 700, 800, 900,
    1000, 1200, 1400, 1600, 1800, 2000,
    2500, 3000, 4000, 5000, 6000, 8000,
    10000, 12000, 15000, 17000, 18000, 19000,
    19500, 19800, 19900, 19950, 19980, 20000
};

int co2_index = 0;

int16_t getNextCO2()
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
				if (UARTRxBuffer[i + 2] != EOF_SYM) {
					return -3;
				}

				int8_t temp = getNextTemperature();
				int8_t hum = getNextHumidity();
				int16_t co2 = getNextCO2();


				txChar('#');
				txChar('A');

				// Temperatura
				txChar('t');
				if (temp < 0) {

					txChar('-');
					temp = -temp;

				} else {

					txChar('+');

				}

				txChar('0' + (temp / 10));
				txChar('0' + (temp % 10));

				// Humidade
				txChar('h');
				if (hum < 0) {

					txChar('-');
					hum = -hum;

				} else {

					txChar('+');	

				}
				txChar('0' + (hum / 10));
				txChar('0' + (hum % 10));

				// CO2
				txChar('c');
				txChar('+');
				char buffer[7];

				snprintf(buffer, sizeof(buffer), "%05d", co2);

				for (int j = 0; j < 5; ++j) {

					txChar(buffer[j]);
				}
	if (i < rxBufLen) {

        switch (UARTRxBuffer[i + 1]) {

            case 'A': {
				if (UARTRxBuffer[i + 2] != EOF_SYM)
					return -3;

				int8_t temp = getNextTemperature();
				int8_t hum = getNextHumidity();
				int16_t co2 = getNextCO2();


				txChar('#');
				txChar('A');

				// Temperatura
				txChar('t');
				if (temp < 0) {
					txChar('-');
					temp = -temp;
				} else {
					txChar('+');
				}

				txChar('0' + (temp / 10));
				txChar('0' + (temp % 10));

				// Humidaif (i < rxBufLen) {
        switch (UARTRxBuffer[i + 1]) {

            case 'A': {
				if (UARTRxBuffer[i + 2] != EOF_SYM) {
					return -3;
				}

				int8_t temp = getNextTemperature();
				int8_t hum = getNextHumidity();
				int16_t co2 = getNextCO2();


				txChar('#');
				txChar('A');

				// Temperatura
				txChar('t');
				if (temp < 0) {

					txChar('-');
					temp = -temp;

				} else {

					txChar('+');

				}

				txChar('0' + (temp / 10));
				txChar('0' + (temp % 10));

				// Humidade
				txChar('h');
				if (hum < 0) {

					txChar('-');
					hum = -hum;

				} else {

					txChar('+');	

				}
				txChar('0' + (hum / 10));
				txChar('0' + (hum % 10));

				// CO2
				txChar('c');
				txChar('+');
				char buffer[7];

				snprintf(buffer, sizeof(buffer), "%05d", co2);

				for (int j = 0; j < 5; ++j) {

					txChar(buffer[j]);
				}

				txChar('!');
				break;
			}
				txChar('h');
				if (hum < 0) {
					txChar('-');
					hum = -hum;
				} else {
					txChar('+');	
				}
				txChar('0' + (hum / 10));
				txChar('0' + (hum % 10));

				// CO2
				txChar('c');
				txChar('+');
				char buffer[7];

				snprintf(buffer, sizeof(buffer), "%05d", co2);

				for (int j = 0; j < 5; ++j)
					txChar(buffer[j]);
				
				txChar('!');
				break;
			}
				case 'P': {  
				sid = UARTRxBuffer[i + 2];  // 't', 'h', ou 'c'

				if (sid == 't') {

					int8_t temp = getNextTemperature();

					txChar('#');
					txChar('P');
					txChar('t');

					if (temp < 0) {

						txChar('-');
						temp = -temp;

					} else {
						txChar('+');
					}

					txChar('0' + (temp / 10));  // dezena
					txChar('0' + (temp % 10));  // unidade

					txChar('!');
				} else if (sid == 'h') {

					int8_t hum = getNextHumidity();

					txChar('#');
					txChar('P');
					txChar('h');

					if (hum < 0) {

						txChar('-');
						hum = -hum;

					} else {

						txChar('+');
					}

					txChar('0' + (hum / 10));
					txChar('0' + (hum % 10));

					txChar('!');
				} else if (sid == 'c') {

					int16_t co2 = getNextCO2(); 

					txChar('#');
					txChar('P');
					txChar('c');

					txChar('+');	//sempre positivo

					// Converter c02 para string
					char buffer[7];  // 6 + '/0'
					snprintf(buffer, sizeof(buffer), "%05d", co2);

					for (int j = 0; j < 5; ++j) {
						txChar(buffer[j]);
					}

					txChar('!');
				} else {
					return -2;  // Tipo de sensor inválido
				}

				break;  
			}

            case 'L':	

                txChar('#');
                txChar('L');

				// Temperature data
				txChar('t');
				for (int j = 0; j < MAX_HISTORY; j++) {
					if (j > 0) txChar(',');
					char tempBuffer[4];
					snprintf(tempBuffer, sizeof(tempBuffer), "%d", temperature_history[(temp_history_index + j) % MAX_HISTORY]);
					for (int k = 0; k < strlen(tempBuffer); k++) {
						txChar(tempBuffer[k]);
					}
				}
		
				// Humidity data
				txChar('h');
				for (int j = 0; j < MAX_HISTORY; j++) {
					if (j > 0) txChar(',');
					char humBuffer[4];
					snprintf(humBuffer, sizeof(humBuffer), "%d", humidity_history[(humidity_history_index + j) % MAX_HISTORY]);
					for (int k = 0; k < strlen(humBuffer); k++) {
						txChar(humBuffer[k]);
					}
				}

				// CO2 data
				txChar('c');
				for (int j = 0; j < MAX_HISTORY; j++) {
					if (j > 0) txChar(',');
					char co2Buffer[7];
					snprintf(co2Buffer, sizeof(co2Buffer), "%05d", co2_history[(co2_history_index + j) % MAX_HISTORY]);
					for (int k = 0; k < strlen(co2Buffer); k++) {
						txChar(co2Buffer[k]);
					}
				}

				txChar('!');  
				break;

            case 'R':

				if (UARTRxBuffer[i + 2] != EOF_SYM) 
					return -3;

 				txChar('#');
                txChar('R');              
                txChar('!');
                break;

            default:
                return -2;  
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
    if (rxBufLen < UART_RX_SIZE) {
        UARTRxBuffer[rxBufLen] = car;
        rxBufLen += 1;

        // Atualiza também Rx_Buf usado para testes
        Rx_Buf.buffer[Rx_Buf.tail++] = car;
        Rx_Buf.count++;

        if (Rx_Buf.tail >= RX_BUF_SIZE)
            Rx_Buf.tail = 0;

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

	return sum % 256;
	// Second: Compute the checksum as the modulo 256 of the sum.
	/* int checksum = 0;
	checksum = sum % 256;

	// Third: Convert string 3 digits to ASCII
	char checksumStr[10];
	snprintf(checksumStr, sizeof(checksumStr), "%03d", checksum);

	return atoi(checksumStr); */              
}