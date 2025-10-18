#ifndef CP_WRITER_H
#define CP_WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "uinteger.h"
#include "utils.h"
#include "wchar.h"

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