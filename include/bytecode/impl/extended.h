/**
 * @file extended.h
 * @brief Implementações de instruções estendidas da JVM (multianewarray, ifnull, ifnonnull, wide…).
 *
 * Este módulo contém instruções pertencentes ao grupo “extended” da JVM,
 * isto é, instruções que ampliam ou complementam as instruções base.
 * No escopo deste projeto, estão implementadas:
 *
 *  - `multianewarray` → criação de arrays multidimensionais
 *  - `ifnull`        → salto condicional baseado em referência nula
 *
 * Conforme a especificação JVM, essas instruções operam sobre valores
 * da pilha de operandos e manipulam tipos de referência.
 */

#ifndef BYTECODE_IMPL_EXTENDED_H
#define BYTECODE_IMPL_EXTENDED_H

#include "Frame.h"
#include "interpreter.h"

/**
 * @brief Implementa a instrução JVM `multianewarray`.
 *
 * Semântica:
 *  - Lê um CONSTANT_Class que representa o tipo base do array.
 *  - Lê o número de dimensões.
 *  - Retira da pilha os tamanhos das dimensões.
 *  - Aloca um array multidimensional e empilha a referência resultante.
 *
 * Exemplos:
 * ```
 * multianewarray [[I, 2  → cria matriz de int
 * multianewarray [[[Ljava/lang/String;, 3
 * ```
 *
 * @param frame Frame atual.
 */
void multianewarray(Frame *frame);

/**
 * @brief Implementa a instrução JVM `ifnull`.
 *
 * Semântica:
 *  - Retira o valor do topo da pilha.
 *  - Se for uma referência nula, realiza um salto (branch) para o offset fornecido.
 *
 * JVM Spec:
 * ```
 * ifnull <branchbyte1> <branchbyte2>
 * ```
 *
 * @param frame Frame atual.
 */
void if_null(Frame *frame);

#endif /* BYTECODE_IMPL_EXTENDED_H */
