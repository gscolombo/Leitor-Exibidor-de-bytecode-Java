#ifndef READER_H_
#define READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Classfile.h"
#include "utils.h"
#include "parser.h"
#include "uinteger.h"
#include "member.h"

#define LE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)

/** @file */

/**
 * @brief Abre e retorna um ponteiro para um arquivo .class.
 * 
 * @param path Caminho relativo para um arquivo .class.
 */
FILE *open_classfile(const char *);

/**
 * @brief Lê um arquivo .class a partir de um ponteiro de arquivo.
 * 
 * @param fptr Ponteiro para um arquivo .class.
 * @return Estrutura `Classfile` de um arquivo .class.
 */
ClassFile read_classfile(FILE *);

/**
 * @brief Lê 1 byte a partir de um ponteiro de arquivo .class.
 * 
 * @param FILE* Ponteiro para um arquivo .class.
 * @return Inteiro não-negativo de 1 byte.
 */
u1 read_u1(FILE *);

/**
 * @brief Lê 2 bytes a partir de um ponteiro de arquivo .class.
 * 
 * Case o endianness do sistema em que programa foi compilado
 * seja little-endian, os bytes são reordenados para corresponder
 * à ordenação big-endian no arquivo .class, como especificado
 * para a JVM 8.
 * 
 * @param FILE* Ponteiro para um arquivo .class.
 * @return Inteiro não-negativo de 2 byte.
 */
u2 read_u2(FILE *);

/**
 * @brief Lê 4 bytes a partir de um ponteiro de arquivo .class.
 * 
 * Case o endianness do sistema em que programa foi compilado
 * seja little-endian, os bytes são reordenados para corresponder
 * à ordenação big-endian no arquivo .class, como especificado
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
 * @param cp Ponteiro para uma estrutura `cp_info`.
 * @param count Número de membros a serem lidos.
 * @param info Ponteiro para uma estrutura `member_info`.
 * @param fptr Ponteiro para um arquivo .class.
 */
void read_member(const cp_info *, u2, member_info *, FILE *);

/**
 * @brief Lê os atributos de arquivo .class a partir de um ponteiro para esse arquivo.
 * 
 * @param cp Ponteiro para uma estrutura `cp_info`.
 * @param count Número de atributos a serem lidos.
 * @param fptr Ponteiro para um arquivo .class.
 * @param attr Ponteiro para uma estrutura `attribute`.
 */
void read_attributes(const cp_info *, u2, FILE *, attribute *);

#endif