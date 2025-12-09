/**
 * @file stack.h
 * @brief Implementações das instruções de manipulação da pilha de operandos da JVM.
 *
 * As instruções definidas aqui atuam diretamente sobre o `operand_stack` do Frame,
 * permitindo remover, duplicar ou reorganizar valores no topo da pilha.
 *
 * Essas operações são fundamentais para diversas instruções JVM e aparecem
 * frequentemente em bytecodes compilados.
 */

#ifndef BYTECODE_IMPL_STACK_H
#define BYTECODE_IMPL_STACK_H

#include "Frame.h"
#include "interpreter.h"

/* ========================================================================== */
/* pop                                                                        */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `pop`.
 *
 * Remove o valor do topo da pilha de operandos.
 * Esta operação é usada quando o resultado de uma expressão é descartado.
 *
 * JVM Spec:
 * ```
 * pop
 * ```
 *
 * @param frame Frame atual de execução.
 */
void pop_(Frame *frame);

/* ========================================================================== */
/* dup                                                                        */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `dup`.
 *
 * Duplica o valor do topo da pilha e empilha o duplicado acima do original:
 *
 * Antes:
 * ```
 * ..., value
 * ```
 *
 * Depois:
 * ```
 * ..., value, value
 * ```
 *
 * Requer que o valor seja categoria 1 (CAT1).
 *
 * @param frame Frame atual de execução.
 */
void dup(Frame *frame);

/* ========================================================================== */
/* dup2                                                                       */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `dup2`.
 *
 * Duplica um valor de categoria 2 (CAT2) OU dois valores consecutivos de categoria 1.
 *
 * Comportamentos possíveis:
 * - Se o topo é CAT2 (`long`, `double`):
 *   ```
 *   ..., v2
 *   → ..., v2, v2
 *   ```
 *
 * - Se os dois valores no topo são CAT1:
 *   ```
 *   ..., v1, v2
 *   → ..., v1, v2, v1, v2
 *   ```
 *
 * @param frame Frame atual de execução.
 */
void dup2(Frame *frame);

#endif /* BYTECODE_IMPL_STACK_H */
