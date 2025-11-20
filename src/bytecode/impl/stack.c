#include "bytecode/impl/stack.h"

void dup(Frame *f)
{
    push_operand(f, f->operand_stack.stack[f->operand_stack.top]);
    f->pc++;
}

void dup2(Frame *f)
{
    dtype value1 = f->operand_stack.stack[f->operand_stack.top];
    if (value1.cat == CAT1)
    {
        dtype value2 = f->operand_stack.stack[f->operand_stack.top - 1];
        push_operand(f, value2);
        push_operand(f, value1);
    }
    else
        push_operand(f, value1);

    f->pc++;
}

void pop_(Frame *f)
{
    (void)pop_operand(f);
    
    if (f->method->bytecode.code[f->pc] == 0x58)
        (void)pop_operand(f);

    f->pc++;
}