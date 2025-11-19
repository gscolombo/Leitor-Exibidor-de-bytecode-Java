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

    char *method_ref = f->class->runtime_cp[idx - 1].value.strref;

    char resolved_method[strlen(method_ref) + 1];
    strcpy(resolved_method, method_ref);

    // Resolved method is Object instance initialization method (do nothing, except for popping the objectref value)
    if (!strcmp(resolved_method, "java/lang/Object.<init>:()V"))
        pop_operand(f);
    else if (!strcmp(resolved_method, "java/lang/StringBuffer.<init>:()V"))
        pop_operand(f); // Already initialized
    else
    { // Lookup and invoke method if is set
        const char *class_name = strtok(method_ref, ".");
        const char *method_name = strtok(NULL, ":");
        char *method_descriptor = strtok(NULL, ":");

        Class *class = lookup_class(class_name, f->method_area);
        if (!class)
        {
            printf("Class %s not found.", class_name);
            exit(1);
        }

        Method *method = lookup_method(method_name, method_descriptor, class);
        if (!method)
        {
            printf("Method %s of class %s not found.", method_name, class_name);
            exit(1);
            // TODO: Search recursively on super classes, if any
        }

        const u2 nargs = method->bytecode.max_locals;
        dtype *local_vars = (dtype *)calloc(nargs, sizeof(dtype));
        for (u2 i = 0; i < nargs; i++)
            local_vars[nargs - (i + 1)] = pop_operand(f);

        invoke_method(class, method, local_vars, f, f->method_area);
    }

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

        _print(f, descriptor, rettype, e);

        (void)pop_operand(f); // Pop PrintStream class reference
    }

    if (!strcmp(method_name, "java/lang/StringBuffer.append"))
        strbuf_append(f);

    if (!strcmp(method_name, "java/lang/StringBuffer.toString"))
        strbuf_tostring(f);

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
        char *method_descriptor = strtok(NULL, ":");

        Method *method = lookup_method(method_name, method_descriptor, class);
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
            local_variables[nargs - (i + 1)] = pop_operand(f);

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

void invokeinterface(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    char *imethod_ref = f->class->runtime_cp[idx - 1].value.strref;

    char resolved_method[strlen(imethod_ref) + 1];
    strcpy(resolved_method, imethod_ref);

    const char *interface_name = strtok(resolved_method, ".");
    const char *method_name = strtok(NULL, ":");
    char *method_descriptor = strtok(NULL, ":");

    // Check if interface exists
    const char *current_class = f->class->name;
    Class *interface = bootstrap_loader(NULL, f->method_area, interface_name);
    f->class = lookup_class(current_class, f->method_area); // Update current class reference in frame

    if (!interface)
    {
        printf("Interface %s not found.", interface);
        exit(1);
    }

    // Retrieve interface method object
    Method *interface_method = lookup_method(method_name, method_descriptor, interface);
    if (!interface_method)
    {
        printf("Interface method %s of interface %s not found.", method_name, interface);
        exit(1);
    }

    // Look for interface method in objectref class
    u2 nargs = interface_method->bytecode.nargs + 1;

    dtype *localvars = (dtype *)calloc(nargs, sizeof(dtype));
    for (u2 i = 0; i < nargs; i++)
        localvars[nargs - (i + 1)] = pop_operand(f);

    Class *objectref = localvars[0].value.ref.object_ref;
    Method *instance_method = lookup_method(method_name, method_descriptor, objectref);

    if (!instance_method)
    {
        printf("Instance method %s declared by interface %s not found in class %s.", method_name, interface, instance_method->name);
        exit(1);
        // TODO: Look for instance method in superclasses of objectref class.
    }

    invoke_method(objectref, instance_method, localvars, f, f->method_area);

    f->pc += 5;
}

void newarray(Frame *f)
{
    u1 atype = f->method->bytecode.code[f->pc + 1];
    u4 count = (u4)pop_operand(f).value.t._int;

    int t;
    void *array;
    switch (atype)
    {
    case 4: // boolean
        t = CONSTANT_Integer;
        array = calloc(count, sizeof(bool));
        break;
    case 5: // char
        t = CONSTANT_Integer;
        array = calloc(count, sizeof(u2));
        break;
    case 6: // float
        t = CONSTANT_Float;
        array = calloc(count, sizeof(float));
        break;
    case 7: // double
        t = CONSTANT_Double;
        array = calloc(count, sizeof(double));
        break;
    case 8: // byte
        t = CONSTANT_Integer;
        array = calloc(count, sizeof(int8_t));
        break;
    case 9: // short
        t = CONSTANT_Integer;
        array = calloc(count, sizeof(int16_t));
        break;
    case 10: // int
        t = CONSTANT_Integer;
        array = calloc(count, sizeof(int32_t));
        break;
    case 11: // long
        t = CONSTANT_Long;
        array = calloc(count, sizeof(int64_t));
        break;
    default:
        break;
    }

    if (array)
    {
        allocref(f);

        if (f->method->refs)
        {
            f->method->ref_count++;
            f->method->refs[f->method->ref_count - 1] = array;

            dtype arrayref = initialize_var(REFERENCE);
            ArrayRef a = {
                .t = t,
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

void new(Frame *f)
{
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];

    u2 idx = (b1 << 8) | b2;

    char *c = f->class->runtime_cp[idx - 1].value.strref;

    dtype o = initialize_var(REFERENCE);
    if (!strcmp(c, "java/lang/StringBuffer"))
        init_stringbuffer(f, &o);
    else // Creates copy of a class
    {
        const char *current_class = f->class->name;
        Class *class = bootstrap_loader(NULL, f->method_area, c);
        f->class = lookup_class(current_class, f->method_area); // Update current class reference in frame

        allocref(f);
        if (f->method->refs)
        {
            f->method->ref_count++;
            o.value.ref.object_ref = (Class *)malloc(sizeof(Class));
            if (o.value.ref.object_ref)
            {
                memcpy(o.value.ref.object_ref, class, sizeof(Class));
                f->method->refs[f->method->ref_count - 1] = o.value.ref.object_ref;
            }
        }
    }

    push_operand(f, o);
    f->pc += 3;
}