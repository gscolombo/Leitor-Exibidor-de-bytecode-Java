#ifndef MEMBERS_METHODS_H
#define MEMBERS_METHODS_H

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
 * Exibe os métodos de uma estrutura ClassFile.
 *
 * @param cf Ponteiro para a estrutura ClassFile que contém os métodos.
 */
void show_methods(const ClassFile *);

#endif