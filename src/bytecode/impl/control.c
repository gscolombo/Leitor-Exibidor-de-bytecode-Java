/**
 * @file bytecode/impl/control.c
 * @brief Implementações das instruções de controle de fluxo da JVM.
 *
 * Contém handlers para instruções de controle (jumps e returns):
 *  - _goto       : jump incondicional com offset de 16 bits
 *  - tableswitch : switch baseado em faixa (dense switch)
 *  - lookupswitch: switch baseado em pares (sparse switch)
 *  - Treturn     : retorno com valor (ireturn, lreturn, freturn, dreturn, areturn)
 *  - _return     : retorno void
 *
 * Observações:
 *  - As instruções `tableswitch` e `lookupswitch` exigem alinhamento a 4 bytes após o opcode;
 *    esta implementação avança `f->pc` até o alinhamento e depois lê os campos de 32 bits.
 *  - Os offsets (32-bit nos switches, 16-bit em goto) são relativos ao início do opcode
 *    (posição original do `pc` onde o opcode foi lido).
 *  - `Treturn` repassa o valor de retorno ao frame anterior (caller) e força a finalização
 *    da execução do método atual definindo `pc` para `code_length`.
 */

#include "bytecode/impl/control.h"

/**
 * @brief Implementa o opcode `goto` (jump incondicional).
 *
 * Lê um offset de 16 bits (big-endian) imediatamente após o opcode e atualiza o
 * PC somando esse offset (relativo ao PC atual).
 *
 * Formato: goto <branchbyte1> <branchbyte2>
 *
 * @param f Ponteiro para o Frame atual.
 */
void _goto(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    f->pc += (int16_t)((code[f->pc + 1] << 8) | code[f->pc + 2]);
}

/**
 * @brief Implementa o opcode `tableswitch`.
 *
 * Estrutura (após padding para 4-byte alignment):
 *   defaultbyte (4 bytes)
 *   low         (4 bytes)
 *   high        (4 bytes)
 *   jump_offsets [high-low+1] (cada 4 bytes)
 *
 * O valor do índice é obtido do topo da pilha (pop int). Se o índice estiver fora
 * do intervalo [low, high], o pc é ajustado para `start + default`. Caso contrário,
 * o pc aponta para `start + jump_offsets[index - low]`.
 *
 * @param f Ponteiro para o Frame atual.
 */
void tableswitch(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 start = f->pc;

    /* Avança até o próximo alinhamento 4-byte após o opcode */
    while ((++f->pc) % 4 != 0)
        continue;

    int32_t _default = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t low = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t high = get_switch_32B_values(f->pc, code);
    f->pc += 4;

    int32_t i = pop_operand(f).value.t._int;

    if (i < low || i > high)
        f->pc = start + _default;
    else
        f->pc = start + get_switch_32B_values(f->pc + 4 * (i - low), code);
}

/**
 * @brief Implementa o opcode `lookupswitch`.
 *
 * Estrutura (após padding):
 *   defaultbyte (4 bytes)
 *   npairs      (4 bytes)
 *   (match, offset) pairs - cada par com 4 bytes match e 4 bytes offset
 *
 * Lê a chave (int) do topo da pilha, procura um par correspondente e, se encontrado,
 * salta para `start + offset`. Se não encontrar, salta para `start + default`.
 *
 * @param f Ponteiro para o Frame atual.
 */
void lookupswitch(Frame *f)
{
    u1 *code = f->method->bytecode.code;
    u4 start = f->pc;

    /* Avança até o alinhamento 4-byte */
    while ((++f->pc) % 4 != 0)
        continue;

    int32_t _default = get_switch_32B_values(f->pc, code);
    f->pc += 4;
    int32_t npairs = get_switch_32B_values(f->pc, code);
    f->pc += 4;

    int32_t key = pop_operand(f).value.t._int;

    int32_t match;
    for (int32_t i = 0; i < npairs; i++)
    {
        match = get_switch_32B_values(f->pc, code);
        if (key == match)
        {
            /* offset vem logo após o match (4 bytes) */
            f->pc = start + get_switch_32B_values(f->pc + 4, code);
            return;
        }
        f->pc += 8; /* avançar para o próximo par (match+offset) */
    }

    /* se não encontrou, usa default */
    f->pc = start + _default;
}

/**
 * @brief Implementa retornos com valor (ireturn, lreturn, freturn, dreturn, areturn).
 *
 * Popa o valor de retorno do frame atual e o push no operand_stack do frame anterior,
 * em seguida marca o método atual como finalizado ajustando o pc para code_length.
 *
 * Observação: assume-se que `f->previous_frame` é válido quando um método não é o top-level.
 *
 * @param f Ponteiro para o Frame atual.
 */
void Treturn(Frame *f)
{
    dtype ret = pop_operand(f);
    push_operand(f->previous_frame, ret);

    /* Sinaliza término do método atual */
    f->pc = f->method->bytecode.code_length;
}

/**
 * @brief Implementa `return` (retorno void).
 *
 * Simplesmente marca o método atual como terminado ajustando `pc` para o fim do código.
 *
 * @param f Ponteiro para o Frame atual.
 */
void _return(Frame *f)
{
    f->pc = f->method->bytecode.code_length;
}
