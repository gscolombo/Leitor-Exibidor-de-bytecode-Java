/**
 * @file reader.h
 * @brief Declarações de funções responsáveis pela leitura de arquivos `.class` conforme a JVM 8.
 *
 * Este módulo contém funções utilitárias para abrir arquivos `.class`, ler valores primitivos
 * no formato big-endian, interpretar estruturas como membros e atributos, além de montar
 * a estrutura completa `ClassFile`.
 */

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

/**
 * @brief Macro para detectar se o sistema é Little Endian.
 *
 * Alguns valores lidos do arquivo `.class` devem ser reinterpretados
 * como _big-endian_, portanto a função de leitura deve ajustar os bytes
 * quando necessário.
 */
#define LittleEndian (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)

/**
 * @brief Abre e retorna um ponteiro para um arquivo `.class`.
 *
 * Realiza verificações simples no caminho passado para garantir que se trata
 * de um arquivo com extensão `.class`. Caso a abertura falhe, a função retorna `NULL`.
 *
 * @param path Caminho relativo ou absoluto para um arquivo `.class`.
 * @return Ponteiro `FILE*` caso a abertura seja bem-sucedida; caso contrário, `NULL`.
 */
FILE *open_classfile(const char *path);

/**
 * @brief Lê integralmente um arquivo `.class` a partir de um `FILE*`.
 *
 * A função percorre o arquivo conforme a especificação da JVM 8,
 * preenchendo uma estrutura `ClassFile` com as informações do bytecode.
 *
 * @param fptr Ponteiro para um arquivo `.class` previamente aberto.
 * @param verbose Define se informações extras devem ser exibidas durante a leitura.
 * @return Estrutura `ClassFile` preenchida com todas as informações lidas.
 */
ClassFile read_classfile(FILE *fptr, bool verbose);

/**
 * @brief Lê 1 byte de um arquivo `.class`.
 *
 * Wrapper sobre `fread` para padronizar leitura de valores de 1 byte.
 *
 * @param fptr Ponteiro válido para o arquivo `.class`.
 * @return Valor lido como `u1` (unsigned char).
 */
u1 read_u1(FILE *fptr);

/**
 * @brief Lê 2 bytes de um arquivo `.class`, com conversão opcional para big-endian.
 *
 * Se o sistema for little-endian, os bytes são invertidos conforme necessário.
 *
 * @param fptr Ponteiro válido para o arquivo `.class`.
 * @return Valor lido como `u2` (unsigned short).
 * @see u2swap
 */
u2 read_u2(FILE *fptr);

/**
 * @brief Lê 4 bytes de um arquivo `.class`, com conversão opcional para big-endian.
 *
 * Assim como em `read_u2`, faz o ajuste de endianess se necessário.
 *
 * @param fptr Ponteiro válido para o arquivo `.class`.
 * @return Valor lido como `u4` (unsigned int).
 */
u4 read_u4(FILE *fptr);

/**
 * @brief Lê membros (fields ou methods) de uma classe.
 *
 * Os itens `field_info` e `method_info` possuem estrutura idêntica na JVM 8.
 * Assim, esta função lê ambos através de uma mesma rotina, preenchendo um
 * vetor de estruturas `member_info`.
 *
 * @param cp Ponteiro para o constant pool (`cp_info`) usado para resolver nomes e descritores.
 * @param count Número de membros a serem lidos.
 * @param info Ponteiro para um vetor `member_info` previamente alocado.
 * @param fptr Ponteiro para o arquivo `.class`.
 */
void read_member(const cp_info *cp, u2 count, member_info *info, FILE *fptr);

/**
 * @brief Lê atributos de um arquivo `.class`.
 *
 * A função interpreta uma quantidade `count` de atributos de acordo com a JVM 8
 * e preenche o vetor fornecido com as estruturas `attribute`.
 * Nem todos os tipos de atributo são necessariamente implementados.
 *
 * @param cp Ponteiro para o constant pool.
 * @param count Quantidade de atributos a serem lidos.
 * @param fptr Ponteiro para o arquivo `.class`.
 * @param attr Vetor de atributos previamente alocado.
 * @see attribute
 */
void read_attributes(const cp_info *cp, u2 count, FILE *fptr, attribute *attr);

#endif // READER_H_
