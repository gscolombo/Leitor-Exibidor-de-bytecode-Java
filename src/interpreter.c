/**
 * @file interpreter.c
 * @brief Implementação do loop de execução e utilitários do intérprete JVM.
 *
 * Este módulo contém a lógica principal para execução de bytecode de métodos,
 * manipulação da pilha de operandos e criação/invocação de frames (chamadas de métodos).
 * As funções aqui são responsáveis por:
 *  - executar o loop de instruções lendo opcodes e invocando handlers;
 *  - empurrar/popear valores da pilha de operandos (dtype);
 *  - inicializar variáveis locais conforme o tipo (categoria 1/2);
 *  - criar um novo frame para invocação de método, executar o método e liberar recursos.
 *
 * As operações assumem as estruturas definidas em `interpreter.h` (Frame, Method, Class,
 * OperandStack, dtype, etc.).
 */

#include "interpreter.h"

/**
 * @brief Loop principal de execução de um método.
 *
 * Lê o array de bytecode do método associado ao frame `f` e, enquanto o
 * program counter (pc) for menor que o tamanho do código, busca o opcode
 * corrente e chama a função de execução correspondente na tabela `opcode_table`.
 *
 * @param f Ponteiro para o frame cujo método será executado.
 */
void execute_method(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 l = f->method->bytecode.code_length;

    while (f->pc < l)
        opcode_table[code[f->pc]].exec(f);
}

/**
 * @brief Empilha um operando no topo da pilha de operandos do frame.
 *
 * Esta função incrementa o índice `top` e armazena o valor `d` na pilha.
 * A pilha é um array de `dtype` armazenado em `f->operand_stack.stack`.
 *
 * @note A função é definida como `inline` para reduzir overhead em chamadas frequentes.
 *
 * @param f Ponteiro para o frame atual.
 * @param d Valor do tipo `dtype` a ser empilhado.
 */
inline void push_operand(Frame *f, dtype d)
{
    f->operand_stack.top++;
    f->operand_stack.stack[f->operand_stack.top] = d;
}

/**
 * @brief Remove e retorna o operando do topo da pilha de operandos.
 *
 * Decrementa o índice `top` da pilha e retorna o valor previamente no topo.
 *
 * @param f Ponteiro para o frame atual.
 * @return O `dtype` que estava no topo da pilha.
 */
dtype pop_operand(Frame *f)
{
    dtype d = f->operand_stack.stack[f->operand_stack.top];
    f->operand_stack.top--;
    return d;
}

/**
 * @brief Inicializa uma variável local conforme o tipo especificado.
 *
 * Define a categoria (CAT1 ou CAT2) do `dtype` retornado dependendo do tipo
 * (`LONG`/`DOUBLE` são CAT2). Se o tipo for `REFERENCE`, aloca uma estrutura
 * `reference`, registra-a na tabela de referências do frame via `allocref`
 * e `appendref` e atribui o ponteiro a `var.value.ref`.
 *
 * @note Se a alocação de referência falhar, a função chama `exit(1)`.
 *
 * @param t Tipo lógico da variável (valor do enum `type_enum`).
 * @param f Ponteiro para o frame atual (usado para registrar referências).
 * @return Um `dtype` inicializado com a categoria adequada e, no caso de referência,
 *         com `value.ref` apontando para a nova referência alocada.
 */
dtype initialize_var(type_enum t, Frame *f)
{
    dtype var;
    switch (t)
    {
    case LONG:
    case DOUBLE:
        var.cat = CAT2;
        break;
    default:
        var.cat = CAT1;
        break;
    }

    if (t == REFERENCE)
    {
        allocref(f);

        var.value.ref = (reference *)malloc(sizeof(reference));
        if (!var.value.ref)
            exit(1);

        appendref(f, var.value.ref);
    }

    return var;
}

/**
 * @brief Invoca um método criando um novo Frame, executando-o e liberando recursos.
 *
 * A função:
 *  1. Aloca um novo `Frame` e inicializa campos como `pc`, `method`, `class`, `previous_frame` e `method_area`.
 *  2. Associa o array de variáveis locais (`local_variables`) passado ao novo frame.
 *  3. Inicializa a pilha de operandos (`OperandStack`) com capacidade igual a `max_stack`.
 *  4. Se a alocação de pilha e variáveis locais for bem sucedida, chama `execute_method`.
 *  5. Após execução, libera `frame->local_variables`, `frame->operand_stack.stack` e o próprio `frame`.
 *
 * @warning A implementação espera que `local_variables` tenha sido alocado pelo chamador
 *          e toma a responsabilidade de liberá-lo ao final da execução deste método.
 *
 * @param class Ponteiro para a estrutura `Class` que contém o método.
 * @param method Ponteiro para o `Method` a ser invocado.
 * @param local_variables Array de variáveis locais (alocado pelo chamador) que será usado pelo novo frame.
 * @param caller Ponteiro para o frame chamador (ou NULL se invocação inicial).
 * @param method_area Ponteiro para a `MethodArea` global (contexto de classes carregadas).
 */
void invoke_method(Class *class, Method *method, dtype *local_variables, Frame *caller, MethodArea *method_area)
{
    // Create frame...
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    frame->pc = 0;
    frame->local_variables = local_variables;
    frame->previous_frame = caller;
    frame->method = method; // current method
    frame->class = class;   // current class
    frame->class_name = class->name;
    frame->method_area = method_area; // pointer to global method area

    OperandStack op_stack;
    op_stack.top = -1;
    op_stack.capacity = frame->method->bytecode.max_stack;
    op_stack.stack = (dtype *)calloc(frame->method->bytecode.max_stack, sizeof(dtype));

    frame->operand_stack = op_stack;

    // ...then execute current method...
    if (frame->operand_stack.stack != NULL && frame->local_variables != NULL)
        execute_method(frame);

    // ...and free allocated memory
    free(frame->local_variables);
    free(frame->operand_stack.stack);
    free(frame);
}
