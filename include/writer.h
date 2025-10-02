#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "ref.h"
#include "utils.h"
#include "wchar.h"
#include "cp/writer.h"
#include "methods.h"

/**
 * @brief Exibe as informações de um ClassFile.
 *
 * @param ClassFile* Ponteiro para a estrutura ClassFile que será exibida.
 */
void show_classfile(ClassFile *);

/**
 * @brief Exibe os flags de acesso da classe.
 *
 * Esta função recebe um ponteiro para uma estrutura do tipo `ClassFile`
 * e exibe os flags de acesso associados à classe. Os flags de acesso
 * indicam as permissões e propriedades da classe, como se ela é pública,
 * final, abstrata, etc.
 *
 * @param classFile Ponteiro para a estrutura ClassFile que contém as
 * informações da classe.
 */
void show_class_access_flags(const ClassFile *);

void show_fields(const ClassFile *);

#endif