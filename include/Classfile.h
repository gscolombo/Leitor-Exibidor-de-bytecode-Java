#ifndef CLASSFILE_H_
#define CLASSFILE_H_

#include <stdlib.h>

#include "uinteger.h"
#include "member.h"
#include "constants.h"
#include "utils.h"

// Representa um arquivo .class, de acordo com a especificação JVM 8
typedef struct ClassFile
{
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info *constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    u2 *interfaces;
    u2 fields_count;
    member_info *fields;
    u2 methods_count;
    member_info *methods;
    u2 attributes_count;
    attribute *attributes;
} ClassFile;

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
void free_attributes(cp_info*, u2, attribute*);
#endif