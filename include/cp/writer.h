#ifndef CP_WRITER_H
#define CP_WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "uinteger.h"
#include "utils.h"
#include "wchar.h"

/** @file
 *  @brief Funções para exibição formatada das constantes no _pool_ de constantes.
 */

/**
 * @brief Exibe as constantes do pool de constantes.
 *
 * Esta função recupera e exibe as constante do pool de constantes.
 * 
 * O formato de exibição é similar ao retornado pela ferramenta `javap` com _flag_ `-v`.
 * Para cada constante no _pool_, é realizada a leitura da _tag_ e, com base no tipo da constante, as informações
 * específicas são recuperadas, formatadas e apresentadas no terminal.
 *
 * @param count O número de constantes no pool de constantes.
 * @param _cp Um ponteiro para o array do pool de constantes.
 */
void show_constants(u2, cp_info *);

#endif