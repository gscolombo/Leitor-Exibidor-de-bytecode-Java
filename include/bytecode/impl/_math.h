/**
 * @file _math.h
 * @brief Declarações das instruções JVM relacionadas a operações aritméticas.
 *
 * Este módulo implementa as instruções aritméticas da JVM que atuam sobre valores
 * presentes na pilha de operandos. As operações abrangem inteiros (`int`), 
 * inteiros longos (`long`) e números de ponto flutuante (`double`).
 *
 * Instruções contempladas:
 *  - Aritmética com `int`: `iadd`, `isub`, `imul`, `iinc`
 *  - Aritmética com `long`: `lsub`
 *  - Aritmética com `double`: `dadd`, `dsub`, `dmul`, `ddiv`, `dneg`, `drem`
 */

#ifndef BYTECODE_IMPL_MATH_H
#define BYTECODE_IMPL_MATH_H

#include "interpreter.h"
#include "Frame.h"

/**
 * @brief Implementa a instrução `iadd`.
 *
 * Remove dois valores inteiros da pilha de operandos, soma-os e empilha
 * o resultado como um `int`.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void iadd(Frame *);

/**
 * @brief Implementa a instrução `isub`.
 *
 * Remove dois valores inteiros da pilha, subtrai o segundo do primeiro
 * e empilha o resultado.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void isub(Frame *);

/**
 * @brief Implementa a instrução `imul`.
 *
 * Remove dois inteiros da pilha e empilha o produto entre eles.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void imul(Frame *);

/**
 * @brief Implementa a instrução `iinc`.
 *
 * Incrementa diretamente o valor de uma variável local `int` por um valor
 * constante fornecido no bytecode, sem usar a pilha de operandos.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void iinc(Frame *);

/**
 * @brief Implementa a instrução `lsub`.
 *
 * Remove dois valores `long` da pilha, subtrai o segundo do primeiro e
 * empilha o resultado.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void lsub(Frame *);

/**
 * @brief Implementa a instrução `dadd`.
 *
 * Remove dois valores `double` da pilha e empilha sua soma.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void dadd(Frame *);

/**
 * @brief Implementa a instrução `dsub`.
 *
 * Remove dois valores `double`, realiza a subtração (primeiro - segundo)
 * e empilha o resultado.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void dsub(Frame *);

/**
 * @brief Implementa a instrução `dmul`.
 *
 * Remove dois valores `double` e empilha o produto.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void dmul(Frame *);

/**
 * @brief Implementa a instrução `ddiv`.
 *
 * Remove dois valores `double`, divide o primeiro pelo segundo e empilha
 * o resultado. Segue a semântica IEEE 754 para divisão por zero.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void ddiv(Frame *);

/**
 * @brief Implementa a instrução `dneg`.
 *
 * Inverte o sinal do valor `double` no topo da pilha.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void dneg(Frame *);

/**
 * @brief Implementa a instrução `drem`.
 *
 * Remove dois valores `double` e empilha o resto da divisão (operador remainder).
 * Segue a semântica IEEE 754, onde `a % b = a - b * trunc(a/b)`.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void drem(Frame *);

#endif
