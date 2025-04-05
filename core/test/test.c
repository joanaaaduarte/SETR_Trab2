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
}

/**
 * @brief Teardown function called after each test.
 *
 * This function is used to clean up after tests, such as releasing resources.
 */
void tearDown(void) {
    // Cleanup code after each test (if needed)
}


// UART Tests

// Testar se resetRxBuffer limpa o buffer corretamente
void test_resetRxBuffer(void)
{

    rxChar('Z');  

    TEST_ASSERT_EQUAL_INT(1, Rx_Buf.count);

    resetRxBuffer();

    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.count);
    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.tail);
    TEST_ASSERT_EQUAL_CHAR('\0', Rx_Buf.buffer[0]);
}

//Testar se rxChar insere caracteres válidos no buffer Rx
void test_rxChar_valid(void)
{
    resetRxBuffer();

    int ret1 = rxChar('#');
    int ret2 = rxChar('2');
    int ret3 = rxChar('A');

    TEST_ASSERT_EQUAL_INT(0, ret1);
    TEST_ASSERT_EQUAL_INT(0, ret2);
    TEST_ASSERT_EQUAL_INT(0, ret3);

    TEST_ASSERT_EQUAL_INT(3, Rx_Buf.count);
    TEST_ASSERT_EQUAL_INT(3, Rx_Buf.tail);
    TEST_ASSERT_EQUAL_STRING("#2A", (char *)Rx_Buf.data);
}

// Testar se rxChar rejeita  um carácter inválido
void test_rxChar_invalid(void)
{
    resetRxBuffer();

    int ret = rxChar('\0');
    TEST_ASSERT_EQUAL_INT(-1, ret);

    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.count);
    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.tail);
    TEST_ASSERT_EQUAL_STRING("", (char *)Rx_Buf.data);
}

// Testar quando existe overflow no buffer Rx
void test_rxChar_overflow(void)
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

    //Rx
    RUN_TEST(test_resetRxBuffer);
    RUN_TEST(test_rxChar_valid);
    RUN_TEST(test_rxChar_invalid);
    RUN_TEST(test_rxChar_overflow);


    return UNITY_END();
}

