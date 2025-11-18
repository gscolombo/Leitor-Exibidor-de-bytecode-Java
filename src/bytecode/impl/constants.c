#include "bytecode/impl/constants.h"

void iconst_i(Frame *f)
{
    dtype i = initialize_var(INT);
    i.value.t._int = (int32_t)f->method->bytecode.code[f->pc] - 3;
    push_operand(f, i);
    f->pc++;
}

void fconst_f(Frame *f)
{
    dtype _f = initialize_var(FLOAT);
    _f.value.t._float = (float)(f->method->bytecode.code[f->pc] - 11);
    push_operand(f, _f);
    f->pc++;
}

void dconst_d(Frame *f)
{
    dtype d = initialize_var(DOUBLE);
    d.value.t._double = (float)(f->method->bytecode.code[f->pc] - 14);
    push_operand(f, d);
    f->pc++;
}

void bipush(Frame *f)
{
    dtype bi = initialize_var(BYTE);
    bi.value.t._int = (int8_t)f->method->bytecode.code[f->pc + 1];
    push_operand(f, bi);
    f->pc += 2;
}

void sipush(Frame *f)
{
    dtype si = initialize_var(SHORT);
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];
    si.value.t._int = (int32_t)((b1 << 8) | b2);

    push_operand(f, si);
    f->pc += 3;
}

void ldc_(Frame *f)
{
    u1 mode = f->method->bytecode.code[f->pc] - 18;

    dtype val;

    u2 index;
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];

    index = mode > 0 ? (b1 << 8) | b2 : b1;

    RuntimeConstant c = f->class->runtime_cp[index - 1];

    switch (c.type)
    {
    case CONSTANT_Integer:
        val = initialize_var(INT);
        val.value.t._int = c.value.i;
        break;
    case CONSTANT_Float:
        val = initialize_var(FLOAT);
        val.value.t._float = c.value.f;
        break;
    case CONSTANT_Long:
        val = initialize_var(LONG);
        val.value.t._long = c.value.l;
        push_operand(f, val);
        break;
    case CONSTANT_Double:
        val = initialize_var(DOUBLE);
        val.value.t._double = c.value.d;
        push_operand(f, val);
        break;
    case CONSTANT_String:
        val = initialize_var(REFERENCE);
        val.value.ref.array_ref.string = c.value.strref;
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(f, val);
    f->pc += mode > 0 ? 3 : 2;
}