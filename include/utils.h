/**
 * @file utils.h
 * @brief Declarações de funções utilitárias usadas em diversos módulos do leitor/exibidor de bytecode Java.
 *
 * Este módulo concentra utilidades gerais, como operações de endianess,
 * manipulação de strings de flags, conversão de descritores da JVM para formato legível,
 * e utilitários auxiliares para atributos e membros.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uinteger.h"
#include "attribute_enum.h"
#include "member.h"

/**
 * @brief Realiza o swap de bytes de um inteiro de 16 bits (`u2`).
 *
 * Converte um valor armazenado em ordem little-endian para big-endian, ou vice-versa.
 * A operação consiste em mover os bytes superior e inferior para suas posições opostas.
 *
 * Exemplo ilustrativo:
 * ```
 * n = 0x1234
 * swap = 0x3412
 * ```
 *
 * @param n Inteiro de 16 bits cujo endianess deve ser invertido.
 * @return Valor de 16 bits com bytes invertidos.
 */
unsigned int u2swap(unsigned int n);

/**
 * @brief Inverte a ordem dos 4 bytes de um inteiro de 32 bits (`u4`).
 *
 * Útil para interpretar valores lidos de arquivos `.class`, que seguem a convenção big-endian.
 * A função utiliza operações bitwise para reorganizar cada byte.
 *
 * Exemplo:
 * ```
 * n = 0x12345678
 * swap = 0x78563412
 * ```
 *
 * @param n Inteiro de 32 bits a ser convertido.
 * @return Inteiro de 32 bits com bytes reorganizados.
 */
unsigned int u4swap(unsigned int n);

/**
 * @brief Calcula quantos dígitos possui um inteiro não negativo.
 *
 * A função divide repetidamente o número por 10 até que reste apenas um dígito,
 * incrementando um contador a cada iteração.
 *
 * Exemplo:
 * ```
 * n = 2048 → retorna 4
 * ```
 *
 * @param n Inteiro não negativo.
 * @return Número de dígitos de `n`.
 */
unsigned int num_digits(unsigned int n);

/**
 * @brief Concatena nomes de flags presentes em um conjunto de bits.
 *
 * A função interpreta um campo de flags de 16 bits e, com base no mapa fornecido,
 * gera uma string contendo os nomes das flags ativas, separadas por `sep`.
 *
 * Regras:
 * - Cada flag é comparada ao mapa (`FlagMap`).
 * - Se a flag estiver presente no valor `flags`, seu nome é adicionado à saída.
 * - Se `sep` for NULL, usa-se um espaço como separador.
 *
 * @param flags Valor de 16 bits com bits representando flags de acesso.
 * @param n Quantidade de elementos no array `flag_map`.
 * @param sep String separadora entre os nomes das flags (ou espaço se NULL).
 * @param flag_map Array contendo pares `{mask, name}`.
 * @return String dinâmica contendo os nomes concatenados das flags.
 *
 * @note A string retornada deve ser liberada pelo chamador.
 */
char *parse_flags(u2 flags, size_t n, const char *sep, const FlagMap flag_map[]);

/**
 * @brief Converte o nome textual de um atributo no seu valor enumerado (`attribute_name`).
 *
 * A JVM define diversos atributos (Code, SourceFile, Exceptions, etc.).
 * Esta função mapeia o nome lido do constant pool para seu tipo enumerado interno.
 *
 * @param name Nome textual do atributo conforme armazenado no constant pool.
 * @return Ponteiro para o valor enumerado correspondente, ou NULL se desconhecido.
 */
const attribute_name *convert_attr_name(const char *name);

/**
 * @brief Converte um descritor da JVM para um formato legível para humanos.
 *
 * Descritores são cadeias compactas definidas pela JVM para representar tipos, campos e assinaturas de métodos.
 * Esta função traduz esse formato para sua representação mais intuitiva.
 *
 * Exemplos:
 * ```
 * I → int
 * [Ljava/lang/String; → java.lang.String[]
 * (ID)V → void(int, double)
 * ```
 *
 * @param descriptor Descritor segundo a especificação da JVM.
 * @param sep Separador opcional entre os tokens traduzidos (pode ser NULL).
 * @return String contendo o descritor convertido em formato legível.
 *
 * @note A string retornada deve ser liberada pelo chamador.
 */
char *parse_descriptor(const char *descriptor, char *sep);

#endif /* UTILS_H_ */
