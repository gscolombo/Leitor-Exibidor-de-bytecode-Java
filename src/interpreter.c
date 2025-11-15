#include "interpreter.h"

Frame *execute_method(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 l = f->method->bytecode.code_length;

    while (f->pc < l)
        opcode_table[code[f->pc]].exec(f);

    Frame *caller_frame = f->previous_frame;

    free(f->local_variables);
    free(f->operand_stack.stack);
    free(f);

    return caller_frame;
}

inline void push_operand(Frame *f, java_type val)
{
    f->operand_stack.top++;
    f->operand_stack.stack[f->operand_stack.top] = val;
}

java_type pop_operand(Frame *f)
{
    java_type val = f->operand_stack.stack[f->operand_stack.top];
    f->operand_stack.top--;
    return val;
}

Frame *invoke_method(Class *class, Method *method, java_type *local_variables, Frame *caller, MethodArea *method_area)
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
    op_stack.stack = (java_type *)calloc(frame->method->bytecode.max_stack, sizeof(java_type));

    frame->operand_stack = op_stack;

    // ...then execute current method
    if (frame->operand_stack.stack != NULL)
        return execute_method(frame);

    return NULL;
}
