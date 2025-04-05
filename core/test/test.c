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
    history_reset('r');
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

void test_UART_putc_Rx_valid(void)
{
    int ret1 = UART_putc_Rx('#');
    int ret2 = UART_putc_Rx('2');
    int ret3 = UART_putc_Rx('A');

    TEST_ASSERT_EQUAL_INT(0, ret1);
    TEST_ASSERT_EQUAL_INT(0, ret2);
    TEST_ASSERT_EQUAL_INT(0, ret3);

    TEST_ASSERT_EQUAL_INT(3, Rx_Buf.count);
    TEST_ASSERT_EQUAL_INT(3, Rx_Buf.tail);
    TEST_ASSERT_EQUAL_STRING("#2A", (char *)Rx_Buf.data);
}

void test_UART_putc_Rx_invalid(void)
{
    int ret = UART_putc_Rx('\0');
    TEST_ASSERT_EQUAL_INT(-1, ret);

    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.count);
    TEST_ASSERT_EQUAL_INT(0, Rx_Buf.tail);
    TEST_ASSERT_EQUAL_STRING("", (char *)Rx_Buf.data);
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

    RUN_TEST(test_UART_putc_Rx_valid);
    RUN_TEST(test_UART_putc_Rx_invalid);

    return UNITY_END();
}

