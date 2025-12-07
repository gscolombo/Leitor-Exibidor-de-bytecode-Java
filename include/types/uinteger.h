/**
 * @file include/types/uinteger.h
 * @brief Definições de tipos inteiros sem sinal utilizados pela JVM e pelo leitor/exibidor de bytecode.
 *
 * A especificação da JVM utiliza tipos inteiros padronizados:
 *  - u1  → inteiro sem sinal de 1 byte
 *  - u2  → inteiro sem sinal de 2 bytes
 *  - u4  → inteiro sem sinal de 4 bytes
 *
 * Estes tipos são empregados em praticamente todas as estruturas do formato
 * `.class`, incluindo constant pool, atributos, tabelas de exceções, versões,
 * índices e comprimentos.
 */

#ifndef TYPES_UINTEGER_H
#define TYPES_UINTEGER_H

#include <stdint.h>

/**
 * @typedef u1
 * @brief Inteiro sem sinal de 8 bits.
 *
 * Representa valores entre 0 e 255.  
 * Usado para bytes individuais lidos diretamente do arquivo .class.
 */
typedef uint8_t u1;

/**
 * @typedef u2
 * @brief Inteiro sem sinal de 16 bits.
 *
 * Representa valores entre 0 e 65535.  
 * Utilizado frequentemente para índices do constant pool, contagem de elementos
 * e valores de certos campos estruturais da JVM.
 */
typedef uint16_t u2;

/**
 * @typedef u4
 * @brief Inteiro sem sinal de 32 bits.
 *
 * Representa valores entre 0 e 4.294.967.295.  
 * Usado para comprimentos maiores, valores numéricos (Integer, Float),
 * offsets e partes do encoding de tipos 64 bits (Long, Double).
 */
typedef uint32_t u4;

#endif /* TYPES_UINTEGER_H */
