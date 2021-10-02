#include <Arduino.h>
#include <unity.h>

void setUp(void) {

}

void tearDown(void) {

}

void test_one_equal_one(void) {
    TEST_ASSERT_EQUAL(1, 1);
}


void test_two_equal_two(void) {
    TEST_ASSERT_EQUAL(2, 2);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_one_equal_one);;
}

void loop() {
    RUN_TEST(test_two_equal_two);
    delay(500);
    UNITY_END(); // stop unit testing
}
