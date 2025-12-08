/**
 * @file stores.h
 * @brief Implementações das instruções *store* da JVM (armazenamento em variáveis locais).
 *
 * As instruções de armazenamento retiram valores do topo da pilha de operandos
 * e os colocam em um índice específico do array de variáveis locais do Frame.
 *
 * Exemplos:
 *  - `istore`      → armazena int
 *  - `dstore`      → armazena double
 *  - `astore`      → armazena referência
 *  - `istore_n`    → forma compacta (`istore_0` … `istore_3`)
 *
 * Essas funções implementam a semântica descrita na especificação da JVM.
 */

#ifndef BYTECODE_IMPL_STORES_H
#define BYTECODE_IMPL_STORES_H

#include "Frame.h"
#include "interpreter.h"

/* ========================================================================== */
/* Instruções store padrão                                                     */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `istore`.
 *
 * Retira um valor do topo da pilha de operandos (tipo int)
 * e o armazena no índice informado pelo próximo byte do bytecode.
 *
 * JVM Spec:
 * ```
 * istore <index>
 * ```
 *
 * @param frame Frame atual em execução.
 */
void istore(Frame *frame);

/**
 * @brief Implementa a instrução JVM `astore`.
 *
 * Armazena referências (objetos, arrays, strings…) em variáveis locais.
 *
 * JVM Spec:
 * ```
 * astore <index>
 * ```
 *
 * @param frame Frame atual.
 */
void astore(Frame *frame);

/* ========================================================================== */
/* Instruções store compactas (_n variants)                                    */
/* ========================================================================== */

/**
 * @brief Implementa as variantes compactas `istore_0`, `istore_1`, `istore_2`, `istore_3`.
 *
 * O índice é inferido do opcode e não faz parte do bytecode seguinte.
 *
 * @param frame Frame atual.
 */
void istore_n(Frame *frame);

/**
 * @brief Implementa as variantes compactas de armazenamento de valores double.
 *
 * Especificamente: `dstore_0`, `dstore_1`, `dstore_2`, `dstore_3`.
 *
 * @param frame Frame atual.
 */
void dstore_n(Frame *frame);

/**
 * @brief Implementa as variantes compactas `astore_0`, `astore_1`, `astore_2`, `astore_3`.
 *
 * Armazena referências em variáveis locais indexadas implicitamente.
 *
 * @param frame Frame atual.
 */
void astore_n(Frame *frame);

/* ========================================================================== */
/* Instruções store genérica                                                   */
/* ========================================================================== */

/**
 * @brief Implementa o comportamento genérico de armazenamento parametrizado por tipo.
 *
 * Função auxiliar usada para reduzir duplicação de código entre várias instruções store.
 * A semântica depende do tipo de dado retirado da pilha.
 *
 * JVM Spec correspondente:
 *  - istore
 *  - fstore
 *  - dstore
 *  - lstore
 *  - astore
 *
 * @param frame Frame atual.
 */
void Tastore(Frame *frame);

#endif /* BYTECODE_IMPL_STORES_H */
