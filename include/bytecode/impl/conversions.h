/**
 * @file conversions.h
 * @brief Implementações das instruções de conversão entre tipos primitivos da JVM.
 *
 * A JVM possui diversas instruções específicas para converter valores entre
 * int, long, float e double. Este módulo implementa dois grupos essenciais:
 *
 *  - `i2T` → conversões a partir de int (i2b, i2c, i2s…)
 *  - `d2T` → conversões a partir de double
 *
 * As variantes específicas (i2f, d2i, etc.) são resolvidas conforme o opcode
 * previamente lido, cujo comportamento é delegado a estas funções genéricas.
 */

#ifndef BYTECODE_IMPL_CONVERSIONS_H
#define BYTECODE_IMPL_CONVERSIONS_H

#include "Frame.h"
#include "interpreter.h"

/**
 * @brief Implementa conversões do tipo `i2X` (int → outro tipo).
 *
 * Exemplos de instruções atendidas:
 *  - `i2l`  → int → long
 *  - `i2f`  → int → float
 *  - `i2d`  → int → double
 *  - `i2b`  → int → byte
 *  - `i2c`  → int → char
 *  - `i2s`  → int → short
 *
 * Semântica:
 *  - Retira um valor int da pilha.
 *  - Converte para o tipo indicado.
 *  - Empilha o valor convertido.
 *
 * @param frame Frame atual.
 */
void i2T(Frame *frame);

/**
 * @brief Implementa conversões do tipo `d2X` (double → outros tipos).
 *
 * Exemplos:
 *  - `d2i` → double → int
 *  - `d2l` → double → long
 *  - `d2f` → double → float
 *
 * Semântica:
 *  - Retira um valor double (CAT2) da pilha.
 *  - Converte para o tipo indicado pelo opcode.
 *  - Empilha o valor convertido.
 *
 * @param frame Frame atual.
 */
void d2T(Frame *frame);

#endif /* BYTECODE_IMPL_CONVERSIONS_H */
