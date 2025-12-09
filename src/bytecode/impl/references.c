/**
 * @file bytecode/impl/references.c
 * @brief Handlers de instruções relacionadas a referências e campos (get/put/invoke/new/array).
 *
 * Implementa as instruções JVM que manipulam referências a campos e métodos, tais como:
 *  - getstatic / putstatic / getfield / putfield
 *  - invokespecial / invokevirtual / invokestatic / invokeinterface
 *  - new / newarray / anewarray / arraylength
 *
 * O módulo também contém utilitários auxiliares locais para:
 *  - resolver referências a campos/métodos a partir da runtime constant pool;
 *  - montar o vetor de variáveis locais para invocação de métodos (set_local_vars);
 *  - atualizar referências de classe presentes na pilha de frames após o carregamento
 *    dinâmico de classes (update_frame_stack_class_references).
 *
 * Observações:
 *  - Muitas operações assumem que a Constant Pool já foi resolvida para strings em
 *    `Class.runtime_cp[*].value.strref` durante o carregamento da classe.
 *  - Várias funções chamam `bootstrap_loader()` para carregar classes sob demanda e
 *    utilizam a MethodArea para registrar referências a objetos/arrays alocados.
 *  - O código faz uso de funções utilitárias como `initialize_var`, `allocref`,
 *    `appendref`, `push_operand` e `pop_operand`.
 */

#include "bytecode/impl/references.h"

/**
 * @brief Resolve e copia o campo de referência (field_ref) apontado pelo operand do bytecode.
 *
 * Lê o índice imediato (u2) a partir do bytecode (posição pc+1/pc+2), busca a string
 * resolvida na runtime constant pool da classe e copia para o buffer _field_ref.
 *
 * @param f Ponteiro para o Frame atual.
 * @param _field_ref Buffer (pré-alocado) onde será copiada a string da referência (ex.: "pkg/Class.field:Type").
 */
static void get_field_ref(Frame *f, char *_field_ref)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    const char *field_ref = f->class->runtime_cp[idx - 1].value.strref;

    strcpy(_field_ref, field_ref);
}

/**
 * @brief Atualiza o campo `class` dos frames encadeados a partir do frame corrente.
 *
 * Quando uma nova classe é carregada via bootstrap_loader, as estruturas Frame que
 * guardam apenas o nome da classe (class_name) precisam ter o ponteiro `class`
 * atualizado para apontar para a entrada correta na MethodArea.
 *
 * @param f Frame de onde a atualização começa (atualiza também frames anteriores encadeados).
 */
static void update_frame_stack_class_references(Frame *f)
{
    Frame *fp = f;
    fp->class = lookup_class(fp->class_name, fp->method_area);
    while (fp->previous_frame)
    {
        fp->previous_frame->class = lookup_class(fp->previous_frame->class_name, fp->method_area);
        fp = fp->previous_frame;
    }
}

/**
 * @brief Implementa getstatic.
 *
 * Resolve a referência ao campo estático (index na runtime CP) e empilha seu valor.
 * Tratamento especial para java/lang/System.out (simulação de System.out).
 *
 * @param f Frame atual.
 */
void getstatic(Frame *f)
{
    char field_ref[255];
    get_field_ref(f, field_ref);

    // Standard output object field, don't need to initialize the class
    if (!strcmp(field_ref, "java/lang/System.out:Ljava/io/PrintStream;"))
    {
        dtype val = initialize_var(REFERENCE, f);
        val.value.ref->value.array_ref.string = field_ref;
        push_operand(f, val);
    }
    else
    {
        strtok(field_ref, ".");
        const char *field_name = strtok(NULL, ":");
        const char *field_descriptor = strtok(NULL, ":");

        Field *field = lookup_field(field_name, field_descriptor, f->class);

        if (!field)
        {
            printf("Field %s not found.", field_name);
            exit(1);
        }

        push_operand(f, field->value);
    }

    f->pc += 3;
}

/**
 * @brief Implementa putstatic.
 *
 * Popa um valor da pilha de operandos e o armazena no campo estático correspondente.
 *
 * @param f Frame atual.
 */
void putstatic(Frame *f)
{
    char field_ref[255];
    get_field_ref(f, field_ref);

    strtok(field_ref, ".");
    const char *field_name = strtok(NULL, ":");
    const char *field_descriptor = strtok(NULL, ":");

    dtype value = pop_operand(f);
    Field *field = lookup_field(field_name, field_descriptor, f->class);

    if (!field)
    {
        printf("Field %s not found.", field_name);
        exit(1);
    }

    field->value = value;

    f->pc += 3;
}

/**
 * @brief Implementa getfield.
 *
 * Popa uma referência a objeto, resolve o field no objeto e empilha o valor do field.
 *
 * @param f Frame atual.
 */
void getfield(Frame *f)
{
    char field_ref[255];
    get_field_ref(f, field_ref);

    strtok(field_ref, ".");
    const char *field_name = strtok(NULL, ":");
    const char *field_descriptor = strtok(NULL, ":");

    Class *objectref = pop_operand(f).value.ref->value.object_ref;

    Field *field = lookup_field(field_name, field_descriptor, objectref);
    if (!field)
    {
        printf("Field %s not found.", field_name);
        exit(1);
    }
    push_operand(f, field->value);
    f->pc += 3;
}

/**
 * @brief Implementa putfield.
 *
 * Popa o valor e a referência ao objeto e armazena o valor no field do objeto.
 *
 * @param f Frame atual.
 */
void putfield(Frame *f)
{
    char field_ref[255];
    get_field_ref(f, field_ref);

    strtok(field_ref, ".");
    const char *field_name = strtok(NULL, ":");
    const char *field_descriptor = strtok(NULL, ":");

    dtype value = pop_operand(f);
    Class *objectref = pop_operand(f).value.ref->value.object_ref;

    Field *field = lookup_field(field_name, field_descriptor, objectref);
    if (!field)
    {
        printf("Field %s not found.", field_name);
        exit(1);
    }
    field->value = value;

    f->pc += 3;
}

/**
 * @brief Constrói array de variáveis locais pré-populado com argumentos do método.
 *
 * Retira os argumentos da pilha de operandos (na ordem inversa) e os coloca
 * nos primeiros slots do array `local_variables`. Se include_this for true,
 * considera um argumento adicional (this).
 *
 * @param f Frame atual (usado para pop_operand).
 * @param method Ponteiro para Method cujo bytecode/metadata será usado.
 * @param include_this Se true, considera 'this' como argumento extra.
 * @return Ponteiro para vetor de dtype alocado (que deve ser liberado pelo chamador),
 *         ou NULL se max_locals == 0.
 */
static dtype *set_local_vars(Frame *f, Method *method, bool include_this)
{
    u2 max_locals = method->bytecode.max_locals;

    if (max_locals)
    {
        u2 nargs = method->bytecode.nargs + (int)include_this;

        dtype args[nargs];
        for (u2 i = 0; i < nargs; i++)
            args[nargs - (i + 1)] = pop_operand(f);

        dtype *local_variables = (dtype *)calloc(max_locals, sizeof(dtype));

        if (local_variables)
            for (u2 i = 0; i < nargs; i++)
                local_variables[i * (args[i].cat + 1)] = args[i];

        return local_variables;
    }

    return NULL;
}

/**
 * @brief Implementa invokespecial.
 *
 * Trata chamadas especiais (construtores, invocações privadas e super).
 * Para alguns métodos conhecidos (Object.<init>, StringBuffer.<init>) realiza apenas
 * o pop do objeto. Para outros, resolve a classe e o método e invoca via invoke_method.
 *
 * @param f Frame atual.
 */
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
        const char *class_name = strtok(resolved_method, ".");
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

        dtype *local_vars = set_local_vars(f, method, true);

        invoke_method(class, method, local_vars, f, f->method_area);
    }

    f->pc += 3;
}

/**
 * @brief Implementa invokevirtual.
 *
 * Trata chamadas de métodos virtuais. Possui tratamento especial para:
 *  - java/io/PrintStream.print/println (usa _print);
 *  - java/lang/StringBuffer.append / toString (usa strbuf helpers).
 * Para outros casos, resolve a classe e método e invoca via invoke_method.
 *
 * @param f Frame atual.
 */
void invokevirtual(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    const char *method_ref = f->class->runtime_cp[idx - 1].value.strref;

    char ref[strlen(method_ref) + 1];
    strcpy(ref, method_ref);

    char *class_name = strtok(ref, ".");
    const char *method_name = strtok(NULL, ":");
    char *method_descriptor = strtok(NULL, ":");

    if (!strcmp(class_name, "java/io/PrintStream"))
    {
        bool newline;
        if ((newline = !strcmp(method_name, "println")) || !strcmp(method_name, "print")) // Call to println/print
        {
            char e = newline ? '\n' : '\0';
            char rettype = *(method_descriptor + 1);

            _print(f, method_descriptor, rettype, e);

            (void)pop_operand(f); // Pop PrintStream class reference
        }
    }
    else if (!strcmp(class_name, "java/lang/StringBuffer"))
    {
        if (!strcmp(method_name, "append"))
            strbuf_append(f);

        if (!strcmp(method_name, "toString"))
            strbuf_tostring(f);
    }
    else
    {
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

        dtype *local_vars = set_local_vars(f, method, true);

        invoke_method(class, method, local_vars, f, f->method_area);
    }

    f->pc += 3;
}

/**
 * @brief Implementa invokestatic.
 *
 * Resolve a classe (usando bootstrap_loader), valida que o método é estático,
 * prepara variáveis locais (sem 'this') e invoca o método.
 *
 * @param f Frame atual.
 */
void invokestatic(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    const char *method_interface_ref = f->class->runtime_cp[idx - 1].value.strref;

    char ref[strlen(method_interface_ref) + 1];
    strcpy(ref, method_interface_ref);

    // TODO: Check method constraints

    char *class_name = strtok(ref, ".");
    Class *class = bootstrap_loader(NULL, f->method_area, class_name);
    update_frame_stack_class_references(f);

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

        dtype *local_variables = set_local_vars(f, method, false);

        invoke_method(class, method, local_variables, f, f->method_area);

        f->pc += 3;
    }
    else
    {
        printf("Error loading class.\n");
        exit(1);
    }
}

/**
 * @brief Implementa invokeinterface.
 *
 * Resolve método de interface, busca a implementação no objeto (objectref) e invoca.
 * Faz validações de existência e índice. Avança pc em 5 bytes (opcode + 4 operands).
 *
 * @param f Frame atual.
 */
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
    Class *interface = bootstrap_loader(NULL, f->method_area, interface_name);
    update_frame_stack_class_references(f);

    if (!interface)
    {
        printf("Interface %s not found.", interface->name);
        exit(1);
    }

    // Retrieve interface method object
    Method *interface_method = lookup_method(method_name, method_descriptor, interface);
    if (!interface_method)
    {
        printf("Interface method %s of interface %s not found.", method_name, interface->name);
        exit(1);
    }

    // Look for interface method in objectref class
    u2 this_arg = f->operand_stack.top - interface_method->bytecode.nargs;

    Class *objectref = f->operand_stack.stack[this_arg].value.ref->value.object_ref;
    Method *instance_method = lookup_method(method_name, method_descriptor, objectref);

    if (!instance_method)
    {
        printf("Instance method %s declared by interface %s not found in class %s.",
               method_name, interface->name, instance_method->name);
        exit(1);
        // TODO: Look for instance method in superclasses of objectref class.
    }

    dtype *localvars = set_local_vars(f, instance_method, true);
    invoke_method(objectref, instance_method, localvars, f, f->method_area);

    f->pc += 5;
}

/**
 * @brief Implementa newarray.
 *
 * Popa o tamanho, aloca array de tipo primitivo apropriado, registra referência
 * em MethodArea e empilha um objeto de referência que descreve o array.
 *
 * @param f Frame atual.
 */
void newarray(Frame *f)
{
    u1 atype = f->method->bytecode.code[f->pc + 1];
    u4 count = (u4)pop_operand(f).value.t._int;

    allocref(f);

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

    if (!array)
        exit(1);

    appendref(f, array);

    dtype arrayref = initialize_var(REFERENCE, f);
    ArrayRef a = {
        .t = t,
        .class_name = NULL,
        .arraylength = count,
        .dims = 1,
        .values = array};

    arrayref.value.ref->value.array_ref.array = a;
    push_operand(f, arrayref);

    f->pc += 2;
}

/**
 * @brief Implementa anewarray.
 *
 * Cria um array de referências para a classe referenciada na runtime CP (idx),
 * inicializa um bloco de referências e empilha a referência.
 *
 * @param f Frame atual.
 */
void anewarray(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 idx = (code[f->pc + 1] << 8) | code[f->pc + 2];

    RuntimeConstant ref = f->class->runtime_cp[idx - 1];

    u4 count = (u4)pop_operand(f).value.t._int;

    dtype aarray = initialize_var(REFERENCE, f);

    allocref(f);
    reference *array = (reference *)calloc(count, sizeof(reference));

    if (!array)
        exit(1);

    appendref(f, array);

    aarray.value.ref->value.array_ref.array.t = ref.type;
    aarray.value.ref->value.array_ref.array.class_name = ref.value.strref;
    aarray.value.ref->value.array_ref.array.dims = 1;
    aarray.value.ref->value.array_ref.array.arraylength = count;
    aarray.value.ref->value.array_ref.array.values = array;

    push_operand(f, aarray);

    f->pc += 3;
}

/**
 * @brief Implementa arraylength.
 *
 * Retorna (empilha) o tamanho (int) do array referenciado no topo da pilha.
 *
 * @param f Frame atual.
 */
void _arraylength(Frame *f)
{
    dtype length = initialize_var(INT, f);
    length.value.t._int = pop_operand(f).value.ref->value.array_ref.array.arraylength;
    push_operand(f, length);
    f->pc++;
}

/**
 * @brief Inicializa campos estáticos constantes da classe recém-instanciada.
 *
 * Procura por static fields com atributo ConstantValue e inicializa seus valores
 * usando os dados em runtime_cp (que já contém os valores resolvidos).
 *
 * @param f Frame atual (usado para inicializar referências quando necessário).
 * @param cls Ponteiro para a classe cujo fields serão inicializados.
 */
static void initialize_constant_fields(Frame *f, Class *cls)
{
    for (u2 i = 0; i < cls->field_count; i++)
        if (cls->fields[i].access_flags & 0x0008 && cls->fields[i].attrs) // Initialize constant value of static field
        {
            dtype _const;
            RuntimeConstant c = cls->runtime_cp[cls->fields[i].attrs->info.ConstantValue.constantvalue_index - 1];
            switch (c.type)
            {
            case CONSTANT_Long:
                _const = initialize_var(LONG, f);
                _const.value.t._long = c.value.l;
                break;
            case CONSTANT_Float:
                _const = initialize_var(FLOAT, f);
                _const.value.t._float = c.value.f;
                break;
            case CONSTANT_Double:
                _const = initialize_var(DOUBLE, f);
                _const.value.t._double = c.value.d;
                break;
            case CONSTANT_Integer:
                switch (*cls->fields[i].type)
                {
                case 'B':
                    _const = initialize_var(BYTE, f);
                    _const.value.t.byte = c.value.i;
                    break;
                case 'C':
                    _const = initialize_var(CHAR, f);
                    _const.value.t._char = c.value.i;
                    break;
                case 'S':
                    _const = initialize_var(SHORT, f);
                    _const.value.t._short = c.value.i;
                    break;
                case 'Z':
                    _const = initialize_var(BOOLEAN, f);
                    _const.value.t.boolean = c.value.i;
                    break;
                case 'I':
                    _const = initialize_var(INT, f);
                    _const.value.t._int = c.value.i;
                    break;
                default:
                    break;
                }
                break;
            case CONSTANT_String:
                _const = initialize_var(REFERENCE, f);
                _const.value.ref->value.array_ref.string = c.value.strref;
                break;
            default:
                break;
            }

            cls->fields[i].value = _const;
        }
}

/**
 * @brief Implementa new (alocação de objeto).
 *
 * Para classes conhecidas (ex.: java/lang/StringBuffer) executa inicialização específica.
 * Para outras classes:
 *  - carrega a Class via bootstrap_loader;
 *  - aloca e copia a estrutura Class/fields para representar a instância;
 *  - inicializa campos constantes;
 *  - empilha a referência ao objeto criado.
 *
 * @param f Frame atual.
 */
void new(Frame *f)
{
    u1 b1 = f->method->bytecode.code[f->pc + 1];
    u1 b2 = f->method->bytecode.code[f->pc + 2];

    u2 idx = (b1 << 8) | b2;

    const char *c = f->class->runtime_cp[idx - 1].value.strref;

    dtype o = initialize_var(REFERENCE, f);
    o.value.ref->type = REF_OBJECT;

    if (!strcmp(c, "java/lang/StringBuffer"))
        init_stringbuffer(f, &o);
    else // Creates copy of a class
    {
        // Retrieve class
        Class *class = bootstrap_loader(NULL, f->method_area, c);

        // Update current class reference in frame stack
        update_frame_stack_class_references(f);

        // Allocate memory for class instance
        allocref(f);

        Class *objectref;
        objectref = (Class *)malloc(sizeof(Class));

        if (!objectref)
            exit(1);

        memcpy(objectref, class, sizeof(Class));
        appendref(f, objectref);

        // Allocate memory for class instance fields
        allocref(f);
        objectref->fields = (Field *)calloc(objectref->field_count, sizeof(Field));

        if (!objectref->fields)
            exit(1);

        memcpy(objectref->fields, class->fields, sizeof(Field) * objectref->field_count);
        appendref(f, objectref->fields);

        initialize_constant_fields(f, objectref);

        o.value.ref->value.object_ref = objectref;
    }

    push_operand(f, o);
    f->pc += 3;
}
