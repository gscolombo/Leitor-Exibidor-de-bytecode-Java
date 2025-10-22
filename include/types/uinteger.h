#ifndef TYPES_UINTEGER_H
#define TYPES_UINTEGER_H

#include <stdint.h>

/** @file */

/**
 * @typedef u1
 * @brief Inteiro não negativo de 1 byte.
 * @details Usado para representar bytes individuais, opcodes, e pequenos valores numéricos.
 */
typedef uint8_t u1;

/**
 * @typedef u2
 * @brief Inteiro não negativo de 2 bytes.
 * @details Usado para índices de constant pool, contagens, e valores de 16 bits.
 */
typedef uint16_t u2;

/**
 * @brief Inteiro não negativo de 4 bytes.
 * @details Usado para tamanhos de atributos, comprimentos de código, e valores de 32 bits.
 */
typedef uint32_t u4;

#endif