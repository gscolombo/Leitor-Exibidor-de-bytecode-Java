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

void lsub(Frame *f)
{
    int64_t v2 = pop_operand(f).value.t._long;
    int64_t v1 = pop_operand(f).value.t._long;

    dtype result;
    result.value.t._long = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

void dadd(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

void dsub(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

void dmul(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

void ddiv(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = v1 / v2;
    push_operand(f, result);

    f->pc++;
}

void dneg(Frame *f)
{
    double value = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = -value;
    push_operand(f, result);

    f->pc++;
}

void drem(Frame *f)
{
    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    result.value.t._double = fmod(v1, v2);
    push_operand(f, result);

    f->pc++;
}
