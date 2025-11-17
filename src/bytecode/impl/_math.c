#include "bytecode/impl/_math.h"

void iadd(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

void isub(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

void imul(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

void iinc(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    int32_t _const = (int32_t)f->method->bytecode.code[f->pc + 2];

    f->local_variables[idx].value.t._int += _const;
    f->pc += 3;
}