/**
 * @file bytecode/impl/loads.c
 * @brief Implementações das instruções JVM de carregamento (load).
 *
 * Este módulo contém os handlers para instruções que carregam valores de variáveis
 * locais para a pilha de operandos e para carregar elementos de arrays.
 *
 * Instruções implementadas:
 *  - `_load`        : iload/aload genérico que usa índice imediato (0x15, 0x19 etc).
 *  - `iload_n`      : iload_0 .. iload_3 (opcodes compactos).
 *  - `lload_n`      : lload_0 .. lload_3.
 *  - `dload_n`      : dload_0 .. dload_3.
 *  - `aload_n`      : aload_0 .. aload_3.
 *  - `Taload`       : iaload / laload / faload / daload / aaload / baload / caload / saload.
 *
 * Observações:
 *  - As funções usam as primitivas `push_operand` / `pop_operand` e a estrutura
 *    `Frame` definida no projeto (contendo `local_variables`, `operand_stack`, `pc`, etc).
 *  - Os handlers avançam `f->pc` conforme o comprimento da instrução que processam.
 */

#include "bytecode/impl/loads.h"

/**
 * @brief Handler genérico para instruções de load com índice imediato.
 *
 * Lê um índice (u1) no próximo byte do bytecode, pega o valor em local_variables[idx]
 * e empilha-o na pilha de operandos.
 *
 * Exemplo de uso: iload <index>, aload <index>, etc (quando mapeado para este handler).
 *
 * @param f Ponteiro para o Frame atual.
 */
void _load(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc + 1];
    push_operand(f, f->local_variables[idx]);
    f->pc += 2;
}

/**
 * @brief Implementa iload_<n> (opcodes 0x1A..0x1D -> 26..29).
 *
 * O índice é derivado diretamente do opcode: opcode - 26 produz 0..3.
 * Empilha o valor presente em local_variables[n].
 *
 * @param f Ponteiro para o Frame atual.
 */
void iload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 26;
    push_operand(f, f->local_variables[n]);
    f->pc++;
}

/**
 * @brief Implementa lload_<n> (opcodes compactos para long).
 *
 * Deriva o índice como em iload_n e empilha o valor long presente em local_variables[n].
 *
 * @param f Ponteiro para o Frame atual.
 */
void lload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 30;
    push_operand(f, f->local_variables[n]);

    f->pc++;
}

/**
 * @brief Implementa dload_<n> (opcodes compactos para double).
 *
 * Deriva o índice como em iload_n e empilha o valor double presente em local_variables[n].
 *
 * @param f Ponteiro para o Frame atual.
 */
void dload_n(Frame *f)
{
    u1 n = f->method->bytecode.code[f->pc] - 38;
    push_operand(f, f->local_variables[n]);

    f->pc++;
}

/**
 * @brief Implementa aload_<n> (opcodes compactos para referências).
 *
 * Deriva o índice como em iload_n (offset 42) e empilha a referência em local_variables[idx].
 *
 * @param f Ponteiro para o Frame atual.
 */
void aload_n(Frame *f)
{
    u1 idx = f->method->bytecode.code[f->pc] - 42;
    push_operand(f, f->local_variables[idx]);
    f->pc++;
}

/**
 * @brief Implementa as instruções de acesso a elementos de arrays (Taload).
 *
 * Esta função serve de handler para:
 *   iaload, laload, faload, daload, aaload, baload, caload, saload
 *
 * Semântica:
 *  - Pop: índice (int)
 *  - Pop: referência ao array
 *  - Lê o elemento array[idx] conforme o tipo e empilha um `dtype` inicializado
 *    com o valor lido.
 *
 * O tipo é derivado do opcode atual: `type = opcode - 46` (conforme mapeamento usado no projeto).
 *
 * @param f Ponteiro para o Frame atual.
 */
void Taload(Frame *f)
{
    int32_t idx = pop_operand(f).value.t._int;
    ArrayRef arrayref = pop_operand(f).value.ref->value.array_ref.array;

    dtype v;
    u1 type = f->method->bytecode.code[f->pc] - 46;

    switch (type)
    {
    case 0: /* iaload */
        v = initialize_var(INT, f);
        v.value.t._int = ((int32_t *)arrayref.values)[idx];
        break;
    case 1: /* laload */
        v = initialize_var(LONG, f);
        v.value.t._long = ((int64_t *)arrayref.values)[idx];
        break;
    case 2: /* faload */
        v = initialize_var(FLOAT, f);
        v.value.t._float = ((float *)arrayref.values)[idx];
        break;
    case 3: /* daload */
        v = initialize_var(DOUBLE, f);
        v.value.t._double = ((double *)arrayref.values)[idx];
        break;
    case 4: /* aaload */
        v = initialize_var(REFERENCE, f);
        *v.value.ref = ((reference *)arrayref.values)[idx];
        break;
    case 5: /* baload (byte) */
        v = initialize_var(BYTE, f);
        v.value.t._int = ((int8_t *)arrayref.values)[idx];
        break;
    case 6: /* caload (char) */
        v = initialize_var(CHAR, f);
        v.value.t._int = ((u2 *)arrayref.values)[idx];
        break;
    case 7: /* saload (short) */
        v = initialize_var(SHORT, f);
        v.value.t._int = (int32_t)((int16_t *)arrayref.values)[idx];
        break;
    default:
        /* tipo desconhecido: não empilha nada (comportamento atual) */
        break;
    }

    push_operand(f, v);

    f->pc++;
}
