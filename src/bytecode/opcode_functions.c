#include "opcode_functions.h"

void iconst_i(Frame *f)
{
    java_type i;
    i.t._int = (int32_t)f->method->bytecode.code[f->pc] - 3;
    push_operand(f, i);
    f->pc++;
}

void bipush(Frame *f)
{
    java_type bi;
    bi.t._int = (int32_t)f->method->bytecode.code[f->pc + 1];
    push_operand(f, bi);
    f->pc += 2;
}

void ldc(Frame *f)
{
    u1 index = f->method->bytecode.code[f->pc + 1];
    RuntimeConstant c = f->class->runtime_cp[index - 1];

    java_type val;

    switch (c.type)
    {
    case CONSTANT_Integer:
        val.t._int = c.value.i;
        break;
    case CONSTANT_Float:
        val.t._float = c.value.f;
        break;
    case CONSTANT_String:
        val.ref.array_ref = (primitive_type *)c.value.strref;
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(f, val);
    f->pc += 2;
}

void _goto(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    f->pc += (int16_t)((code[f->pc + 1] << 8) | code[f->pc + 2]);
}

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
    int32_t value = pop_operand(f).t._int;

    f->pc += icompare(cond, value, 0, branch);
}

void if_icmp_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 159;
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];

    int32_t v2 = pop_operand(f).t._int;
    int32_t v1 = pop_operand(f).t._int;

    f->pc += icompare(cond, v1, v2, branch);
}

void iadd(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).t._int;
    v1 = pop_operand(f).t._int;

    java_type result;
    result.t._int = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

void isub(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).t._int;
    v1 = pop_operand(f).t._int;

    java_type result;
    result.t._int = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

void imul(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).t._int;
    v1 = pop_operand(f).t._int;

    java_type result;
    result.t._int = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

void iinc(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    int32_t _const = (int32_t)f->method->bytecode.code[f->pc + 2];

    f->local_variables[idx].t._int += _const;
    f->pc += 3;
}

void iload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 26;
    push_operand(f, f->local_variables[n]);
    f->pc++;
}

void aload_0(Frame *f)
{
    push_operand(f, f->local_variables[0]);
    f->pc++;
}

void istore_n(Frame *f)
{
    int32_t value = pop_operand(f).t._int;
    int idx = f->method->bytecode.code[f->pc] - 59;

    f->local_variables[idx].t._int = value;
    f->pc++;
}

void getstatic(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *field_ref = f->class->runtime_cp[idx - 1].value.strref;

    // Standard output object field, don't need to initialize the class
    if (!strcmp(field_ref, "java/lang/System.out:Ljava/io/PrintStream;"))
    {
        java_type val;
        val.ref.array_ref = (primitive_type *)field_ref;
        push_operand(f, val);
    }

    // TODO: Handle general field references

    f->pc += 3;
}

void invokespecial(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *resolved_method = f->class->runtime_cp[idx - 1].value.strref;

    // Super class is Object class (do nothing, except for popping the objectref value)
    if (!strcmp(resolved_method, "java/lang/Object.<init>:()V"))
        pop_operand(f);

    // TODO: Finish (super) class initialization

    free(resolved_method);
    f->pc += 3;
}

void invokevirtual(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *method_ref = f->class->runtime_cp[idx - 1].value.strref;
    size_t length = strlen(method_ref);

    char *method_name = (char *)malloc(sizeof(char) * length + 1);
    strcpy(method_name, method_ref);

    method_name = strtok(method_name, ":");
    int newline;
    if ((newline = !strcmp(method_name, "java/io/PrintStream.println")) ||
        !strcmp(method_name, "java/io/PrintStream.print")) // Call to println/print
    {
        char e = newline ? '\n' : '\0';
        char *descriptor = strtok(NULL, ":");
        char rettype = *(descriptor + 1);
        switch (rettype)
        {
        case 'L':
            if (!strcmp(descriptor, "(Ljava/lang/String;)V"))
            {
                char *str = (char *)pop_operand(f).ref.array_ref;
                printf("%s%c", str, e);
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

        (void)pop_operand(f).ref.array_ref; // Pop PrintStream class reference
    }

    // TODO: Handle general class method invocation

    free(method_name);
    f->pc += 3;
}

void invokestatic(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *method_interface_ref = f->class->runtime_cp[idx - 1].value.strref;

    // TODO: Check method constraints

    char *class_name = strtok(method_interface_ref, ".");
    Class *class = bootstrap_loader(NULL, f->method_area, class_name);

    if (class)
    {
        char *method_name = strtok(method_interface_ref + strlen(class_name) + 1, ":"); // Get only method name

        Method *method = lookup_method(method_name, class);

        if (!method)
        {
            printf("Error: method not found.\n");
            exit(1);
        }

        if (!(method->access_flags & 0x0008)) // Check if method is static
        {
            printf("Error: method is not static.\n");
            exit(1);
        }

        u2 nargs = method->bytecode.max_locals;
        java_type *local_variables = (java_type *)calloc(nargs, sizeof(java_type));

        for (u2 i = 0; i < nargs; i++)
            local_variables[i] = pop_operand(f);

        u4 last_pc = f->pc;

        invoke_method(class, method, local_variables, f, f->method_area);
        f->pc = last_pc + 3;
    }
    else
    {
        printf("Error loading class.\n");
        exit(1);
    }
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

    int32_t i = pop_operand(f).t._int;

    if (i < low || i > high)
        f->pc = start + _default;
    else
        f->pc = start + get_tableswitch_32B_values(f->pc + 4 * i, code);
}

void ireturn(Frame *f)
{
    java_type ret;
    ret = pop_operand(f);
    push_operand(f->previous_frame, ret);
    f->pc = f->method->bytecode.code_length;
}

void _return(Frame *f)
{
    f->pc = f->method->bytecode.code_length;
}