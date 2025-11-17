#include "bytecode/impl/references.h"

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
            printf("%lli%c", l, e);
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
            f->method->refs = (void **)realloc(f->method->refs, ((f->method->ref_count + 1) * sizeof(void **)));

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