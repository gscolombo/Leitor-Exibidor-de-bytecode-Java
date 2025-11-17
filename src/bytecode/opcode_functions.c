#include "opcode_functions.h"

void iconst_i(Frame *f)
{
    dtype i = initialize_var(INT);
    i.value.t._int = (int32_t)f->method->bytecode.code[f->pc] - 3;
    push_operand(f, i);
    f->pc++;
}

void fconst_f(Frame *f)
{
    dtype _f = initialize_var(FLOAT);
    _f.value.t._float = (float)(f->method->bytecode.code[f->pc] - 11);
    push_operand(f, _f);
    f->pc++;
}

void bipush(Frame *f)
{
    dtype bi = initialize_var(BYTE);
    bi.value.t._int = (int8_t)f->method->bytecode.code[f->pc + 1];
    push_operand(f, bi);
    f->pc += 2;
}

void sipush(Frame *f)
{
    dtype si = initialize_var(SHORT);
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];
    si.value.t._int = (int32_t)((b1 << 8) | b2);

    push_operand(f, si);
    f->pc += 3;
}

void ldc_(Frame *f)
{
    u1 mode = f->method->bytecode.code[f->pc] - 18;

    dtype val;

    u2 index;
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];

    index = mode > 0 ? (b1 << 8) | b2 : b1;

    RuntimeConstant c = f->class->runtime_cp[index - 1];

    switch (c.type)
    {
    case CONSTANT_Integer:
        val = initialize_var(INT);
        val.value.t._int = c.value.i;
        break;
    case CONSTANT_Float:
        val = initialize_var(FLOAT);
        val.value.t._float = c.value.f;
        break;
    case CONSTANT_Long:
        val = initialize_var(LONG);
        val.value.t._long = c.value.l;
        break;
    case CONSTANT_Double:
        val = initialize_var(DOUBLE);
        val.value.t._double = c.value.d;
        break;
    case CONSTANT_String:
        val = initialize_var(REFERENCE);
        val.value.ref.array_ref.string = c.value.strref;
        break;
    // TODO: Handle class references
    default:
        break;
    }

    push_operand(f, val);
    f->pc += mode > 0 ? 3 : 2;
}

void _load(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    push_operand(f, f->local_variables[idx]);
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
    int32_t value = pop_operand(f).value.t._int;

    f->pc += icompare(cond, value, 0, branch);
}

void if_icmp_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 159;
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];

    int32_t v2 = pop_operand(f).value.t._int;
    int32_t v1 = pop_operand(f).value.t._int;

    f->pc += icompare(cond, v1, v2, branch);
}

void iadd(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 + v2;
    push_operand(f, result);

    f->pc++;
}

void isub(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 - v2;
    push_operand(f, result);

    f->pc++;
}

void imul(Frame *f)
{
    int32_t v1, v2;
    v2 = pop_operand(f).value.t._int;
    v1 = pop_operand(f).value.t._int;

    dtype result = initialize_var(INT);
    result.value.t._int = v1 * v2;
    push_operand(f, result);

    f->pc++;
}

void iinc(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    int32_t _const = (int32_t)f->method->bytecode.code[f->pc + 2];

    f->local_variables[idx].value.t._int += _const;
    f->pc += 3;
}

void i2T(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    dtype result;

    switch (f->method->bytecode.code[f->pc])
    {
    case 0x85:
        result = initialize_var(LONG);
        result.value.t._long = (long)value;
        break;
    case 0x86:
        result = initialize_var(FLOAT);
        result.value.t._float = (float)value;
        break;
    case 0x87:
        result = initialize_var(DOUBLE);
        result.value.t._double = (double)value;
        break;
    case 0x91:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(int8_t)value;
        break;
    case 0x92:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(u2)value;
        break;
    case 0x93:
        result = initialize_var(INT);
        result.value.t._int = (int32_t)(int16_t)value;
        break;
    default:
        break;
    }

    push_operand(f, result);
    f->pc++;
}

void iload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 26;
    push_operand(f, f->local_variables[n]);
    f->pc++;
}

void aload_n(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc] - 42;
    push_operand(f, f->local_variables[idx]);
    f->pc++;
}

void Taload(Frame *f)
{
    int32_t idx = pop_operand(f).value.t._int;
    ArrayRef arrayref = pop_operand(f).value.ref.array_ref.array;

    dtype v;
    u1 type = f->method->bytecode.code[f->pc] - 46;

    switch (type)
    {
    case 0:
        v = initialize_var(INT);
        v.value.t._int = ((int32_t *)arrayref.values)[idx];
        break;
    case 1:
        v = initialize_var(LONG);
        v.value.t._long = ((int64_t *)arrayref.values)[idx];
        break;
    case 2:
        v = initialize_var(FLOAT);
        v.value.t._float = ((float *)arrayref.values)[idx];
        break;
    case 3:
        v = initialize_var(DOUBLE);
        v.value.t._double = ((double *)arrayref.values)[idx];
        break;
    case 4:
        v = initialize_var(REFERENCE);
        v.value.ref = ((reference *)arrayref.values)[idx];
        break;
    case 5:
        v = initialize_var(BYTE);
        v.value.t._int = ((int8_t *)arrayref.values)[idx];
        break;
    case 6:
        v = initialize_var(CHAR);
        v.value.t._int = ((u2 *)arrayref.values)[idx];
        break;
    case 7:
        v = initialize_var(SHORT);
        v.value.t._int = (int32_t)((int16_t *)arrayref.values)[idx];
        break;
    default:
        break;
    }

    push_operand(f, v);

    f->pc++;
}

void istore(Frame *f)
{
    int32_t idx = f->method->bytecode.code[f->pc + 1];
    int32_t value = pop_operand(f).value.t._int;

    f->local_variables[idx].value.t._int = value;
    f->pc += 2;
}

void astore(Frame *f)
{
    reference objectref = pop_operand(f).value.ref;
    u1 idx = f->method->bytecode.code[f->pc + 1];

    f->local_variables[idx].value.ref = objectref;
    f->pc += 2;
}

void istore_n(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    int idx = f->method->bytecode.code[f->pc] - 59;

    f->local_variables[idx].value.t._int = value;
    f->pc++;
}

void astore_n(Frame *f)
{
    reference r = pop_operand(f).value.ref;
    int idx = f->method->bytecode.code[f->pc] - 75;

    f->local_variables[idx].value.ref = r;
    f->pc++;
}

void Tastore(Frame *f)
{
    u1 type = f->method->bytecode.code[f->pc] - 79;
    java_type value = pop_operand(f).value;
    int32_t idx = pop_operand(f).value.t._int;
    ArrayRef arrayref = pop_operand(f).value.ref.array_ref.array;

    if (arrayref.values)
        switch (type)
        {
        case 0:
            ((int32_t *)arrayref.values)[idx] = value.t._int;
            break;
        case 1:
            ((int64_t *)arrayref.values)[idx] = value.t._long;
            break;
        case 2:
            ((float *)arrayref.values)[idx] = value.t._float;
            break;
        case 3:
            ((double *)arrayref.values)[idx] = value.t._double;
            break;
        case 4:
            ((reference *)arrayref.values)[idx] = value.ref;
            break;
        case 5:
            ((int8_t *)arrayref.values)[idx] = value.t.byte;
            break;
        case 6:
            ((u2 *)arrayref.values)[idx] = value.t._char;
            break;
        case 7:
            ((int16_t *)arrayref.values)[idx] = value.t._short;
            break;
        default:
            break;
        }

    f->pc++;
}

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

void getstatic(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    const char *_field_ref = f->class->runtime_cp[idx - 1].value.strref;

    char *field_ref = (char *)malloc((strlen(_field_ref) + 1) * sizeof(char));
    strcpy(field_ref, _field_ref);

    // Standard output object field, don't need to initialize the class
    if (!strcmp(field_ref, "java/lang/System.out:Ljava/io/PrintStream;"))
    {
        dtype val = initialize_var(REFERENCE);
        val.value.ref.array_ref.string = field_ref;
        push_operand(f, val);
    }

    // TODO: Handle general field references

    f->pc += 3;
    free(field_ref);
}

void invokespecial(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *method_name = f->class->runtime_cp[idx - 1].value.strref;

    char *resolved_method = (char *)malloc((strlen(method_name) + 1) * sizeof(char));
    strcpy(resolved_method, method_name);

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

    const char *method_ref = f->class->runtime_cp[idx - 1].value.strref;
    size_t length = strlen(method_ref);

    char *method_name = (char *)malloc(sizeof(char) * length + 1);
    strcpy(method_name, method_ref);

    method_name = strtok(method_name, ":");
    bool newline;
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
                char *str = pop_operand(f).value.ref.array_ref.string;
                printf("%s%c", str, e);
            }
            // TODO: Define logic for class references
            break;
        case 'I':
            int32_t i = pop_operand(f).value.t._int;
            printf("%i%c", i, e);
            break;
        case 'J':
            int64_t l = pop_operand(f).value.t._long;
            printf("%li%c", l, e);
            break;
        case 'F':
            float _f = pop_operand(f).value.t._float;
            printf("%.1f%c", _f, e);
            break;
        case 'D':
            double _d = pop_operand(f).value.t._double;
            printf("%.1f%c", _d, e);
            break;
        case 'C':
            u2 c = pop_operand(f).value.t._char;
            printf("%c%c", c, e);
            break;
        // TODO: Define logic for other cases
        default:
            printf("%c", e);
            break;
        }

        (void)pop_operand(f); // Pop PrintStream class reference
    }

    // TODO: Handle general class method invocation

    free(method_name);
    f->pc += 3;
}

void invokestatic(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    const char *method_interface_ref = f->class->runtime_cp[idx - 1].value.strref;

    char *ref = (char *)malloc((strlen(method_interface_ref) + 1) * sizeof(char));
    strcpy(ref, method_interface_ref);

    // TODO: Check method constraints

    char *class_name = strtok(ref, ".");
    Class *class = bootstrap_loader(NULL, f->method_area, class_name);

    if (class)
    {
        char *method_name = strtok(ref + strlen(class_name) + 1, ":"); // Get only method name

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
        dtype *local_variables = (dtype *)calloc(nargs, sizeof(dtype));

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

    free(ref);
}

void newarray(Frame *f)
{
    u1 atype = f->method->bytecode.code[f->pc + 1];
    u4 count = (u4)pop_operand(f).value.t._int;

    void *array;
    switch (atype)
    {
    case 4:
        array = calloc(count, sizeof(bool));
        break;
    case 5:
        array = calloc(count, sizeof(u2));
        break;
    case 6:
        array = calloc(count, sizeof(float));
        break;
    case 7:
        array = calloc(count, sizeof(double));
        break;
    case 8:
        array = calloc(count, sizeof(int8_t));
        break;
    case 9:
        array = calloc(count, sizeof(int16_t));
        break;
    case 10:
        array = calloc(count, sizeof(int32_t));
        break;
    case 11:
        array = calloc(count, sizeof(int64_t));
        break;
    default:
        break;
    }

    if (array)
    {
        if (!f->method->ref_count)
            f->method->refs = malloc(sizeof(void **));
        else
            f->method->refs = (void **)realloc(f->method->refs, (sizeof(f->method->refs) + 1) * sizeof(void **));

        if (f->method->refs)
        {
            f->method->ref_count++;
            f->method->refs[f->method->ref_count - 1] = array;

            dtype arrayref = initialize_var(REFERENCE);
            ArrayRef a = {
                .arraylength = count,
                .dims = 1,
                .values = array};
            arrayref.value.ref.array_ref.array = a;
            push_operand(f, arrayref);
        }
    }

    f->pc += 2;
}

void _arraylength(Frame *f)
{
    dtype length = initialize_var(INT);
    length.value.t._int = pop_operand(f).value.ref.array_ref.array.arraylength;
    push_operand(f, length);
    f->pc++;
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