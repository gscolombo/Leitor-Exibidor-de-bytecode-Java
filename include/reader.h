#ifndef READER_H_
#define READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "Classfile.h"
#include "utils.h"
#include "parser.h"
#include "uinteger.h"
#include "member.h"

#define LE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)

/** @file
 * @brief Declarações de funções relacionadas a leitura de arquivos `.class`.
 */

/**
 * @brief Abre e retorna um ponteiro para um arquivo .class.
 *
 * Essa função retorna um ponteiro de arquivo dado um
 * caminho para um arquivo `.class`. \n
 * São realizadas verificações mínimas para garantir que o caminho
 * passado é referente a arquivo `.class`.
 *
 * @param path Caminho relativo para um arquivo .class.
 */
FILE *open_classfile(const char *);

/**
 * @brief Lê um arquivo .class a partir de um ponteiro de arquivo.
 *
 * Com base na especificação da JVM 8, essa função lê _byte_ a _byte_ de
 * um arquivo `.class` a partir de um ponteiro de arquivo. \n
 * A quantidade de bytes do arquivo e a quantidade de _bytes_ lidos
 * também são apresentados após a execução da função.
 *
 * @param fptr Ponteiro para um arquivo .class.
 * @return Estrutura `Classfile` de um arquivo .class.
 */
ClassFile read_classfile(FILE *, bool);

/**
 * @brief Lê 1 byte a partir de um ponteiro de arquivo .class.
 *
 * Essa função é um _wrapper_ sobre a função `fread` para
 * padronizar a chamada para essa função com parâmetros pré-definidos
 * e melhorar a legibilidade do restante do código.
 *
 * @param FILE* Ponteiro para um arquivo .class.
 * @return Inteiro não-negativo de 1 byte.
 */
u1 read_u1(FILE *);

/**
 * @brief Lê 2 bytes a partir de um ponteiro de arquivo .class.
 *
 * Essa função é um _wrapper_ sobre a função `fread` para
 * padronizar sua chamada com parâmetros pré-definidos
 * e melhorar a legibilidade do restante do código. \n
 * Caso o _endianness_ do sistema em que programa foi compilado
 * seja _little-endian_, os _bytes_ são reordenados para corresponder
 * à ordenação _big-endian_ do arquivo .class, como especificado
 * para a JVM 8.
 *
 * @param FILE* Ponteiro para um arquivo .class.
 * @return Inteiro não-negativo de 2 byte.
 * @see u2swap
 */
u2 read_u2(FILE *);

/**
 * @brief Lê 4 bytes a partir de um ponteiro de arquivo .class.
 *
 * Essa função é um _wrapper_ sobre a função `fread` para
 * padronizar sua chamada com parâmetros pré-definidos
 * e melhorar a legibilidade do restante do código. \n
 * Caso o _endianness_ do sistema em que programa foi compilado
 * seja _little-endian_, os _bytes_ são reordenados para corresponder
 * à ordenação _big-endian_ do arquivo .class, como especificado
 * para a JVM 8.
 *
 * @param FILE* Ponteiro para um arquivo .class.
 * @return Inteiro não-negativo de 4 byte.
 */
u4 read_u4(FILE *);

/**
 * @brief Lê um membro (campo ou método) da classe do respectivo arquivo .class, a
 * partir de um ponteiro para esse arquivo.
 * 
 * Devido à estrutura similar para campos (`field_info`) e métodos (`method_info`) 
 * no arquivo `.class` na especificação da JVM 8, essa função lê as informações 
 * referentes à ambas as estruturas e armazena em uma estrutura `member_info`.
 *
 * @param cp Ponteiro para uma estrutura `cp_info`.
 * @param count Número de membros a serem lidos.
 * @param info Ponteiro para uma estrutura `member_info`.
 * @param fptr Ponteiro para um arquivo .class.
 */
void read_member(const cp_info *, u2, member_info *, FILE *);

/**
 * @brief Lê os atributos de arquivo .class a partir de um ponteiro para esse arquivo.
 * 
 * Essa função lê uma quantidade `count` de atributos como descritos na especificação da JVM 8
 * e preenche um _array_ de atributos com as informações respectivas para cada tipo de atributo. \n
 * Nem todos os tipos são considerados.
 *
 * @param cp Ponteiro para uma estrutura `cp_info`.
 * @param count Número de atributos a serem lidos.
 * @param fptr Ponteiro para um arquivo .class.
 * @param attr Ponteiro para uma _array_ de estruturas `attribute`.
 * @see attribute
 */
void read_attributes(const cp_info *, u2, FILE *, attribute *);

#endif