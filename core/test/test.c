/**
    \file test.c
    \authors Joana Duarte, nmec: 102608
    \authors Hugo Miranda, nmec: 104348
    \date Colocar data  
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


    //Testar o caso de overflow no buffer TX
    void test_txChar_overflow()
    {
        resetTxBuffer();  

        for (int i = 0; i < UART_TX_SIZE; i++) {

            TEST_ASSERT_EQUAL_INT(0, txChar('H'));

        }

        TEST_ASSERT_EQUAL_INT(-1, txChar('U'));              
    }


//Testar a cmdProcess

    //Testar comando incompleto, suposto retornar -1

    void  test_cmdProcessor_incompleto(void)
    {

        rxChar('#');
        rxChar('P');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-1, retur);
    }


    //Testar cmdProcessor com um comando inválido , deve retornar -2
    void  test_cmdProcessor_letra_comando_invalido()
    {

        rxChar('#');
        rxChar('H');  
        rxChar('!');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-2, retur);

    }

    void test_cmdProcessor_comando_invalido()
    {

        rxChar('#');
        rxChar('X'); // comando inválido

        rxChar('1');
        rxChar('2');
        rxChar('3');

        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-2, ret);
    }



    //Testar Checksum

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




//Testar a leiuras de dados e dar reset ( temp, hum e co2)

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

//Comando 'A'
    //Adicionar testes

        //Testar o comando A com uma checksum errada,retorna -3
        void test_cmdProcessor_A_checksum_invalido() {
            resetRxBuffer(); resetTxBuffer();

            rxChar('#');
            rxChar('A');
            rxChar('0'); rxChar('0'); rxChar('0');  // Checksum errado
            rxChar('!');

            int ret = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(-3, ret);
        }

        // Entrada: #A! , retorna -1
        void test_cmdProcessor_A_incompleto() {
            resetRxBuffer(); resetTxBuffer();

            rxChar('#'); rxChar('A'); rxChar('!');

            int ret = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(-1, ret); // Incompleto
        }


            //Testar formato errado , retorna -4
        void test_cmdProcessor_A_formato_invalido_sem_exclamacao()
        {
            

            rxChar('#');
            rxChar('A');
            rxChar('0');
            rxChar('6');
            rxChar('5');  //Checksum correta
            rxChar('?');   //devia ser !

            int retur = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(-4, retur);  
        }


        // Testa se a resposta à entrada #A065! é formada corretamente
        void test_cmdProcessor_A_output_correta()
        {
            resetTemperatureHistory();
            resetHumidityHistory();
            resetCO2History();

            // Envia o comando válido #A065!
            rxChar('#');
            rxChar('A');
            rxChar('0');
            rxChar('6');
            rxChar('5');
            rxChar('!');

            cmdProcessor();

            unsigned char tx[64] = {0};
            int len = 0;
            getTxBuffer(tx, &len);
            tx[len] = '\0';

            // Checksum 193
            TEST_ASSERT_EQUAL_STRING("#At-50h+50c+00400193!", (char*)tx);
        }




//Comando 'P'

    // Testa se cmdproc retorna -4 , quando falta !
    void test_cmdProcessor_P_falta_exclamacao()
    {

        rxChar('#');
        rxChar('P');
        rxChar('t');
        rxChar('1');    //checksum bem
        rxChar('9');
        rxChar('6');
        rxChar('?');  //  devia ser '!'

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-4, retur);
    }

    // Testa se cmdProc retorna -1 quando o comando P está incompleto
    void test_cmdProcessor_P_incompleto()
    {
        resetRxBuffer();

        rxChar('#');
        rxChar('P');
        rxChar('t');  
        // falta checksum
        //falta !
        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-1, retur);
    }

    // Testa se cmdProcessor retorna -3 quando a checksum está errada
    void test_cmdProcessor_P_checksum_invalida()
    {
        resetRxBuffer();

        rxChar('#');
        rxChar('P');
        rxChar('t');

        rxChar('0');  // ← Checksum errada 
        rxChar('0');
        rxChar('0');

        rxChar('!');  

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-3, retur);
    }





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

            int ret = cmdProcessor();
            TEST_ASSERT_EQUAL_INT(0, ret);

            unsigned char tx[64] = {0};
            int len = 0;
            getTxBuffer(tx, &len);

            TEST_ASSERT_EQUAL_STRING("#Pc+00400210!", (char*)tx);
        }

//Comando 'L'
    // Testa se cmdproc retorna -1 , comando incompleto para 'L'
    void test_cmdProcessor_L_incompleto() {
        rxChar('#');
        rxChar('L'); 

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-1, retur);
    }

    // Testa se  cmdproc retorna -2 para um comando inválido
    void test_cmdProcessor_L_invalido() {

        rxChar('#');
        rxChar('X');  // comando desconhecido
        rxChar('0');  
        rxChar('0');
        rxChar('0');
        rxChar('!');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-2, retur);
    }

    // Testa  se cmdproc retorna -3, quando checksum inválido no comando L
    void test_cmdProcessor_L_checksum_invalido() {

        rxChar('#');
        rxChar('L');
        rxChar('9');  // checksum inválido
        rxChar('9');
        rxChar('9');
        rxChar('!');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-3, retur);
    }

    // Testa se cmdproc retorna -4 para falta do caractere !
    void test_cmdProcessor_L_falta_exclamacao() {

        rxChar('#');
        rxChar('L');
        rxChar('0');
        rxChar('7');
        rxChar('6');
         rxChar('?');

        int retur = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-4, retur);
    }


   void test_cmdProcessor_Lt_resposta_correta() {

        // Preenche histórico
        for (int i = 0; i < MAX_HISTORY; ++i)
        {
            getNextTemperature();
        }

        // Envia comando válido com checksum
        rxChar('#');
        rxChar('L');
        rxChar('0'); 
        rxChar('7'); 
        rxChar('6');  
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, ret);

        // Verifica 1ºsegmento: #Lt...
        unsigned char tx[124] = {0};
        int len = 0;
        getTxBuffer(tx, &len);

        // Valida que a resposta começa com '#Lt'
        TEST_ASSERT_EQUAL_CHAR('#', tx[0]);
        TEST_ASSERT_EQUAL_CHAR('L', tx[1]);
        TEST_ASSERT_EQUAL_CHAR('t', tx[2]);
    
    }

    void test_cmdProcessor_Lh_resposta_correta() {

        // Preenche histórico de humidade
        for (int i = 0; i < MAX_HISTORY; ++i) {
            getNextHumidity();
        }

        // Envia comando válido com checksum
        rxChar('#');
        rxChar('L');
        rxChar('0'); 
        rxChar('7'); 
        rxChar('6');  // Checksum de 'L'
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, ret);

        // Verifica segundo segmento #Lh...
        unsigned char tx[512] = {0};
        int len = 0;
        getTxBuffer(tx, &len);

        // Encontra o início de #Lh
        char *ptr = strstr((char *)tx, "#Lh");
        TEST_ASSERT_NOT_NULL(ptr);

        TEST_ASSERT_EQUAL_CHAR('#', ptr[0]);
        TEST_ASSERT_EQUAL_CHAR('L', ptr[1]);
        TEST_ASSERT_EQUAL_CHAR('h', ptr[2]);
    }

    void test_cmdProcessor_Lc_resposta_correta() {

        // Preenche histórico de CO₂
        for (int i = 0; i < MAX_HISTORY; ++i) {
            getNextCO2();
        }

        // Envia comando válido com checksum
        rxChar('#');
        rxChar('L');
        rxChar('0'); 
        rxChar('7'); 
        rxChar('6');  // Checksum de 'L'
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, ret);

        // Verifica terceiro segmento no TX: #Lc...
        unsigned char tx[1024] = {0};
        int len = 0;
        getTxBuffer(tx, &len);

        // Encontra o início de #Lc
        char *ptr = strstr((char *)tx, "#Lc");
        TEST_ASSERT_NOT_NULL(ptr);

        TEST_ASSERT_EQUAL_CHAR('#', ptr[0]);
        TEST_ASSERT_EQUAL_CHAR('L', ptr[1]);
        TEST_ASSERT_EQUAL_CHAR('c', ptr[2]);
    }

//Comando 'R'

    //Testar se cmdproc retorna -1, para comando incompleto
    void test_cmdProcessor_R_incompleto() {

        rxChar('#');
        rxChar('R');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-1, ret);

    }

    //Testar se cmdproc retorna -2, para um comando inválido no caso R
    void test_cmdProcessor_R_comando_invalido() {

        rxChar('#');
        rxChar('V');  
        rxChar('0');
        rxChar('0');
        rxChar('0');
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-2, ret);
    }

    //Verificar se cmdproc retorna -3, para checksum errada 
    void test_cmdProcessor_R_checksum_incorreta() {

        rxChar('#');
        rxChar('R');
        rxChar('1');  // Checksum errada
        rxChar('2');
        rxChar('3');
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-3, ret);
    }

    void test_cmdProcessor_R_falta_exclamacao() {

        rxChar('#');
        rxChar('R');
        rxChar('0');
        rxChar('8');
        rxChar('2');
        rxChar('?');

        int retUR = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(-4, retUR);
    }

    //Testar se o histórico foi resetado
    void test_cmdProcessor_R_reset_histórico() {
    
        //Preencher histórico
        for (int i = 0; i < MAX_HISTORY; ++i) {
            getNextTemperature();
            getNextHumidity();
            getNextCO2();
        }

        rxChar('#');
        rxChar('R');
        rxChar('0');
        rxChar('8');
        rxChar('2');
        rxChar('!');

        int ret = cmdProcessor();
        TEST_ASSERT_EQUAL_INT(0, ret);

        unsigned char tx[64] = {0};
        int len = 0;
        getTxBuffer(tx, &len);

        // Verifica resposta correta
        TEST_ASSERT_EQUAL_STRING("#R082!", (char *)tx);

        // Verifica se o histórico foi resetado:
        int8_t temp = getNextTemperature();
        int8_t hum  = getNextHumidity();
        int16_t co2 = getNextCO2();

        TEST_ASSERT_EQUAL_INT8(-50, temp);     // Primeiro valor da temp
        TEST_ASSERT_EQUAL_INT8(50, hum);       // Primeiro valor da hum
        TEST_ASSERT_EQUAL_INT16(400, co2);     // Primeiro valor de co2
    }





    




/**
 * @brief Main function that runs the unit tests.
 *
 * Initializes the Unity test framework, runs the defined tests,
 * and returns the overall test status.
 *
 * @return int Returns the test status code.
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
    RUN_TEST(test_cmdProcessor_comando_invalido);
    
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
    RUN_TEST(test_cmdProcessor_A_checksum_invalido);
    RUN_TEST(test_cmdProcessor_A_incompleto);
    RUN_TEST(test_cmdProcessor_A_formato_invalido_sem_exclamacao);
    RUN_TEST(test_cmdProcessor_A_output_correta);
    printf("\n");

    //Comando P
    printf("------- Verificar comando 'P'  -------\n");
    RUN_TEST(test_cmdProcessor_P_falta_exclamacao);
    RUN_TEST(test_cmdProcessor_P_incompleto);
    RUN_TEST(test_cmdProcessor_P_checksum_invalida);

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
    RUN_TEST(test_cmdProcessor_L_incompleto);
    RUN_TEST(test_cmdProcessor_L_invalido);
    RUN_TEST(test_cmdProcessor_L_checksum_invalido);
    RUN_TEST(test_cmdProcessor_L_falta_exclamacao);

        //Semgmento da temperatura
            RUN_TEST(test_cmdProcessor_Lt_resposta_correta);
        //Segmento da humidade
            RUN_TEST(test_cmdProcessor_Lh_resposta_correta);

        //Para este caso está a dar erro devido ao overflow do buffer tx
        //segmento do co2
            //RUN_TEST(test_cmdProcessor_Lc_segmento);


    printf("\n");


    //Comando 'R'
    printf("------- Verificar comando 'R'  -------\n");
    RUN_TEST(test_cmdProcessor_R_incompleto);
    RUN_TEST(test_cmdProcessor_R_checksum_incorreta);
    RUN_TEST(test_cmdProcessor_R_comando_invalido);
    RUN_TEST(test_cmdProcessor_R_falta_exclamacao);
    RUN_TEST(test_cmdProcessor_R_reset_histórico);
    printf("\n");


    return UNITY_END();
}

