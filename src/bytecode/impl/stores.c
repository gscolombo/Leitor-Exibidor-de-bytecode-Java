#include "bytecode/impl/stores.h"

void istore(Frame *f)
{
    int32_t idx = f->method->bytecode.code[f->pc + 1];
    int32_t value = pop_operand(f).value.t._int;

    f->local_variables[idx].value.t._int = value;
    f->pc += 2;
}

void astore(Frame *f)
{
    reference *objectref = pop_operand(f).value.ref;
    u1 idx = f->method->bytecode.code[f->pc + 1];

    f->local_variables[idx].value.ref = objectref;
    f->pc += 2;
}

void istore_n(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    int idx = f->method->bytecode.code[f->pc] - 59;

    f->local_variables[idx].value.t._int = value;
    f->pc++;
}

void dstore_n(Frame *f)
{
    (void)pop_operand(f).value.t._double;
    double value = pop_operand(f).value.t._double;
    int idx = f->method->bytecode.code[f->pc] - 71;

    f->local_variables[idx].value.t._double = value;
    f->pc++;
}

void astore_n(Frame *f)
{
    reference *r = pop_operand(f).value.ref;
    int idx = f->method->bytecode.code[f->pc] - 75;

    f->local_variables[idx].value.ref = r;
    f->pc++;
}

void Tastore(Frame *f)
{
    u1 type = f->method->bytecode.code[f->pc] - 79;
    dtype op = pop_operand(f);

    if (op.cat == CAT2)
        (void)pop_operand(f);

    int32_t idx = pop_operand(f).value.t._int;
    ArrayRef arrayref = pop_operand(f).value.ref->value.array_ref.array;
    
    if (arrayref.values)
        switch (type)
        {
        case 0:
            ((int32_t *)arrayref.values)[idx] = op.value.t._int;
            break;
        case 1:
            ((int64_t *)arrayref.values)[idx] = op.value.t._long;
            break;
        case 2:
            ((float *)arrayref.values)[idx] = op.value.t._float;
            break;
        case 3:
            ((double *)arrayref.values)[idx] = op.value.t._double;
            break;
        case 4:
            ((reference *)arrayref.values)[idx] = *op.value.ref;
            break;
        case 5:
            ((int8_t *)arrayref.values)[idx] = op.value.t.byte;
            break;
        case 6:
            ((u2 *)arrayref.values)[idx] = op.value.t._char;
            break;
        case 7:
            ((int16_t *)arrayref.values)[idx] = op.value.t._short;
            break;
        default:
            break;
        }

    f->pc++;
}