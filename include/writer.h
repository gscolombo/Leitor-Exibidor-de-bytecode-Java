#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "member.h"
#include "utils.h"
#include "cp/writer.h"
#include "methods.h"
#include "fields.h"

/** @file
 *  @brief Declaração de função para exibição de informações do arquivo `.class`.
 */

/**
 * @brief Exibe as informações de um `ClassFile`.
 *
 * Essa função exibe as informações de um arquivo `.class` em um formato
 * similar à saída do comando `javap` com a _flag_ `-v`.
 * 
 * @param cf Ponteiro para a estrutura `ClassFile` que será exibida.
 */
void show_classfile(ClassFile *);

#endif