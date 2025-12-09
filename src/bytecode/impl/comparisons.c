/**
 * @file bytecode/impl/comparisons.c
 * @brief Implementações das instruções de comparação e saltos condicionais da JVM.
 *
 * Este módulo implementa:
 *  - instruções de comparação de inteiros com salto condicional (`ifeq`, `ifne`, `iflt`, `ifge`, `ifgt`, `ifle`);
 *  - instruções de comparação entre inteiros (`if_icmpeq`, `if_icmpne`, `if_icmplt`, `if_icmpge`, `if_icmpgt`, `if_icmple`);
 *  - instruções de comparação de double que produzem -1,0,1 na pilha (`dcmpl`, `dcmpg`).
 *
 * Notas de implementação:
 *  - Os handlers usam offsets baseados no opcode lido em `f->method->bytecode.code[f->pc]`.
 *    Por convenção neste projeto os valores do enum Cond são deriváveis a partir do opcode:
 *      * if_cond:   enum = opcode - 153  (0x99..0x9E -> 153..158)
 *      * if_icmp_:  enum = opcode - 159  (0x9F..0xA4 -> 159..164)
 *  - A função auxiliar `icompare` recebe a condição esperada, os dois operandos (ou v2 = 0
 *    para if_cond) e o offset de branch; retorna o deslocamento a somar ao PC (branch ou 3).
 *  - `dcmpl` e `dcmpg` são tratados por `dcmp_op`. O comportamento para NaN difere entre os
 *    dois opcodes: `dcmpl` retorna -1 quando envolvido NaN, enquanto `dcmpg` retorna 1.
 */

#include "bytecode/impl/comparisons.h"

enum Cond
{
    EQ,
    NE,
    LT,
    GE,
    GT,
    LE
};

/**
 * @brief Avalia uma comparação inteira e decide o deslocamento do PC.
 *
 * Se a condição `cond` entre v1 e v2 for verdadeira, retorna `branch` (offset do salto);
 * caso contrário retorna 3 (tamanho da instrução condicional típica: opcode + 2 bytes de offset).
 *
 * @param cond Condição a avaliar (EQ, NE, LT, GE, GT, LE).
 * @param v1 Primeiro operando da comparação (geralmente o valor da pilha mais "antigo").
 * @param v2 Segundo operando da comparação (ou 0 para instruções `if_cond` unárias).
 * @param branch Offset de salto (16-bit) a ser aplicado caso a condição seja verdadeira.
 * @return int16_t Valor a somar ao PC (branch em caso de condição verdadeira, 3 caso contrário).
 */
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

/**
 * @brief Handler para instruções if_cond (ifeq/ifne/iflt/ifge/ifgt/ifle).
 *
 * Lê a condição a partir do opcode atual, obtém o offset de 16 bits e popa um inteiro da pilha.
 * Em seguida usa `icompare` para decidir se faz o branch (soma branch ao PC) ou apenas avança 3 bytes.
 *
 * Formato da instrução:
 *   opcode (1 byte) | branchbyte1 | branchbyte2
 *
 * @param f Frame atual.
 */
void if_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 153; /* opcode - 153 -> mapeia EQ..LE */
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];
    int32_t value = pop_operand(f).value.t._int;

    f->pc += icompare(cond, value, 0, branch);
}

/**
 * @brief Handler para instruções if_icmp_cond (if_icmpeq/.../if_icmple).
 *
 * Popa dois inteiros (v2 então v1 — ordem de empilhamento), lê o offset e decide o salto.
 *
 * Formato:
 *   opcode (1 byte) | branchbyte1 | branchbyte2
 *
 * @param f Frame atual.
 */
void if_icmp_cond(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    enum Cond cond = code[f->pc] - 159; /* opcode - 159 -> mapeia EQ..LE para if_icmp* */
    int16_t branch = (code[f->pc + 1] << 8) | code[f->pc + 2];

    int32_t v2 = pop_operand(f).value.t._int;
    int32_t v1 = pop_operand(f).value.t._int;

    f->pc += icompare(cond, v1, v2, branch);
}

/**
 * @brief Handler para dcmpl / dcmpg.
 *
 * Popa dois doubles (v2 então v1) e empilha um inteiro com:
 *   1  se v1 > v2
 *   0  se v1 == v2
 *  -1  se v1 < v2
 *
 * Em presença de NaN, dcmpl e dcmpg têm comportamento diferente:
 *  - para `dcmpl` (l == 0): escolher 1 quando comparações indeterminadas? (implementação proj. usa l=opcode-152)
 *  - para `dcmpg`  (l == 1): escolher -1 quando comparações indeterminadas?
 *
 * Nota: o campo `l` é derivado do opcode como `opcode - 152` (0 para dcmpl, 1 para dcmpg),
 *       e o código escolhe o valor retornado quando a comparação não é ordenável.
 *
 * @param f Frame atual.
 */
void dcmp_op(Frame *f)
{
    u1 l = f->method->bytecode.code[f->pc] - 152; /* 0 => dcmpl, 1 => dcmpg */

    double v2 = pop_operand(f).value.t._double;
    double v1 = pop_operand(f).value.t._double;

    dtype result;
    if (v1 > v2)
        result.value.t._int = 1;
    else if (v1 == v2)
        result.value.t._int = 0;
    else if (v1 < v2)
        result.value.t._int = -1;
    else if (l)
        result.value.t._int = -1;
    else
        result.value.t._int = 1;

    push_operand(f, result);
    f->pc++;
}
