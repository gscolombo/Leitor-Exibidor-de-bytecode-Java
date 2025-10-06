#include "test_suites.h"

void utils_ts(void)
{
    CU_pSuite ts = CU_add_suite("utils", NULL, NULL);
    if (ts != NULL)
    {
        CU_add_test(ts, "Test halfword swap function", &test_u2swap);
        CU_add_test(ts, "Test word swap function", &test_u4swap);
        CU_add_test(ts, "Test function to retrieve number of digits", &test_num_digits);
    }
}
