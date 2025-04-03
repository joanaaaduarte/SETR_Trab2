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
    // Initialization code before each test (if needed)
}

/**
 * @brief Teardown function called after each test.
 *
 * This function is used to clean up after tests, such as releasing resources.
 */
void tearDown(void) {
    // Cleanup code after each test (if needed)
}

/**
 * @brief Test for calcChecksum with a valid input buffer.
 *
 * Uses the buffer "Pt-40". For this buffer:
 * - 'P' = 80, 't' = 116, '-' = 45, '4' = 52, '0' = 48;
 * - Sum = 80 + 116 + 45 + 52 + 48 = 341;
 * - checksum = 341 % 256 = 85.
 *
 * @see calcChecksum()
 */
void test_calcChecksum_valid(void) {
    unsigned char buffer[] = "Pt-40";
    int result = calcChecksum(buffer, strlen((char *)buffer));
    TEST_ASSERT_EQUAL_INT(85, result);
}

/**
 * @brief Test for calcChecksum with an empty buffer.
 *
 * Verifies that the function returns 0 when the buffer is empty.
 *
 * @see calcChecksum()
 */
void test_calcChecksum_empty(void) {
    unsigned char buffer[] = "";
    int result = calcChecksum(buffer, 0);
    TEST_ASSERT_EQUAL_INT(0, result);
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
    
    RUN_TEST(test_calcChecksum_valid);
    RUN_TEST(test_calcChecksum_empty);
    
    return UNITY_END();
}

