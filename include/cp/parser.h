#ifndef CP_PARSER_H
#define CP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"

/** @file parser.h
 *  @brief Funções para leitura do _pool_ de constantes e decodificação de _bytes_ de constantes.
 */

/**
 * @brief Lê as constantes do pool de constantes em um arquivo .class.
 * 
 * Após alocar espaço na memória para um _array_ de estruturas `cp_info` com base na quantidade `count` de constantes,
 * o _byte_ correspondente a _tag_ para cada constante é lido e utilizado para ler o restante das 
 * informações do tipo de constante correspondente.
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
 * São realizadas duas passagens pela sequência de _bytes_: a primeira para definir o tamanho da _string_ final
 * e a segunda para decodificar os _code points_ no formato UTF-8 modificado, com base na especificação da JVM 8.
 * Para melhor compatibilidade, é utilizado o tipo de caractere extendido `char` para compor a _string_ final.
 * 
 * @param length Quantidade de bytes da string.
 * @param bytes Array de bytes.
 * 
 * @return String no formato UTF-8 modificado correspondente ao array de bytes.
 */
char* decode_modified_utf8_str(u2, const u1*);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo float.
 * 
 * Essa é uma implementação direta da especificação da JVM 8 para 
 * decoficação de valores `float` a partir da sequência de _bytes_ correspondente.\n
 * Nesse caso, a função pode retornar tanto um valor representando ± infinito, o valor `NaN`
 * ou um número real de precisão simples.
 * 
 * @param b Array de bytes.
 * 
 * @return `float` correspondente à sequência de bytes.
 */
float decode_float_bytes(u4);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo long.
 * 
 * Essa é uma implementação direta da especificação da JVM 8 para 
 * decoficação de valores `long` a partir da sequência de _bytes_ correspondente.
 * 
 * @param b Array de bytes.
 * 
 * @return `long` correspondente à sequência de bytes.
 */
long decode_long_bytes(u4, u4);

/**
 * @brief Decodifica uma sequência de bytes correspondente a um valor do tipo double.
 * 
 * Essa é uma implementação direta da especificação da JVM 8 para 
 * decoficação de valores `double` a partir da sequência de _bytes_ correspondente.\n
 * Nesse caso, a função pode retornar tanto um valor representando ± infinito, o valor `NaN`
 * ou um número real de precisão dupla.
 * 
 * @param b Array de bytes.
 * 
 * @return `double` correspondente à sequência de bytes.
 */
double decode_double_bytes(u4, u4);

#endif