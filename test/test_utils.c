#include "test_utils.h"

void test_u2swap(void)
{
    CU_ASSERT(u2swap(0x1234) == 0x3412);
    CU_ASSERT(u2swap(0x0101) == 0x0101);
    CU_ASSERT(u2swap(0x0001) == 0x0100);
}

void test_u4swap(void)
{
    CU_ASSERT(u4swap(0x12345678) == 0x78563412);
    CU_ASSERT(u4swap(0x01010101) == 0x01010101);
    CU_ASSERT(u4swap(0x00000001) == 0x01000000);
}

void test_num_digits(void)
{
    CU_ASSERT(num_digits(10) == 2);
    CU_ASSERT(num_digits(1234) == 4);
    CU_ASSERT(num_digits(1) == 1);
    CU_ASSERT(num_digits(0) == 1);
    CU_ASSERT(num_digits(1927081723) == 10);
}
