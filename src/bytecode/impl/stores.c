/**
 * @file stores.c
 * @brief Implementações das instruções JVM relacionadas a armazenamento (store).
 *
 * Este módulo contém os handlers das instruções `istore`, `astore`, variantes
 * compactadas `istore_n`, `dstore_n`, `astore_n`, e a instrução genérica
 * `Tastore`, usada para armazenar valores em arrays de tipos primitivos ou de referência.
 *
 * Em geral, estas instruções têm o seguinte comportamento:
 *  - Remover (pop) um valor da pilha de operandos;
 *  - Determinar o índice de variável local (ou de posição do array);
 *  - Armazenar o valor no local apropriado;
 *  - Avançar corretamente o contador de programa (pc).
 */

#include "bytecode/impl/stores.h"

/**
 * @brief Implementa `istore index`.
 *
 * Lê o índice `index` do próximo byte após o opcode, remove o topo da pilha
 * interpretado como inteiro (`int32_t`) e o armazena em `local_variables[index]`.
 *
 * Formato de instrução:
 *   0x36 <index>
 *
 * @param f Frame atual de execução.
 */
void istore(Frame *f)
{
    int32_t idx = f->method->bytecode.code[f->pc + 1];
    int32_t value = pop_operand(f).value.t._int;

    f->local_variables[idx].value.t._int = value;
    f->pc += 2;
}

/**
 * @brief Implementa `astore index`.
 *
 * Remove uma referência (`reference*`) da pilha, lê o índice imediato
 * e armazena a referência em `local_variables[index]`.
 *
 * @param f Frame atual.
 */
void astore(Frame *f)
{
    reference *objectref = pop_operand(f).value.ref;
    u1 idx = f->method->bytecode.code[f->pc + 1];

    f->local_variables[idx].value.ref = objectref;
    f->pc += 2;
}

/**
 * @brief Implementa `istore_<n>` (opcodes 0x3B–0x3E).
 *
 * O índice é derivado diretamente do opcode.
 * Exemplo: `istore_0` => índice 0, `istore_3` => índice 3.
 *
 * @param f Frame atual.
 */
void istore_n(Frame *f)
{
    int32_t value = pop_operand(f).value.t._int;
    int idx = f->method->bytecode.code[f->pc] - 59; /* 0x3B = 59 */

    f->local_variables[idx].value.t._int = value;
    f->pc++;
}

/**
 * @brief Implementa `dstore_<n>` (opcodes 0x47–0x4A).
 *
 * Similar ao `istore_n`, mas armazena valores double.
 *
 * @param f Frame atual.
 */
void dstore_n(Frame *f)
{
    double value = pop_operand(f).value.t._double;
    int idx = f->method->bytecode.code[f->pc] - 71; /* 0x47 = 71 */

    f->local_variables[idx].value.t._double = value;
    f->pc++;
}

/**
 * @brief Implementa `astore_<n>` (opcodes 0x4B–0x4E).
 *
 * Armazena uma referência em uma variável local cujo índice é derivado do opcode.
 *
 * @param f Frame atual.
 */
void astore_n(Frame *f)
{
    reference *r = pop_operand(f).value.ref;
    int idx = f->method->bytecode.code[f->pc] - 75; /* 0x4B = 75 */

    f->local_variables[idx].value.ref = r;
    f->pc++;
}

/**
 * @brief Implementa instruções de armazenamento em arrays: `iastore`, `lastore`, `fastore`,
 * `dastore`, `aastore`, `bastore`, `castore`, `sastore`.
 *
 * O opcode define o tipo do array. A semântica geral:
 *   1. Pop do topo: valor a armazenar (`op`);
 *   2. Pop: índice do array (`idx`);
 *   3. Pop: referência ao array (`ArrayRef`);
 *   4. Armazena op em arrayref.values[idx], considerando o tipo apropriado.
 *
 * O cálculo do tipo baseia-se em: `type = opcode - 79`, pois:
 *   0 -> iastore   (0x4F)
 *   1 -> lastore   (0x50)
 *   2 -> fastore   (0x51)
 *   3 -> dastore   (0x52)
 *   4 -> aastore   (0x53)
 *   5 -> bastore   (0x54)
 *   6 -> castore   (0x55)
 *   7 -> sastore   (0x56)
 *
 * @param f Frame atual.
 */
void Tastore(Frame *f)
{
    u1 type = f->method->bytecode.code[f->pc] - 79; /* 0x4F = 79 */
    dtype op = pop_operand(f);                      /* valor */
    int32_t idx = pop_operand(f).value.t._int;      /* índice */
    ArrayRef arrayref = pop_operand(f).value.ref->value.array_ref.array; /* array */

    if (arrayref.values)
        switch (type)
        {
        case 0: /* iastore */
            ((int32_t *)arrayref.values)[idx] = op.value.t._int;
            break;
        case 1: /* lastore */
            ((int64_t *)arrayref.values)[idx] = op.value.t._long;
            break;
        case 2: /* fastore */
            ((float *)arrayref.values)[idx] = op.value.t._float;
            break;
        case 3: /* dastore */
            ((double *)arrayref.values)[idx] = op.value.t._double;
            break;
        case 4: /* aastore */
            ((reference *)arrayref.values)[idx] = *op.value.ref;
            break;
        case 5: /* bastore */
            ((int8_t *)arrayref.values)[idx] = op.value.t.byte;
            break;
        case 6: /* castore */
            ((u2 *)arrayref.values)[idx] = op.value.t._char;
            break;
        case 7: /* sastore */
            ((int16_t *)arrayref.values)[idx] = op.value.t._short;
            break;
        default:
            break;
        }

    f->pc++;
}
