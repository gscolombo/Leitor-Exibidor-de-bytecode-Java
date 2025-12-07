/**
 * @file cp/parser.h
 * @brief Prototótipos para o parser do constant pool e utilitários de decodificação.
 *
 * Este header declara as funções usadas para:
 *  - interpretar/parsear o constant pool do arquivo .class
 *  - decodificar strings no formato Modified UTF-8 usado pela JVM
 *  - reconstruir valores numéricos (float, long, double) a partir de suas
 *    representações em bytes (u4/u4)
 *
 * As implementações correspondentes geralmente residem em `src/cp/parser.c`.
 */

#ifndef CP_PARSER_H
#define CP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"

/**
 * @brief Faz o parse completo do constant pool a partir do stream do .class.
 *
 * Lê `constant_pool_count` entradas do arquivo `f` (seguindo a especificação da JVM)
 * e retorna um array alocado de `cp_info` com as entradas interpretadas.
 *
 * Observações:
 * - A função aloca dinamicamente o array de cp_info e possivelmente buffers internos
 *   (por exemplo, para strings UTF8). É responsabilidade do chamador liberar
 *   essa memória (ex.: via free_classfile()).
 * - A contagem `constant_pool_count` normalmente é lida do arquivo (.class) e
 *   passada para esta função diretamente.
 *
 * @param f Ponteiro para o FILE já posicionado após a leitura do constant_pool_count.
 * @param constant_pool_count Número de entradas no constant pool (u2).
 * @return cp_info* Ponteiro para o array alocado de cp_info. Retorna NULL em erro.
 */
cp_info* parse_constant_pool(FILE* f, u2 constant_pool_count);

/**
 * @brief Decodifica uma string em Modified UTF-8 para wchar_t*.
 *
 * A JVM armazena strings UTF-8 em um formato ligeiramente modificado (modified UTF-8).
 * Esta função recebe o tamanho em bytes (u2) da sequência codificada e o buffer `data`
 * contendo os bytes, e retorna uma string wide (`wchar_t*`) alocada dinamicamente
 * com o conteúdo decodificado.
 *
 * Observações:
 * - O buffer retornado deve ser liberado com `free()` pelo chamador.
 * - Em caso de erro de alocação ou sequência inválida, a função pode retornar NULL.
 *
 * @param len Número de bytes do texto em modified UTF-8 (u2).
 * @param data Ponteiro para os bytes codificados (u1*).
 * @return wchar_t* String wide alocada contendo o texto decodificado, ou NULL em erro.
 */
wchar_t* decode_modified_utf8_str(u2 len, const u1* data);

/**
 * @brief Reconstrói um valor float a partir de 4 bytes (u4) seguindo IEEE 754.
 *
 * Interpreta `bytes` como a representação em 32 bits de um float (big-endian
 * conforme leitura do .class) e retorna o `float` correspondente.
 *
 * @param bytes Valor u4 contendo os 4 bytes que representam o float.
 * @return float Valor em ponto flutuante reconstruído.
 */
float decode_float_bytes(u4 bytes);

/**
 * @brief Reconstrói um valor long (64 bits) a partir de duas palavras de 32 bits.
 *
 * Recebe a parte alta (high_bytes) e a parte baixa (low_bytes) do long (ambas u4)
 * e combina-as para retornar o `long` (64-bit).
 *
 * @param high_bytes Parte alta do valor (u4).
 * @param low_bytes Parte baixa do valor (u4).
 * @return long Valor long (64 bits) reconstruído.
 */
long decode_long_bytes(u4 high_bytes, u4 low_bytes);

/**
 * @brief Reconstrói um valor double (64 bits) a partir de duas palavras de 32 bits.
 *
 * Recebe a parte alta e baixa do double (u4 cada), combina-as em um valor de 64 bits
 * e interpreta segundo o formato IEEE 754 para retornar o `double`.
 *
 * @param high_bytes Parte alta (u4).
 * @param low_bytes Parte baixa (u4).
 * @return double Valor de ponto flutuante dupla precisão reconstruído.
 */
double decode_double_bytes(u4 high_bytes, u4 low_bytes);

#endif /* CP_PARSER_H */
