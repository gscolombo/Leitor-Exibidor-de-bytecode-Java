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
 * @brief Exibe as constantes do pool de constantes.
 *
 * Esta função recupera e exibe as constante do pool de constantes.
 *
 * @param u2 O número de constantes no pool de constantes.
 * @param cp_info Um ponteiro para o array do pool de constantes.
 */
void show_constants(u2, cp_info *);

#endif