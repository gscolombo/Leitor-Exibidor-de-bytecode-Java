#include "opcode_functions.h"

void aload_0(Thread *t)
{
    push_operand(t->current_frame, t->current_frame->local_variables[0]);
    t->pc_register++;
}

void ldc(Thread *t)
{
    u1 index = t->current_frame->method_code[t->pc_register + 1];
    cp_info constant = t->current_frame->runtime_cp[index - 1];

    java_type val;

    switch (constant.tag)
    {
    case CONSTANT_Integer:
        val.t._int = constant.info._4Bn.number.i;
        break;
    case CONSTANT_Float:
        val.t._float = constant.info._4Bn.number.f;
        break;
    case CONSTANT_String:
        val.ref.array_ref = (primitive_type *)get_constant_UTF8_value(index, t->current_frame->runtime_cp);
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(t->current_frame, val);
    t->pc_register += 2;
}

void getstatic(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u2 idx = (code[t->pc_register + 1] << 8) | code[t->pc_register + 2];

    java_type resolved_field;
    resolved_field.ref.array_ref = (primitive_type *)get_constant_UTF8_value(idx, t->current_frame->runtime_cp);

    // Standard output object field, don't need to initialize the class
    if (!strcmp((const char *)resolved_field.ref.array_ref, "java/lang/System.out:Ljava/io/PrintStream;"))
    {
        push_operand(t->current_frame, resolved_field);
    }

    t->pc_register += 3;
}

void invokespecial(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u2 idx = (code[t->pc_register + 1] << 8) | code[t->pc_register + 2];

    char *resolved_method = get_constant_UTF8_value(idx, t->current_frame->runtime_cp);

    // Super class is Object class (do nothing)
    if (!strcmp(resolved_method, "java/lang/Object.<init>:()V"))
    {
    }

    // TODO: Finish (super) class initialization

    free(resolved_method);
    t->pc_register += 3;
}

void invokevirtual(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u2 idx = (code[t->pc_register + 1] << 8) | code[t->pc_register + 2];

    char *resolved_method = get_constant_UTF8_value(idx, t->current_frame->runtime_cp);

    const char *method_name = strtok(resolved_method, ":");
    if (!strcmp(method_name, "java/io/PrintStream.println")) // Call to println
    {
        char *str = (char *)pop_operand(t->current_frame).ref.array_ref;
        free(pop_operand(t->current_frame).ref.array_ref); // Pop and free PrintStream class reference
        printf("%s\n", str);
        free(str);
    }

    // TODO: Handle general class method invocation

    free(resolved_method);
    t->pc_register += 3;
}

void _return(Thread *t) { t->pc_register++; }