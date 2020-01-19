#ifdef UNIT_TEST

#include <unity.h>

int main(int argc, char **argv) {
    UNITY_BEGIN();
    TEST_ASSERT_EQUAL(1, 1);
    UNITY_END();
}

#endif