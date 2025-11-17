#include "bytecode/impl/conversions.h"

void i2T(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x85:
        result = initialize_var(LONG);
        result.value.t._long = (long)value;
        break;
    case 0x86:
        result = initialize_var(FLOAT);
        result.value.t._float = (float)value;
        break;
    case 0x87:
        result = initialize_var(DOUBLE);
        result.value.t._double = (double)value;
        break;
    case 0x91:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(int8_t)value;
        break;
    case 0x92:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(u2)value;
        break;
    case 0x93:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(int16_t)value;
        break;
    default:
        break;
    }

    push_operand(f, result);
    f->pc++;
}