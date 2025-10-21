#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H

#include <wchar.h>

#include "uinteger.h"
#include "attributes.h"

/**
 * @brief Estrutura geral de um atributo.
 */
typedef struct attribute
{
    /// @brief Índice do nome do atributo no pool de constantes.
    u2 attribute_name_index;
    /// @brief Tamanho em bytes do atributo.
    u4 attribute_length;
    /// @brief Informação do atributo.
    attribute_info info;
} attribute;

#endif