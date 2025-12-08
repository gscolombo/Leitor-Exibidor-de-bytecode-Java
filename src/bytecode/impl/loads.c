#include "bytecode/impl/loads.h"

void _load(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    push_operand(f, f->local_variables[idx]);
    f->pc += 2;
}

void iload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 26;
    push_operand(f, f->local_variables[n]);
    f->pc++;
}

void lload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 30;
    push_operand(f, f->local_variables[n]);

    f->pc++;
}

void dload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 38;
    push_operand(f, f->local_variables[n]);

    f->pc++;
}

void aload_n(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc] - 42;
    push_operand(f, f->local_variables[idx]);
    f->pc++;
}

void Taload(Frame *f)
{
    int32_t idx = pop_operand(f).value.t._int;
    ArrayRef arrayref = pop_operand(f).value.ref->value.array_ref.array;

    dtype v;
    u1 type = f->method->bytecode.code[f->pc] - 46;

    switch (type)
    {
    case 0:
        v = initialize_var(INT, f);
        v.value.t._int = ((int32_t *)arrayref.values)[idx];
        break;
    case 1:
        v = initialize_var(LONG, f);
        v.value.t._long = ((int64_t *)arrayref.values)[idx];
        break;
    case 2:
        v = initialize_var(FLOAT, f);
        v.value.t._float = ((float *)arrayref.values)[idx];
        break;
    case 3:
        v = initialize_var(DOUBLE, f);
        v.value.t._double = ((double *)arrayref.values)[idx];
        break;
    case 4:
        v = initialize_var(REFERENCE, f);
        *v.value.ref = ((reference *)arrayref.values)[idx];
        break;
    case 5:
        v = initialize_var(BYTE, f);
        v.value.t._int = ((int8_t *)arrayref.values)[idx];
        break;
    case 6:
        v = initialize_var(CHAR, f);
        v.value.t._int = ((u2 *)arrayref.values)[idx];
        break;
    case 7:
        v = initialize_var(SHORT, f);
        v.value.t._int = (int32_t)((int16_t *)arrayref.values)[idx];
        break;
    default:
        break;
    }

    push_operand(f, v);

    f->pc++;
}
