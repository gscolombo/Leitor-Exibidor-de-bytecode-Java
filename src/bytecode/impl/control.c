#include "bytecode/impl/control.h"

void _goto(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    f->pc += (int16_t)((code[f->pc + 1] << 8) | code[f->pc + 2]);
}

void tableswitch(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 start = f->pc;

    while ((++f->pc) % 4 != 0)
        continue;

    int32_t _default = get_tableswitch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t low = get_tableswitch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t high = get_tableswitch_32B_values(f->pc, code);
    f->pc += 4;

    int32_t i = pop_operand(f).value.t._int;

    if (i < low || i > high)
        f->pc = start + _default;
    else
        f->pc = start + get_tableswitch_32B_values(f->pc + 4 * i, code);
}

void ireturn(Frame *f)
{
    dtype ret;
    ret = pop_operand(f);
    push_operand(f->previous_frame, ret);
    f->pc = f->method->bytecode.code_length;
}

void _return(Frame *f)
{
    f->pc = f->method->bytecode.code_length;
}