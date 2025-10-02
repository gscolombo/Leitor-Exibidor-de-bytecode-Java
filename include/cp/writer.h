#ifndef CP_WRITER_H
#define CP_WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
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

#endif