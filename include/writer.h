#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

#include "Classfile.h"
#include "member.h"
#include "utils.h"
#include "cp/writer.h"
#include "methods.h"
#include "fields.h"

/**
 * @brief Exibe as informações de um `ClassFile`.
 *
 * @param cf Ponteiro para a estrutura `ClassFile` que será exibida.
 */
void show_classfile(ClassFile *);

#endif