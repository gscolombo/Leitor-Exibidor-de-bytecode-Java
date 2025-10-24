#ifndef FREE_H_
#define FREE_H_

#include <stdlib.h>

#include "utils.h"
#include "Classfile.h"

/** @file
 * @brief Declaração de funções para liberação de memória alocada.
 */

/**
 * @brief Libera a memória alocada para uma estrutura `ClassFile`.
 *
 * @param cf Ponteiro para uma estrutura `ClassFile`
 */
void free_classfile(ClassFile *);

/**
 * @brief Libera a memória alocada para uma estrutura `attribute`.
 *
 * @param cp Ponteiro para uma estrutura `cp_info` contendo o pool de constantes.
 * @param count Número de atributos.
 * @param attr Ponteiro para uma estrutura `attribute`.
 */
void free_attributes(cp_info *, u2, attribute *);

#endif