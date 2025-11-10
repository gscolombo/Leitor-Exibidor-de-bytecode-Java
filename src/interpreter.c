#include "interpreter.h"

Thread *initialize_thread(MethodArea *method_area)
{
    Thread *t = (Thread *)malloc(sizeof(Thread));

    t->pc_register = 0;
    t->current_frame = NULL;
    t->stack_size = 0;
    t->method_area = method_area;

    return t;
}

void execute_method(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u4 l = t->current_frame->method_code_length;

    t->pc_register = 0;
    while (t->pc_register < l)
        opcode_table[code[t->pc_register]].exec(t);

    pop_frame(t, t->current_frame);
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

void push_frame(Thread *t, Frame *frame)
{
    t->current_frame = frame;
    t->stack_size++;
}

void pop_frame(Thread *t, Frame *frame)
{
    t->current_frame = frame->previous_frame;
    t->stack_size--;

    free(frame->local_variables);
    free(frame->operand_stack.stack);
    free(frame);
}

void free_thread(Thread *t)
{
    while (t->stack_size > 0)
        pop_frame(t, t->current_frame);
    free(t);
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

void invoke_method(Thread *t, ClassFile *class, member_info *method, java_type *local_variables, Frame *caller)
{
    Frame *frame = create_frame(class, method, local_variables, caller);
    push_frame(t, frame);

    execute_method(t);
}
