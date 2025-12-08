/**
 * @file bytecode/impl/extended.c
 * @brief Implementações de instruções JVM "estendidas" (multianewarray, wide ifnull/ifnonnull).
 *
 * Este módulo implementa instruções que são um pouco mais complexas que as instruções
 * padrão de load/store/aritmética, incluindo:
 *  - multianewarray: criação de arrays multidimensionais (arrays de arrays);
 *  - apoio recursivo para criação de cada dimensão do multiarray (create_multiarray);
 *  - ifnull / ifnonnull: instruções condicionais que verificam referências nulas.
 *
 * Observações:
 *  - A implementação faz uso intensivo de funções utilitárias do runtime (allocref, appendref,
 *    initialize_var, push_operand, pop_operand, etc.) e da estrutura MethodArea para registrar
 *    referências alocadas.
 *  - A função `create_multiarray` é recursiva e constrói a estrutura do array por dimensões,
 *    alocando blocos de referências (reference) para cada nível. Tipos primitivos ainda possuem
 *    tratamento a completar (comentado como TODO).
 *  - Em caso de falhas de alocação a função finaliza o processo com `exit(1)` (comportamento herdado
 *    do projeto).
 */

#include "bytecode/impl/extended.h"

/**
 * @brief Cria recursivamente um array multidimensional (helper para multianewarray).
 *
 * A função constrói a estrutura de multiarray a partir do vetor `counts` contendo o tamanho
 * de cada dimensão. Quando `dims == 0` a função cria o array final (base) com o tipo
 * descrito pela runtime constant pool (classe ou string) ou - TODO - tipos primitivos.
 * Em níveis intermediários, aloca um vetor de referências e chama a si mesma para preencher
 * cada elemento com sub-arrays.
 *
 * Este helper preenche o `multiarray` (um `reference *` que representa a referência ao array)
 * com um `ArrayRef` contendo:
 *  - t: tipo de elemento (CONSTANT_* quando aplicável),
 *  - class_name: nome da classe (para arrays de referência),
 *  - arraylength: tamanho naquela dimensão,
 *  - dims: número de dimensões restantes,
 *  - values: ponteiro para o buffer de valores (referências ou primitivos).
 *
 * @param f Frame atual (usado para consultar bytecode, method_area e utilitários).
 * @param counts Vetor contendo os tamanhos de cada dimensão (counts[0] corresponde à dimensão mais externa).
 * @param dims Índice atual de profundidade/ dimensão (0-based para o nível mais interno quando chamado recursivamente).
 *             Ao chamar inicialmente para n dimensões, passe dims = n-1.
 * @param multiarray Ponteiro para a referência onde será construída a estrutura do array (pré-inicializada por caller).
 *
 * @note:
 *  - A função usa `allocref` e `appendref` para registrar cada bloco alocado na MethodArea.
 *  - Atualmente apenas tipos de referência (CONSTANT_Class / CONSTANT_String) recebem implementação completa;
 *    tipos primitivos no branch `case` estão marcados como TODO.
 *  - Em caso de falha de alocação, o processo termina com `exit(1)`.
 */
static void create_multiarray(Frame *f, int32_t counts[], u1 dims, reference *multiarray)
{
    if (!dims)
    {
        u1 b1 = f->method->bytecode.code[f->pc + 1];
        u1 b2 = f->method->bytecode.code[f->pc + 2];
        u2 index = (b1 << 8) | b2;

        RuntimeConstant c = f->class->runtime_cp[index - 1];

        switch (c.type)
        {
        case CONSTANT_Class:
        case CONSTANT_String:
            allocref(f);
            multiarray->value.array_ref.array.t = (u1)c.type;
            multiarray->value.array_ref.array.class_name = c.value.strref;
            multiarray->value.array_ref.array.arraylength = counts[dims];
            multiarray->value.array_ref.array.dims = dims + 1;
            multiarray->value.array_ref.array.values = (reference *)calloc(counts[dims], sizeof(reference));

            if (!multiarray->value.array_ref.array.values)
                exit(1);

            appendref(f, multiarray->value.array_ref.array.values);
            break;
        // TODO: Handle primitive types
        default:
            break;
        }
    }
    else
    {
        allocref(f);
        reference *ma = (reference *)calloc(counts[dims], sizeof(reference));
        if (ma)
        {
            f->method_area->refs[f->method_area->ref_count - 1] = ma;
            ArrayRef aref = {
                .t = 0,
                .class_name = NULL,
                .arraylength = counts[dims],
                .dims = dims + 1,
                .values = ma};

            multiarray->value.array_ref.array = aref;
            for (u1 d = 0; d < counts[dims]; d++)
                create_multiarray(f, counts, dims - 1, &((reference *)multiarray->value.array_ref.array.values)[d]);
        }
    }
}

/**
 * @brief Handler para a instrução multianewarray.
 *
 * Lê o número de dimensões (u1) no operando (pc+3), empilha os contadores de dimensões
 * retirados da pilha de operandos (cada contador é um int), e chama o helper recursivo
 * para montar a estrutura de array multidimensional. Ao final, empilha a referência ao
 * multiarray criado.
 *
 * Formato da instrução:
 *   multianewarray indexbyte1 indexbyte2 dimensions
 *
 * @param f Frame atual.
 */
void multianewarray(Frame *f)
{
    u1 dims = f->method->bytecode.code[f->pc + 3];

    int32_t counts[dims];
    for (u1 i = 0; i < dims; i++)
        counts[dims - (i + 1)] = pop_operand(f).value.t._int;

    dtype multiarray = initialize_var(REFERENCE, f);
    multiarray.value.ref->type = REF_ARRAY;
    create_multiarray(f, counts, dims - 1, multiarray.value.ref);

    push_operand(f, multiarray);

    f->pc += 4;
}

/**
 * @brief Implementa ifnull / ifnonnull.
 *
 * Essas instruções verificam se a referência no topo da pilha é nula (REF_NULL).
 * - ifnull  (opcode 198 / 0xC6)  -> salta se a referência for nula
 * - ifnonnull (opcode 199 / 0xC7) -> salta se a referência NÃO for nula
 *
 * O operando é um offset de 16 bits (big-endian). Se a condição for verdadeira,
 * o pc é ajustado adicionando o offset; caso contrário, pc avança 3 bytes (opcode + 2).
 *
 * @param f Frame atual.
 */
void if_null(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u2 offset = (code[f->pc + 1] << 8) | code[f->pc + 2];

    bool isnull = pop_operand(f).value.ref->type == REF_NULL;
    bool branch = (code[f->pc] == 198) ? isnull : !isnull; // Choose between ifnull and ifnonnull

    if (branch)
        f->pc += offset;
    else
        f->pc += 3;
}
