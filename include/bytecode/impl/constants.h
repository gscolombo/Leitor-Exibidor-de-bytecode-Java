/**
 * @file constants.h
 * @brief Implementações das instruções JVM relacionadas a carregamento de constantes.
 *
 * Estas instruções colocam valores constantes diretamente na pilha de operandos.
 * Exemplos:
 *  - `aconst_null`
 *  - `iconst_0` … `iconst_5`
 *  - `bipush`, `sipush`
 *  - `ldc`, `ldc_w`, `ldc2_w`
 */

#ifndef BYTECODE_IMPL_CONSTANTS_H
#define BYTECODE_IMPL_CONSTANTS_H

#include "Frame.h"
#include "interpreter.h"

/**
 * @brief Implementa `aconst_null`.
 *
 * Empilha uma referência nula.
 *
 * @param frame Frame atual.
 */
void aconst_null(Frame *frame);

/**
 * @brief Implementa `iconst_<i>` para inteiros pequenos (0–5).
 *
 * O valor a ser empilhado é inferido do opcode.
 *
 * @param frame Frame atual.
 */
void iconst_i(Frame *frame);

/**
 * @brief Implementa `fconst_<f>` para floats pequenos (0.0f, 1.0f, 2.0f).
 *
 * @param frame Frame atual.
 */
void fconst_f(Frame *frame);

/**
 * @brief Implementa `dconst_0` e `dconst_1`.
 *
 * Empilha valores double constantes.
 *
 * @param frame Frame atual.
 */
void dconst_d(Frame *frame);

/**
 * @brief Implementa `bipush`.
 *
 * Empilha um valor byte sign-extended.
 *
 * @param frame Frame atual.
 */
void bipush(Frame *frame);

/**
 * @brief Implementa `sipush`.
 *
 * Empilha um valor short sign-extended.
 *
 * @param frame Frame atual.
 */
void sipush(Frame *frame);

/**
 * @brief Implementa `ldc`, `ldc_w` e `ldc2_w`.
 *
 * Carrega constantes do constant pool, podendo ser:
 *  - String
 *  - int / float
 *  - long / double
 *  - Class reference
 *
 * @param frame Frame atual.
 */
void ldc_(Frame *frame);

#endif /* BYTECODE_IMPL_CONSTANTS_H */
