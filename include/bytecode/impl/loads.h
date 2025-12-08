/**
 * @file loads.h
 * @brief Implementações das instruções JVM da família *load*.
 *
 * Instruções `load` colocam valores das variáveis locais no topo da pilha
 * de operandos. Incluem versões genéricas e versões compactas (`_n`).
 *
 * Exemplos de instruções load:
 *   - `iload`, `fload`, `aload`, `lload`, `dload`
 *   - `iload_0` … `iload_3`
 *   - `aload_0` … `aload_3`
 *
 * Estas funções implementam exatamente a semântica definida pela JVM.
 */

#ifndef BYTECODE_IMPL_LOADS_H
#define BYTECODE_IMPL_LOADS_H

#include "Frame.h"
#include "interpreter.h"

/* ========================================================================== */
/* Generic Load                                                               */
/* ========================================================================== */

/**
 * @brief Implementa a instrução genérica `_load` (int, float, long, double, reference).
 *
 * Semântica geral:
 *  - Lê o índice da variável local a partir do byte seguinte do bytecode.
 *  - Carrega o valor presente em `local_variables[index]`.
 *  - Empilha esse valor na operand stack.
 *
 * Esta função é usada internamente por instruções específicas (`iload`, `aload`, etc.).
 *
 * @param frame Frame atual.
 */
void _load(Frame *frame);

/* ========================================================================== */
/* Compact Load Variants (_n)                                                 */
/* ========================================================================== */

/**
 * @brief Implementa instruções `iload_0`, `iload_1`, `iload_2`, `iload_3`.
 *
 * O índice da variável local é inferido diretamente do opcode.
 *
 * Categoria: Tipos CAT1 (int).
 *
 * @param frame Frame atual.
 */
void iload_n(Frame *frame);

/**
 * @brief Implementa instruções `lload_0`, `lload_1`, `lload_2`, `lload_3`.
 *
 * Para tipos `long`, categoria CAT2.
 *
 * @param frame Frame atual.
 */
void lload_n(Frame *frame);

/**
 * @brief Implementa instruções `dload_0`, `dload_1`, `dload_2`, `dload_3`.
 *
 * Para tipo `double`, categoria CAT2.
 *
 * @param frame Frame atual.
 */
void dload_n(Frame *frame);

/**
 * @brief Implementa instruções `aload_0`, `aload_1`, `aload_2`, `aload_3`.
 *
 * Usado para carregar referências (objetos, arrays, strings…).
 *
 * @param frame Frame atual.
 */
void aload_n(Frame *frame);

/* ========================================================================== */
/* Array Loads                                                                 */
/* ========================================================================== */

/**
 * @brief Implementa instruções `TALOAD` genéricas (`iaload`, `aaload`, `daload`, etc.).
 *
 * Semântica:
 *  - Retira o índice do topo da pilha.
 *  - Retira a referência ao array.
 *  - Acessa `array[index]`.
 *  - Empilha o valor encontrado.
 *
 * Instruções atendidas por esta função:
 *   - `iaload`, `laload`, `faload`, `daload`
 *   - `aaload` (arrays de referência)
 *   - `baload`, `caload`, `saload`
 *
 * @param frame Frame atual.
 */
void Taload(Frame *frame);

#endif /* BYTECODE_IMPL_LOADS_H */
