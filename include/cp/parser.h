#ifndef CP_PARSER_H
#define CP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"

/**
 * @brief Lê as constantes do pool de constantes em um arquivo .class.
 * 
 * @param fptr Ponteiro para um arquivo .class.
 * @param count Número de constantes no pool de constantes.
 * 
 * @return Ponteiro para uma estrutura `cp_info` representando o pool de constantes.
 */
cp_info* parse_constant_pool(FILE*, u2);

/**
 * @brief Decodifica uma sequência de bytes no formato UTF-8 modificado.
 * 
 * @param length Quantidade de bytes da string.
 * @param bytes Array de bytes.
 * 
 * @return String no formato UTF-8 modificado correspondente ao array de bytes.
 */
wchar_t* decode_modified_utf8_str(u2, const u1*);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo float.
 * 
 * @param b Array de bytes.
 * 
 * @return Float correspondente à sequência de bytes.
 */
float decode_float_bytes(u4);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo long.
 * 
 * @param b Array de bytes.
 * 
 * @return Long correspondente à sequência de bytes.
 */
long decode_long_bytes(u4, u4);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo double.
 * 
 * @param b Array de bytes.
 * 
 * @return Long correspondente à sequência de bytes.
 */
double decode_double_bytes(u4, u4);

#endif