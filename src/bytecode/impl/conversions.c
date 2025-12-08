#include "bytecode/impl/conversions.h"

void i2T(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x85:
        result = initialize_var(LONG, f);
        result.value.t._long = (long)value;
        break;
    case 0x86:
        result = initialize_var(FLOAT, f);
        result.value.t._float = (float)value;
        break;
    case 0x87:
        result = initialize_var(DOUBLE, f);
        result.value.t._double = (double)value;
        break;
    case 0x91:
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(int8_t)value;
        break;
    case 0x92:
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(u2)value;
        break;
    case 0x93:
        result = initialize_var(INT, f);
        result.value.t._int = (int32_t)(int16_t)value;
        break;
    default:
        break;
    }

    push_operand(f, result);
    f->pc++;
}

void d2T(Frame *f)
{
    double value = pop_operand(f).value.t._double;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x90:
        result = initialize_var(FLOAT, f);
        result.value.t._float = (float)value;
        break;
    case 0x8e:
        result = initialize_var(INT, f);
        if (value == NAN)
            result.value.t._int = 0;
        else if (value < INT32_MIN)
            result.value.t._int = INT32_MIN;
        else if (value > INT32_MAX)
            result.value.t._int = INT32_MAX;
        else
            result.value.t._int = (int32_t)value;
        break;
    case 0x8f:
        result = initialize_var(LONG, f);
        if (value == NAN)
            result.value.t._long = 0;
        else if (value < INT64_MIN)
            result.value.t._long = INT64_MIN;
        else if (value > INT64_MAX)
            result.value.t._long = INT64_MAX;
        else
            result.value.t._long = (int64_t)value;
        break;
    default:
        break;
    }

    push_operand(f, result);
    f->pc++;
}