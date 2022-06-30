#include "lib.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void get_hello_str_test(void) {
  TEST_ASSERT_EQUAL_STRING(get_hello_str(), "Hello, World!");
}

typedef void (*Func)(void);

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(get_hello_str_test);

  return UNITY_END();
}
