#include "opcode_functions.h"

void aload_0(Thread *t)
{
    push_operand(t->current_frame, t->current_frame->local_variables[0]);
    t->pc_register++;
}

void iload_n(Thread *t)
{
    u1 n = t->current_frame->method_code[t->pc_register] - 26;
    push_operand(t->current_frame, t->current_frame->local_variables[n]);
    t->pc_register++;
}

void iconst_i(Thread *t)
{
    java_type i;
    i.t._int = (int32_t)t->current_frame->method_code[t->pc_register] - 3;
    push_operand(t->current_frame, i);
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

    // Super class is Object class (do nothing, except for popping the objectref value)
    if (!strcmp(resolved_method, "java/lang/Object.<init>:()V"))
        pop_operand(t->current_frame);

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
        char *descriptor = strtok(NULL, ":");
        char rettype = *(descriptor + 1);
        switch (rettype)
        {
        case 'L':
            if (!strcmp(descriptor, "(Ljava/lang/String;)V"))
            {
                char *str = (char *)pop_operand(t->current_frame).ref.array_ref;
                printf("%s\n", str);
                free(str);
            }
            // TODO: Define logic for class references
            break;
        case 'I':
            int32_t i = pop_operand(t->current_frame).t._int;
            printf("%i\n", i);
            break;
        // TODO: Define logic for other cases
        default:
            break;
        }

        free(pop_operand(t->current_frame).ref.array_ref); // Pop and free PrintStream class reference
    }

    // TODO: Handle general class method invocation

    free(resolved_method);
    t->pc_register += 3;
}

void invokestatic(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u2 idx = (code[t->pc_register + 1] << 8) | code[t->pc_register + 2];

    u2 method_class_idx = t->current_frame->runtime_cp[idx - 1].info.Ref.class_index;
    char *class_name = get_constant_UTF8_value(method_class_idx, t->current_frame->runtime_cp);
    ClassFile *class = bootstrap_loader(NULL, t->method_area, class_name);

    if (class)
    {
        char *resolved_method = get_constant_UTF8_value(idx, t->current_frame->runtime_cp);
        const char *resolved_method_name = strtok(resolved_method + strlen(class_name) + 1, ":"); // Get only method name

        member_info *method = find_method(class, resolved_method_name);

        free(class_name);
        free(resolved_method);

        if (method)
        {
            if (!(method->access_flags & 0x0008)) // Check if method is static
            {
                printf("Error: method is not static.\n");
                exit(1);
            }
        }
        else
        {
            printf("Error: method not found.\n");
            exit(1);
        }

        u2 nargs = method->attributes->info.Code.max_locals;
        java_type *local_variables = (java_type *)calloc(nargs, sizeof(java_type));

        for (u2 i = 0; i < nargs; i++)
            local_variables[i] = pop_operand(t->current_frame);

        Frame *frame = create_frame(class, method, local_variables, t->current_frame);
        push_frame(t, frame);
        u4 last_pc = t->pc_register;
        execute_method(t);
        t->pc_register = last_pc + 3;
    }
    else
    {
        printf("Error loading class.\n");
        exit(1);
    }
}

void tableswitch(Thread *t)
{
    u1 *code = t->current_frame->method_code;
    u4 start = t->pc_register;

    while ((++t->pc_register) % 4 != 0)
        continue;

    int32_t _default = get_tableswitch_32B_values(t->pc_register, code);
    t->pc_register += 4;
    int32_t low = get_tableswitch_32B_values(t->pc_register, code);
    t->pc_register += 4;
    int32_t high = get_tableswitch_32B_values(t->pc_register, code);
    t->pc_register += 4;

    int32_t i = pop_operand(t->current_frame).t._int;

    if (i < low || i > high)
        t->pc_register = start + _default;
    else
        t->pc_register = start + get_tableswitch_32B_values(t->pc_register + 4 * i, code);
}

void ireturn(Thread *t)
{
    java_type ret;
    ret = pop_operand(t->current_frame);
    push_operand(t->current_frame->previous_frame, ret);
    t->pc_register = t->current_frame->method_code_length;
}

void _return(Thread *t)
{
    t->pc_register = t->current_frame->method_code_length;
}