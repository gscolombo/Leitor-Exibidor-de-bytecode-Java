/**
 * @file stack.c
 * @brief Implementações das instruções JVM relacionadas à manipulação da pilha de operandos.
 *
 * Aqui estão implementadas três instruções fundamentais da JVM:
 *
 *  - **dup**  (0x59): Duplica o valor do topo da pilha.
 *  - **dup2** (0x5C): Duplica um valor de categoria 2 ou dois valores de categoria 1.
 *  - **pop / pop2** (0x57 / 0x58): Remove um ou dois valores do topo da pilha.
 *
 * O campo `dtype.cat` é usado para distinguir valores de categoria **CAT1** (32 bits)
 * e **CAT2** (64 bits), conforme a JVM Specification.
 *
 * O interpretador usa `push_operand` e `pop_operand` como primitivas abstratas
 * para manipular a pilha.
 */

#include "bytecode/impl/stack.h"

/**
 * @brief Implementa o opcode `dup` (0x59).
 *
 * Duplica o valor do topo da pilha de operandos:
 *
 *   ..., value → ..., value, value
 *
 * Não altera categorias nem realiza verificações adicionais, pois `dup` só pode
 * ser aplicado a valores de categoria 1 (CAT1) segundo a especificação JVM.
 *
 * @param f Frame atual.
 */
void dup(Frame *f)
{
    push_operand(f, f->operand_stack.stack[f->operand_stack.top]);
    f->pc++;
}

/**
 * @brief Implementa o opcode `dup2` (0x5C).
 *
 * Semântica (conforme JVM Spec):
 *
 *  - Se o topo da pilha contém um valor de categoria 2 (CAT2), então:
 *
 *        ..., v2(Category2) → ..., v2, v2
 *
 *  - Caso contrário (topo e abaixo são ambos CAT1):
 *
 *        ..., v2(Category1), v1(Category1)
 *        → ..., v2, v1, v2, v1
 *
 * Aqui simplificamos assumindo que a pilha sempre contém valores válidos e que
 * o estado do interpretador já cumpre as regras da JVM quanto a categorias.
 *
 * @param f Frame atual.
 */
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
    {
        push_operand(f, value1);
    }

    f->pc++;
}

/**
 * @brief Implementa `pop` (0x57) e `pop2` (0x58).
 *
 * A função detecta o opcode pela leitura de `f->method->bytecode.code[f->pc]`.
 *
 *  - `pop`  remove um valor do topo da pilha.
 *  - `pop2` remove dois valores. Na JVM real, `pop2` remove um valor CAT2 ou dois CAT1,
 *    mas nesta implementação, pela natureza do projeto, ele retira explicitamente
 *    dois valores da pilha.
 *
 * O comportamento exato depende do opcode encontrado em tempo de execução.
 *
 * @param f Frame atual.
 */
void pop_(Frame *f)
{
    (void)pop_operand(f);

    /* 0x58 = pop2 → descartar mais um valor */
    if (f->method->bytecode.code[f->pc] == 0x58)
        (void)pop_operand(f);

    f->pc++;
}
