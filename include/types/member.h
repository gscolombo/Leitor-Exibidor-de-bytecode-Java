#ifndef TYPES_INFO_H
#define TYPES_INFO_H

#include "uinteger.h"
#include "attribute_info.h"

/** @file */

/**
 * @brief Mapeia um valor de flag para seu nome em string.
 */
typedef struct FlagMap
{
    /// @brief Valor numérico da flag.
    u2 flag;
    /// @brief Nome descritivo da flag em string.
    const char *name;
} FlagMap;

/**
 * @brief Estrutura comum para representar campos (fields) e métodos (methods) em arquivos .class.
 */
typedef struct member_info
{
    /// @brief Máscara de bits com flags de acesso (public, private, static, final, etc.).
    u2 access_flags;
    /// @brief Índice no pool de constantes para o nome do campo/método.
    u2 name_index;
    /// @brief Índice no pool de constantes para o descritor do campo/método.
    u2 descriptor_index;
    /// @brief Número de atributos associados a este campo/método.
    u2 attributes_count;
    /// @brief Array de atributos (Code, Exceptions, Signature, etc.) do campo/método.
    attribute *attributes;
} member_info;
#endif