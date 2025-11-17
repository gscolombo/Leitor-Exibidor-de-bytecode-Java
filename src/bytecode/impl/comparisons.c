#include "bytecode/impl/comparisons.h"

static const enum Cond { EQ,
                         NE,
                         LT,
                         GE,
                         GT,
                         LE };

static int16_t icompare(enum Cond cond, const int32_t v1, const int32_t v2, const int16_t branch)
{
    switch (cond)
    {
    case EQ:
        if (v1 == v2)
            return branch;
        break;
    case NE:
        if (v1 != v2)
            return branch;
        break;
    case LT:
        if (v1 < v2)
            return branch;
        break;
    case GE:
        if (v1 >= v2)
            return branch;
        break;
    case GT:
        if (v1 > v2)
            return branch;
        break;
    case LE:
        if (v1 <= v2)
            return branch;
        break;
    default:
        break;
    }

    return 3;
}

void if_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 153;
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];
    int32_t value = pop_operand(f).value.t._int;

    f->pc += icompare(cond, value, 0, branch);
}

void if_icmp_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 159;
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];

    int32_t v2 = pop_operand(f).value.t._int;
    int32_t v1 = pop_operand(f).value.t._int;

    f->pc += icompare(cond, v1, v2, branch);
}