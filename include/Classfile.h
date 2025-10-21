#ifndef CLASSFILE_H_
#define CLASSFILE_H_

#include <stdlib.h>

#include "uinteger.h"
#include "member.h"
#include "constants.h"
#include "utils.h"

/**
 * @brief Estrutura que representa um arquivo .class Java, conforme especificação JVM 8.
 */
typedef struct ClassFile
{
    /// @brief Número mágico que identifica arquivos .class (0xCAFEBABE).
    u4 magic;
    /// @brief Versão menor do compilador que gerou o arquivo.
    u2 minor_version;
    /// @brief Versão maior do compilador que gerou o arquivo.
    u2 major_version;
    /// @brief Número de entradas no pool de constantes (índices de 1 a n-1).
    u2 constant_pool_count;
    /// @brief Pool de constantes do arquivo - array de estruturas cp_info.
    cp_info *constant_pool;
    /// @brief Máscara de bits com flags de acesso da classe (public, final, etc.).
    u2 access_flags;
    /// @brief Índice no pool de constantes para esta classe.
    u2 this_class;
    /// @brief Índice no pool de constantes para a superclasse (0 se for java.lang.Object).
    u2 super_class;
    /// @brief Número de interfaces implementadas por esta classe.
    u2 interfaces_count;
    /// @brief Array de índices no pool de constantes para cada interface implementada.
    u2 *interfaces;
    /// @brief Número de campos declarados na classe.
    u2 fields_count;
    /// @brief Array de campos da classe.
    member_info *fields;
    /// @brief Número de métodos declarados na classe.
    u2 methods_count;
    /// @brief Array de métodos da classe.
    member_info *methods;
    /// @brief Número de atributos da classe.
    u2 attributes_count;
    /// @brief Array de atributos da classe (SourceFile, InnerClasses, etc.).
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
void free_attributes(cp_info *, u2, attribute *);
#endif