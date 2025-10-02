#ifndef REFERENCES_METHODS_H
#define REFERENCES_METHODS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "ref.h"
#include "utils.h"
#include "wchar.h"

/**
 * Exibe os métodos de uma estrutura ClassFile.
 *
 * @param ClassFile* Ponteiro para a estrutura ClassFile que contém os métodos.
 */
void show_methods(const ClassFile *);

/**
 * Retorna os nomes das flags de acesso de um método.
 *
 * @param u2 Valor das flags de acesso do método.
 * @return `char*` String contendo os nomes das flags de acesso.
 */
char *get_method_access_flags_names(u2);

/**
 * Obtém o nome completo de um método.
 *
 * @param wchar_t* Buffer onde o nome completo será armazenado.
 * @param const wchar_t* Nome base do método.
 * @param u2 Índice do método na tabela de métodos.
 * @param const ClassFile* Ponteiro para a estrutura ClassFile que contém o método.
 * @return `wchar_t*` Ponteiro para o buffer contendo o nome completo do método.
 */
wchar_t *get_full_method_name(wchar_t *, const wchar_t *, u2, const ClassFile *);

/**
 * Analisa e interpreta os descritores de um método.
 *
 * @param wchar_t* Buffer onde os descritores analisados serão armazenados.
 * @param const wchar_t** Array de strings contendo os descritores.
 * @param size_t* Tamanho do array de descritores.
 */
void parse_method_descriptors(wchar_t *, const wchar_t **, size_t *);

#endif