#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "ref.h"
#include "tags.h"
#include "utils.h"
#include "wchar.h"

/**
 * @struct RefInfo
 * @brief Representa informações de referência para uma classe, incluindo o nome da classe, nome, tipo e índices associados.
 *
 * Esta estrutura armazena metadados sobre uma referência de classe, incluindo o nome, tipo e índices
 * associados em um pool de constantes.
 *
 * @var RefInfo::cls
 * Ponteiro para uma string de caracteres largos representando o nome da classe.
 *
 * @var RefInfo::ref_name
 * Ponteiro para uma string de caracteres largos representando o identificador da referência.
 *
 * @var RefInfo::ref_type
 * Ponteiro para uma string de caracteres largos representando a descrição do tipo da referência.
 *
 * @var RefInfo::cls_index
 * Inteiro sem sinal de 16 bits representando o índice da classe em um pool de constantes.
 *
 * @var RefInfo::name_and_type_index
 * Inteiro sem sinal de 16 bits representando o índice do nome e tipo em um pool de constantes.
 */
struct RefInfo
{
    wchar_t *cls, *ref_name, *ref_type;
    u2 cls_index, name_and_type_index;
};

/**
 * @brief Exibe uma constante do pool de constantes.
 *
 * Esta função recupera e exibe uma constante do pool de constantes
 * com base nos índices fornecidos.
 *
 * @param u2 O primeiro índice usado para localizar a constante.
 * @param u2 O número de constantes no pool de constantes.
 * @param cp_info Um ponteiro para o array do pool de constantes.
 */
void show_constant(u2, u2, cp_info *);

/**
 * @brief Exibe as informações de um ClassFile.
 * 
 * @param ClassFile* Ponteiro para a estrutura ClassFile que será exibida.
 */
void show_classfile(ClassFile *);

/**
 * @brief Obtém informações de referência a partir de uma estrutura `cp_info`.
 *
 * Esta função recebe um ponteiro para uma estrutura cp_info e um índice,
 * e retorna uma estrutura RefInfo contendo as informações de referência
 * associadas ao índice fornecido.
 *
 * @param cp_info Ponteiro constante para a estrutura cp_info que contém os dados.
 * @param index Índice do elemento na estrutura cp_info para o qual as informações
 *              de referência devem ser obtidas.
 * @return Uma estrutura `RefInfo` contendo as informações de referência associadas
 *         ao índice fornecido.
 */
struct RefInfo get_ref_info(const cp_info *, u2);

/**
 * @brief Exibe os flags de acesso da classe.
 *
 * Esta função recebe um ponteiro para uma estrutura do tipo `ClassFile`
 * e exibe os flags de acesso associados à classe. Os flags de acesso
 * indicam as permissões e propriedades da classe, como se ela é pública,
 * final, abstrata, etc.
 *
 * @param classFile Ponteiro para a estrutura ClassFile que contém as
 * informações da classe.
 */
void show_class_access_flags(const ClassFile *);

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