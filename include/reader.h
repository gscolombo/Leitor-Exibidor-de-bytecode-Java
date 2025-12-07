/**
 * @file reader.h
 * @brief Funções de leitura de arquivos .class e utilitários de leitura de inteiros.
 *
 * Contém funções para abrir e ler um arquivo .class e utilitários para ler
 * inteiros em big-endian (u1, u2, u4). Também define funções para leitura
 * de membros (fields/methods) e atributos a partir do arquivo.
 */

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

/**
 * @brief Macro que indica se a máquina é little-endian.
 *
 * Resolve para 1 se a arquitetura atual é little-endian, caso contrário 0.
 */
#define LE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)


/**
 * @brief Abre um arquivo .class para leitura binária.
 *
 * Realiza a abertura do arquivo indicado por `path` em modo de leitura binária.
 * Em caso de erro retorna NULL.
 *
 * @param path Caminho para o arquivo .class.
 * @return FILE* Ponteiro para o stream do arquivo aberto, ou NULL em erro.
 */
FILE *open_classfile(const char *path);


/**
 * @brief Lê um arquivo .class completo e retorna uma estrutura ClassFile preenchida.
 *
 * A função consome o stream `f` começando do início (ou onde estiver) e popula
 * uma nova instância de ClassFile alocada dinamicamente. O chamador é responsável
 * por chamar `free_classfile` para liberar a memória.
 *
 * @param f Ponteiro para o arquivo .class já aberto (FILE *).
 * @return ClassFile Estrutura com os dados lidos do arquivo .class.
 */
ClassFile read_classfile(FILE *f);


/**
 * @brief Lê um byte (8 bits) do arquivo.
 *
 * @param f Ponteiro para o arquivo.
 * @return u1 Valor lido (0..255). Em caso de falha de leitura o comportamento
 *         depende da implementação (normalmente retorna 0).
 */
u1 read_u1(FILE *f);


/**
 * @brief Lê dois bytes (16 bits, big-endian) do arquivo.
 *
 * @param f Ponteiro para o arquivo.
 * @return u2 Valor 16-bit lido em ordem big-endian.
 */
u2 read_u2(FILE *f);


/**
 * @brief Lê quatro bytes (32 bits, big-endian) do arquivo.
 *
 * @param f Ponteiro para o arquivo.
 * @return u4 Valor 32-bit lido em ordem big-endian.
 */
u4 read_u4(FILE *f);


/**
 * @brief Lê um member_info (field ou method) do stream e popula a estrutura fornecida.
 *
 * Esta função lê os campos que compõem um `member_info` (access_flags, name_index,
 * descriptor_index, attributes_count e attributes) usando o `cp` (constant pool)
 * para interpretar nomes/descritores se necessário.
 *
 * @param cp Ponteiro para o constant pool associado (cp_info *).
 * @param index Índice ou posição onde o member será colocado (uso interno do projeto).
 *              Se não usado, pode ser ignorado pela implementação.
 * @param out_member Ponteiro para a estrutura `member_info` previamente alocada
 *                   que será preenchida pela função.
 * @param f Ponteiro para o arquivo .class aberto do qual os dados serão lidos.
 */
void read_member(const cp_info *cp, u2 index, member_info *out_member, FILE *f);


/**
 * @brief Lê um array de atributos a partir do arquivo e preenche o array fornecido.
 *
 * A função lê `attributes_count` atributos do arquivo `f` e os armazena em `out_attributes`.
 * O `cp` (constant pool) é usado para interpretar nomes e conteúdos dos atributos.
 *
 * @param cp Ponteiro para o constant pool (cp_info *).
 * @param attributes_count Número de atributos a serem lidos.
 * @param f Ponteiro para o arquivo .class de onde os atributos serão lidos.
 * @param out_attributes Ponteiro para um buffer de atributos previamente alocado,
 *                       que será preenchido pela função.
 */
void read_attributes(const cp_info *cp, u2 attributes_count, FILE *f, attribute *out_attributes);

#endif /* READER_H_ */
