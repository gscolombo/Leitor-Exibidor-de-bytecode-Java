// include/types/uinteger.h
#ifndef TYPES_UINTEGER_H
#define TYPES_UINTEGER_H

#include <stdint.h>

/**
 * @file uinteger.h
 * @brief Tipos inteiros sem sinal com tamanho explícito usados pelo projeto.
 *
 * Estes tipos representam as unidades primárias usadas ao ler arquivos .class
 * e ao manipular estruturas internas da JVM (u1, u2, u4 conforme especificação).
 */

/**
 * @typedef u1
 * @brief Inteiro sem sinal de 1 byte (8 bits).
 *
 * Usado para representar bytes individuais, opcodes, flags pequenos e valores de 8 bits.
 */
typedef uint8_t u1;

/**
 * @typedef u2
 * @brief Inteiro sem sinal de 2 bytes (16 bits).
 *
 * Usado para índices do constant pool, contadores, access_flags e outros valores de 16 bits.
 */
typedef uint16_t u2;

/**
 * @typedef u4
 * @brief Inteiro sem sinal de 4 bytes (32 bits).
 *
 * Usado para tamanhos de atributos, comprimentos de código, magic numbers e valores de 32 bits.
 */
typedef uint32_t u4;

#endif /* TYPES_UINTEGER_H */
