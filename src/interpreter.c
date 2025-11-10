#include "interpreter.h"

Frame *execute_method(Frame *f)
{
    u1 *code = f->method_code;
    u4 l = f->method_code_length;

    f->pc_register = 0;
    while (f->pc_register < l)
        opcode_table[code[f->pc_register]].exec(f);

    Frame *caller_frame = f->previous_frame;

    free(f->local_variables);
    free(f->operand_stack.stack);
    free(f);

    return caller_frame;
}

member_info *find_method(ClassFile *_class, const char *method_name)
{
    char *resolved_method_name = NULL;
    for (size_t i = 0; i < _class->methods_count; i++)
    {
        resolved_method_name = get_constant_UTF8_value(_class->methods[i].name_index, _class->constant_pool);
        if (!strcmp(resolved_method_name, method_name))
        {
            free(resolved_method_name);
            return &_class->methods[i];
        }
        free(resolved_method_name);
    }

    return NULL;
}

Frame *create_frame(ClassFile *_class, member_info *method, java_type *local_variables, Frame *previous_frame)
{
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    frame->runtime_cp = _class->constant_pool;

    frame->method = method;
    frame->method_code = method->attributes->info.Code.code;
    frame->method_code_length = method->attributes->info.Code.code_length;
    frame->max_locals = method->attributes->info.Code.max_locals;
    frame->max_stack = method->attributes->info.Code.max_stack;
    frame->local_variables = local_variables;
    frame->previous_frame = previous_frame;

    OperandStack op_stack;
    op_stack.top = -1;
    op_stack.capacity = frame->max_stack;
    op_stack.stack = (java_type *)calloc(frame->max_stack, sizeof(java_type));

    frame->operand_stack = op_stack;

    if (frame->operand_stack.stack != NULL)
        return frame;

    return NULL;
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

Frame *invoke_method(ClassFile *class, member_info *method, java_type *local_variables, Frame *caller, MethodArea *method_area)
{
    Frame *frame = create_frame(class, method, local_variables, caller);
    frame->method_area = method_area;
    return execute_method(frame);
}
