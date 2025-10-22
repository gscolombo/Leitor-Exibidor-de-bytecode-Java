#ifndef MEMBERS_FIELDS_H
#define MEMBERS_FIELDS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "uinteger.h"
#include "member.h"
#include "utils.h"
#include "wchar.h"

/** @file */

/**
 * Exibe os campos (fields) de uma estrutura ClassFile.
 *
 * @param cf Ponteiro para a estrutura ClassFile que contém os métodos.
 */
void show_fields(const ClassFile *);

#endif