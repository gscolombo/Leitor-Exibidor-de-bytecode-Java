/**
 * @file comparisons.h
 * @brief Declarações das instruções JVM relacionadas a comparações e desvios condicionais.
 *
 * Este módulo implementa instruções responsáveis por comparar valores na pilha
 * de operandos e, dependendo do resultado, alterar o fluxo de execução através
 * de desvios condicionais. Também inclui instruções específicas de comparação
 * de números de ponto flutuante.
 *
 * Instruções contempladas:
 *  - `ifeq`, `ifne`, `iflt`, `ifge`, `ifgt`, `ifle` — comparações unárias sobre um único valor.
 *  - `if_icmpeq`, `if_icmpne`, `if_icmplt`, `if_icmpge`, `if_icmpgt`, `if_icmple` — comparações binárias de inteiros.
 *  - `dcmpg`, `dcmpl` — comparação entre valores `double` com semântica específica para NaN.
 */

#ifndef BYTECODE_IMPL_COMPARISONS_H
#define BYTECODE_IMPL_COMPARISONS_H

#include "Frame.h"
#include "interpreter.h"

/**
 * @brief Implementa instruções condicionais unárias (`if<cond>`).
 *
 * Remove um valor do topo da pilha de operandos e verifica a condição
 * especificada pela instrução (ex.: igual a zero, menor que zero, etc.).
 * Caso a condição seja verdadeira, realiza um salto relativo no bytecode.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void if_cond(Frame *);

/**
 * @brief Implementa instruções condicionais binárias de inteiros (`if_icmp<cond>`).
 *
 * Remove dois valores inteiros do topo da pilha, compara-os de acordo com
 * a condição definida pela instrução e, se satisfeita, altera o fluxo de execução
 * realizando um salto relativo.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void if_icmp_cond(Frame *);

/**
 * @brief Implementa instruções de comparação de valores `double` (`dcmpg` e `dcmpl`).
 *
 * Remove dois valores `double` do topo da pilha e empilha o resultado da comparação:
 *  - `1`  se o primeiro valor for maior que o segundo;
 *  - `0`  se forem iguais;
 *  - `-1` se o primeiro for menor;
 * A diferença entre `dcmpg` e `dcmpl` ocorre caso um dos operandos seja `NaN`,
 * definindo o valor empilhado conforme a semântica da instrução.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void dcmp_op(Frame *);

#endif
