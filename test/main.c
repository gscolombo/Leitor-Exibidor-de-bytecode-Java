#include <CUnit/Basic.h>

#include "test_suites.h"

int main(void)
{
    if (CU_initialize_registry() == CUE_SUCCESS)
    {
        utils_ts();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    if (CU_get_error() == CUE_SUCCESS)
        CU_basic_run_tests();

    CU_cleanup_registry();

    return 0;
}