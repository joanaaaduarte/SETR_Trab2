/**
    \file test.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date April 8, 2025 
 */

 #include "unity.h"
 #include "cmdproc.h"
 #include <string.h>


 /**
 * @brief Setup function called before each test.
 *
 * This function is used to initialize variables or prepare the test environment.
 */

 void setUp(void) {
    resetTxBuffer();
    resetRxBuffer();
    resetTemperatureHistory();
    resetHumidityHistory();
    resetCO2History();
}

/**
 * @brief Teardown function called after each test.
 *
 * This function is used to clean up after tests, such as releasing resources.
 */
void tearDown(void) {
    // Cleanup code after each test (if needed)
}


//                       UART Tests

//RX


/**
 * \test Test if resetRxBuffer correctly clears the RX buffer.
 */

    // Testar se resetRxBuffer limpa o buffer corretamente
    void test_resetRxBuffer()
    {

        rxChar('Z');  

        TEST_ASSERT_EQUAL_INT(1, Rx_Buf.contador);

        resetRxBuffer();

        TEST_ASSERT_EQUAL_INT(0, Rx_Buf.contador);
        TEST_ASSERT_EQUAL_INT(0, Rx_Buf.indice_fim);
        TEST_ASSERT_EQUAL_CHAR('\0', Rx_Buf.buffer[0]);
    }

/**
 * \test Test if rxChar accepts valid characters into the RX buffer.
 */
    //Testar se rxChar insere caracteres válidos no buffer Rx
    void test_rxChar_valido()
    {
        resetRxBuffer();

        int ret1 = rxChar('#');
        int ret2 = rxChar('2');
        int ret3 = rxChar('A');

        TEST_ASSERT_EQUAL_INT(0, ret1);
        TEST_ASSERT_EQUAL_INT(0, ret2);
        TEST_ASSERT_EQUAL_INT(0, ret3);

        TEST_ASSERT_EQUAL_INT(3, Rx_Buf.contador);
        TEST_ASSERT_EQUAL_INT(3, Rx_Buf.indice_fim);
        TEST_ASSERT_EQUAL_STRING("#2A", (char *)Rx_Buf.p_dados);
    }

/**
 * \test Test if rxChar rejects invalid characters.
 */
    // Testar se rxChar rejeita  um carácter inválido
    void test_rxChar_invalido()
    {
        resetRxBuffer();

        int ret = rxChar('\0');
        TEST_ASSERT_EQUAL_INT(-1, ret);

        TEST_ASSERT_EQUAL_INT(0, Rx_Buf.contador);
        TEST_ASSERT_EQUAL_INT(0, Rx_Buf.indice_fim);
        TEST_ASSERT_EQUAL_STRING("", (char *)Rx_Buf.p_dados);
    }

/**
 * \test Test RX buffer overflow behavior.
 */
    // Testar quando existe overflow no buffer Rx
    void test_rxChar_overflow()
    {
        resetRxBuffer();  

        int data = 0;


        for (int i = 0; i < UART_RX_SIZE; i++) {

            data = rxChar('K');
            TEST_ASSERT_EQUAL_INT(0, data); 

        }

        // Tentar inserir mais um , que vai dar erro
        data = rxChar('N');
        TEST_ASSERT_EQUAL_INT(-1, data);
    }


//TX

/**
 * \test Test if resetTxBuffer correctly clears the TX buffer.
 */

    // Testar se a função resetTxBuffer limpa o buffer de transmissão 
    void test_resetTxBuffer()
    {

        txChar('X');
        txChar('Y');
        txChar('Z');

        unsigned char tx[64] = {0};
        int len = 0;

        getTxBuffer(tx, &len);

        TEST_ASSERT_EQUAL_INT(3, len);
        TEST_ASSERT_EQUAL_CHAR('X', tx[0]);
        TEST_ASSERT_EQUAL_CHAR('Y', tx[1]);
        TEST_ASSERT_EQUAL_CHAR('Z', tx[2]);

        
        resetTxBuffer();

    
        getTxBuffer(tx, &len);

        TEST_ASSERT_EQUAL_INT(0, len);             
    }

/**
 * \test Test insertion of characters into the TX buffer.
 */
    //Testa se  são inseridos caracteres no  buffer TX
    void test_txChar_top()
    {
        resetTxBuffer();  

        int ret1 = txChar('O');
        int ret2 = txChar('L');
        int ret3 = txChar('A');

        TEST_ASSERT_EQUAL_INT(0, ret1);
        TEST_ASSERT_EQUAL_INT(0, ret2);
        TEST_ASSERT_EQUAL_INT(0, ret3);

        unsigned char tx[64] = {0};
        int len = 0;

        getTxBuffer(tx, &len);

        TEST_ASSERT_EQUAL_INT(3, len);              
        TEST_ASSERT_EQUAL_CHAR('O', tx[0]);
        TEST_ASSERT_EQUAL_CHAR('L', tx[1]);
        TEST_ASSERT_EQUAL_CHAR('A', tx[2]);
        
    }

/**
 * \test Test TX buffer overflow behavior.
 */
    //Testar o caso de overflow no buffer TX
    void test_txChar_overflow()
    {
        resetTxBuffer();  

        for (int i = 0; i < UART_TX_SIZE; i++) {

            TEST_ASSERT_EQUAL_INT(0, txChar('H'));
        }

        TEST_ASSERT_EQUAL_INT(-1, txChar('U'));              
    }


// -------------------- Command Processor Tests --------------------


/**
 * \test Test incomplete command, expected to return -1.
 */
    //Testar comando incompleto, suposto retornar -1

    void  test_cmdProcessor_incompleto(void)
    {

        rxChar('#');
        rxChar('P');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-1, retur);
    }

/**
 * \test Test invalid command letter, expected to return -2.
 */
    //Testar cmdProcessor com um comando inválido , deve retornar -2
    void  test_cmdProcessor_letra_comando_invalido()
    {

        rxChar('#');
        rxChar('H');  
        rxChar('!');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-2, retur);

    }

/**
 * \test Test command missing '!' at the end, expected to return -4.
 */
    // Testa se cmdProcessor retorna -4 com comando sem  !
    void test_cmdProcessor_faltando_ultimo_caractere(void)
    {

        rxChar('#');
        rxChar('A');  

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-4, retur);
    }

    //Testar Checksum

/**
 * \test Test checksum error in command #Pt, expected to return -3.
 */
        // Testar com checksum incorreto o comando #Pt na cmdProcessor  , deve retornar -3
        void test_cmdProcessor_checksum_errado_entrada()
        {

            rxChar('#');
            rxChar('P');
            rxChar('t');
            rxChar('9');
            rxChar('9');
            rxChar('9');
            rxChar('!');

            int result = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(-3, result);  
        }

/**
 * \test Test valid checksum for command #Pt196!
 */
        //Testar cmd proc com checksum correta para a entrada #pt!
        void test_cmdProcessor_checksum_valido_entrada()
        {
            resetRxBuffer();
            resetTxBuffer();

            
            rxChar('#');
            rxChar('P');
            rxChar('t');
            rxChar('1');
            rxChar('9');
            rxChar('6');
            rxChar('!');

            int result = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, result);  
        }




// -------------------- Sensor Reading and Reset Tests --------------------


/**
 * \test Read and reset temperature history.
 */
    // Testar a  leitura de temperaturas e dar reset
    void test_temperature_ler_temps_e_reset()
    {
        resetTemperatureHistory();  

    
        int8_t t1 = getNextTemperature();  
        int8_t t2 = getNextTemperature();  

        TEST_ASSERT_EQUAL_INT8(-50, t1);
        TEST_ASSERT_EQUAL_INT8(-40, t2);

        
        resetTemperatureHistory();

        
        int8_t t3 = getNextTemperature();  
        TEST_ASSERT_EQUAL_INT8(-50, t3);
    
    }


/**
 * \test Read and reset humidity history.
 */
    // Testar a  leitura de humidades e dar reset
    void test_humidity_ler_hums_e_reset()
    {
        resetHumidityHistory();

        int8_t h1 = getNextHumidity();  
        int8_t h2 = getNextHumidity();  

        TEST_ASSERT_EQUAL_INT8(50, h1);
        TEST_ASSERT_EQUAL_INT8(5, h2);

        resetHumidityHistory();
        int8_t h3 = getNextHumidity();  
        TEST_ASSERT_EQUAL_INT8(50, h3);
    }


/**
 * \test Read and reset CO2 history.
 */
    // Testar a  leitura de co2 e dar reset
    void test_co2_ler_co2_e_reset()
    {
        resetCO2History();

        int16_t c1 = getNextCO2();  
        int16_t c2 = getNextCO2();  

        TEST_ASSERT_EQUAL_INT16(400, c1);
        TEST_ASSERT_EQUAL_INT16(500, c2);

        resetCO2History();
        int16_t c3 = getNextCO2();  
        TEST_ASSERT_EQUAL_INT16(400, c3);
    }

// -------------------- Command 'A' Tests --------------------

    //Adicionar testes

/**
 * \test Test command 'A' to add data and check response.
 */
    void test_cmdProcessorA_add_data(){

        resetTemperatureHistory();
        resetHumidityHistory();
        resetCO2History();
    
        // Envia o comando #At181!
        rxChar('#');
        rxChar('A');
        rxChar('8');
        rxChar('9');
        rxChar('6');
        rxChar('!');
       

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, retur);
    
        unsigned char tx[64] = {0};
        int len = 0;
        getTxBuffer(tx, &len);
    
        TEST_ASSERT_EQUAL_STRING("#At-50h+50c+00400!", (char*)tx);
    }



// -------------------- Command 'P' Tests --------------------


/**
 * \test Test correct response to command #Pt196!
 */
    //caso t
        //Testar a resposta correta á entrada #pt196!
        void test_cmdProcessor_Pt_verificar_resposta_correta()
        {
            resetTemperatureHistory();  

            // Enviar comando #Pt196!
            rxChar('#');
            rxChar('P');
            rxChar('t');
            rxChar('1');
            rxChar('9');
            rxChar('6');
            rxChar('!');

            int retur = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, retur);

            unsigned char tx[64] = {0};
            int len = 0;

            getTxBuffer(tx, &len);

            TEST_ASSERT_EQUAL_STRING("#Pt-50086!", (char*)tx);  
        }

    //caso h


/**
 * \test Test correct response to command #Ph184!
 */
        // Verificar a  resposta correta ao comando #Ph184! 
        void test_cmdProcessor_Ph_verificar_resposta_correta()
        {
            resetHumidityHistory();  

            // // Enviar comando #Ph184!
            rxChar('#');
            rxChar('P');
            rxChar('h');
            rxChar('1');
            rxChar('8');
            rxChar('4');
            rxChar('!');

            int retur = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, retur);

            unsigned char tx[64] = {0};
            int len = 0;

            getTxBuffer(tx, &len);

            TEST_ASSERT_EQUAL_STRING("#Ph+50072!", (char*)tx);
        }

    //caso c

/**
 * \test Test correct response to command #Pc179!
 */
        // verificar a resposta correta ao comando #Pc179!
        void test_cmdProcessor_Pc_verificar_resposta_correta()
        {
            resetCO2History();  

            // Enviar comando #Pc179! 
            rxChar('#');
            rxChar('P');
            rxChar('c');
            rxChar('1');
            rxChar('7');
            rxChar('9');
            rxChar('!');

            int retur = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, retur);

            unsigned char tx[64] = {0};
            int len = 0;
            getTxBuffer(tx, &len);

            TEST_ASSERT_EQUAL_STRING("#Pc+00400210!", (char*)tx);
        }


// -------------------- Command 'L' Tests --------------------

    //Adicionar testes

/**
 * \test Test command 'L' to return last samples.
 */
    void test_cmdProcessorL_returns_last_samples() {
        // Reset dos históricos
        resetTemperatureHistory();
        resetHumidityHistory();
        resetCO2History();
        
        for (int i = 0; i < MAX_HISTORY; i++) {
            getNextTemperature();  
            getNextHumidity();     
            getNextCO2();          
        }
        
        // Envia o comando 'L' (assumindo que o comando 'L' não requer parâmetros e termina com '!')
        rxChar('#');
        rxChar('L');
        rxChar('!');
        
        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, ret);
        
        // Obter a resposta do buffer de transmissão
        unsigned char tx[256] = {0};
        int len = 0;
        getTxBuffer(tx, &len);      
        // TEST_ASSERT_EQUAL_STRING(expected, (char*)tx);
    }

// -------------------- Command 'R' Tests --------------------

    //Adicionar testes

/**
 * \test Test command 'R' to reset all histories.
 */
        void test_cmdProcessorR_reset_history(){
            
            resetTemperatureHistory();
            resetHumidityHistory();
            resetCO2History();

            // Simula o envio do comando #R!
            rxChar('#');
            rxChar('R');
            rxChar('!');
        
            int retur = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, retur);
        
            // Verifica se o buffer de transmissão contém a resposta "#R!"
            unsigned char tx[64] = {0};
            int len = 0;
            getTxBuffer(tx, &len);
            TEST_ASSERT_EQUAL_STRING("#R!", (char*)tx);
        
        }
 

/**
 * \brief Main function that runs the unit tests.
 *
 * Initializes the Unity test framework, runs the defined tests,
 * and returns the overall test status.
 *
 * \return int Returns the test status code.
 */

int main(void) {
    UNITY_BEGIN();
    printf("\n");

    //RX
    printf("------- Testes de Receção (RX) -------\n");
    RUN_TEST(test_resetRxBuffer);
    RUN_TEST(test_rxChar_valido);
    RUN_TEST(test_rxChar_invalido);
    RUN_TEST(test_rxChar_overflow);
    printf("\n");

    //TX
    printf("------- Testes de Transmissão (TX) -------\n");
    RUN_TEST(test_resetTxBuffer);
    RUN_TEST(test_txChar_top);
    RUN_TEST(test_txChar_overflow);
    printf("\n");

    //Verificar Comandos incompletos, inválido e sem o !
    printf("------- Testes de cmdProcessor  -------\n");
    RUN_TEST(test_cmdProcessor_incompleto);
    RUN_TEST(test_cmdProcessor_letra_comando_invalido);
    RUN_TEST(test_cmdProcessor_faltando_ultimo_caractere);
    printf("\n");

    //Checksum
    printf("------- Verificar Checksum  -------\n");
    RUN_TEST(test_cmdProcessor_checksum_valido_entrada);
    RUN_TEST(test_cmdProcessor_checksum_errado_entrada);
    printf("\n");

    //Leituras de dados e resets
    printf("------- leituras de dados e dar reset ( temp, hum e co2)  -------\n");
    RUN_TEST(test_temperature_ler_temps_e_reset);
    RUN_TEST(test_humidity_ler_hums_e_reset);
    RUN_TEST(test_co2_ler_co2_e_reset);
    printf("\n");

    //Comando A
    printf("------- Verificar comando 'A'  -------\n");
    RUN_TEST(test_cmdProcessorA_add_data);
    printf("\n");


    //Comando P
    printf("------- Verificar comando 'P'  -------\n");
        //Caso t
            //Verificar a resposta correta para o comando #pt196!
            RUN_TEST(test_cmdProcessor_Pt_verificar_resposta_correta);
    
        //caso h
            //Verificar a  resposta correta ao comando #Ph184! 
            RUN_TEST(test_cmdProcessor_Ph_verificar_resposta_correta);

        //caso c
            // verificar a resposta correta ao comando #Pc179!
            RUN_TEST(test_cmdProcessor_Pc_verificar_resposta_correta);
    printf("\n");

    //Comando 'L'

    printf("------- Verificar comando 'L'  -------\n");
    RUN_TEST(test_cmdProcessorL_returns_last_samples);
    printf("\n");

    //Comando 'R'
    printf("------- Verificar comando 'R'  -------\n");
    RUN_TEST(test_cmdProcessorR_reset_history);
    printf("\n");


    return UNITY_END();
}

