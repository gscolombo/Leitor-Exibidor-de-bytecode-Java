#ifndef REFERENCES_METHODS_H
#define REFERENCES_METHODS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "uinteger.h"
#include "member.h"
#include "utils.h"
#include "wchar.h"

/**
 * Exibe os métodos de uma estrutura ClassFile.
 *
 * @param ClassFile* Ponteiro para a estrutura ClassFile que contém os métodos.
 */
void show_methods(const ClassFile *);

/**
 * Obtém o nome completo de um método.
 *
 * @param wchar_t* Buffer onde o nome completo será armazenado.
 * @param const `wchar_t*` Descritor do método.
 * @param u2 Flags de modificadores do método.
 * @param const `ClassFile*` Ponteiro para a estrutura `ClassFile` que contém o método.
 * @return `wchar_t*` Ponteiro para o buffer contendo o nome completo do método.
 */
wchar_t *get_full_method_name(wchar_t *, const wchar_t *, u2, const ClassFile *);

#endif