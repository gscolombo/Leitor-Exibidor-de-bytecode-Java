#include "opcode_functions.h"

void aload_0(Frame *f)
{
    push_operand(f, f->local_variables[0]);
    f->pc_register++;
}

void iload_n(Frame *f)
{
    u1 n = f->method_code[f->pc_register] - 26;
    push_operand(f, f->local_variables[n]);
    f->pc_register++;
}

void iconst_i(Frame *f)
{
    java_type i;
    i.t._int = (int32_t)f->method_code[f->pc_register] - 3;
    push_operand(f, i);
    f->pc_register++;
}

void ldc(Frame *f)
{
    u1 index = f->method_code[f->pc_register + 1];
    cp_info constant = f->runtime_cp[index - 1];

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
        val.ref.array_ref = (primitive_type *)get_constant_UTF8_value(index, f->runtime_cp);
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(f, val);
    f->pc_register += 2;
}

void getstatic(Frame *f)
{
    u1 *code = f->method_code;
    u2 idx = (code[f->pc_register + 1] << 8) | code[f->pc_register + 2];

    java_type resolved_field;
    resolved_field.ref.array_ref = (primitive_type *)get_constant_UTF8_value(idx, f->runtime_cp);

    // Standard output object field, don't need to initialize the class
    if (!strcmp((const char *)resolved_field.ref.array_ref, "java/lang/System.out:Ljava/io/PrintStream;"))
    {
        push_operand(f, resolved_field);
    }

    f->pc_register += 3;
}

void invokespecial(Frame *f)
{
    u1 *code = f->method_code;
    u2 idx = (code[f->pc_register + 1] << 8) | code[f->pc_register + 2];

    char *resolved_method = get_constant_UTF8_value(idx, f->runtime_cp);

    // Super class is Object class (do nothing, except for popping the objectref value)
    if (!strcmp(resolved_method, "java/lang/Object.<init>:()V"))
        pop_operand(f);

    // TODO: Finish (super) class initialization

    free(resolved_method);
    f->pc_register += 3;
}

void invokevirtual(Frame *f)
{
    u1 *code = f->method_code;
    u2 idx = (code[f->pc_register + 1] << 8) | code[f->pc_register + 2];

    char *resolved_method = get_constant_UTF8_value(idx, f->runtime_cp);

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
                char *str = (char *)pop_operand(f).ref.array_ref;
                printf("%s\n", str);
                free(str);
            }
            // TODO: Define logic for class references
            break;
        case 'I':
            int32_t i = pop_operand(f).t._int;
            printf("%i\n", i);
            break;
        // TODO: Define logic for other cases
        default:
            break;
        }

        free(pop_operand(f).ref.array_ref); // Pop and free PrintStream class reference
    }

    // TODO: Handle general class method invocation

    free(resolved_method);
    f->pc_register += 3;
}

void invokestatic(Frame *f)
{
    u1 *code = f->method_code;
    u2 idx = (code[f->pc_register + 1] << 8) | code[f->pc_register + 2];

    u2 method_class_idx = f->runtime_cp[idx - 1].info.Ref.class_index;
    char *class_name = get_constant_UTF8_value(method_class_idx, f->runtime_cp);
    ClassFile *class = bootstrap_loader(NULL, f->method_area, class_name);

    if (class)
    {
        char *resolved_method = get_constant_UTF8_value(idx, f->runtime_cp);
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
            local_variables[i] = pop_operand(f);

        u4 last_pc = f->pc_register;

        invoke_method(class, method, local_variables, f, f->method_area);
        f->pc_register = last_pc + 3;
    }
    else
    {
        printf("Error loading class.\n");
        exit(1);
    }
}

void tableswitch(Frame *f)
{
    u1 *code = f->method_code;
    u4 start = f->pc_register;

    while ((++f->pc_register) % 4 != 0)
        continue;

    int32_t _default = get_tableswitch_32B_values(f->pc_register, code);
    f->pc_register += 4;
    int32_t low = get_tableswitch_32B_values(f->pc_register, code);
    f->pc_register += 4;
    int32_t high = get_tableswitch_32B_values(f->pc_register, code);
    f->pc_register += 4;

    int32_t i = pop_operand(f).t._int;

    if (i < low || i > high)
        f->pc_register = start + _default;
    else
        f->pc_register = start + get_tableswitch_32B_values(f->pc_register + 4 * i, code);
}

void ireturn(Frame *f)
{
    java_type ret;
    ret = pop_operand(f);
    push_operand(f->previous_frame, ret);
    f->pc_register = f->method_code_length;
}

void _return(Frame *f)
{
    f->pc_register = f->method_code_length;
}