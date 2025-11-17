#include "interpreter.h"

void execute_method(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 l = f->method->bytecode.code_length;

    while (f->pc < l)
    {
        printf("%u: %s\n", f->pc, opcode_table[code[f->pc]].mnemonic);
        opcode_table[code[f->pc]].exec(f);
    }
}

inline void push_operand(Frame *f, dtype d)
{
    f->operand_stack.top++;
    f->operand_stack.stack[f->operand_stack.top] = d;
}

dtype pop_operand(Frame *f)
{
    dtype d = f->operand_stack.stack[f->operand_stack.top];
    f->operand_stack.top--;
    return d;
}

dtype initialize_var(type_enum t)
{
    dtype var;
    switch (t)
    {
    case LONG:
    case DOUBLE:
        var.cat = CAT2;
        break;
    default:
        var.cat = CAT1;
        break;
    }

    return var;
}

void invoke_method(Class *class, Method *method, dtype *local_variables, Frame *caller, MethodArea *method_area)
{
    // Create frame...
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    frame->pc = 0;
    frame->local_variables = local_variables;
    frame->previous_frame = caller;
    frame->method = method;           // current method
    frame->class = class;             // current class
    frame->method_area = method_area; // pointer to global method area

    OperandStack op_stack;
    op_stack.top = -1;
    op_stack.capacity = frame->method->bytecode.max_stack;
    op_stack.stack = (dtype *)calloc(frame->method->bytecode.max_stack, sizeof(dtype));

    frame->operand_stack = op_stack;

    // ...then execute current method...
    if (frame->operand_stack.stack != NULL && frame->local_variables != NULL)
        execute_method(frame);

    // ...and free allocated memory
    for (u4 k = 0; k < frame->method->ref_count; k++)
        free(frame->method->refs[k]);
    free(frame->method->refs);
    frame->method->ref_count = 0; // Reset ref count

    free(frame->local_variables);
    free(frame->operand_stack.stack);
    free(frame);
}
