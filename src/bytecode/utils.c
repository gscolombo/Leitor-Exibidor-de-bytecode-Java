#include "bytecode/utils.h"

inline extern int32_t get_tableswitch_32B_values(u4 i, const u1 *code)
{
    return (code[i] << 24) | (code[i + 1] << 16) | (code[i + 2] << 8) | code[i + 3];
}
