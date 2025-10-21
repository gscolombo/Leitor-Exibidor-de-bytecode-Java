#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H

#include <wchar.h>

#include "uinteger.h"
#include "attributes.h"

/**
 * @brief Estrutura geral de um atributo em arquivos .class Java.
 */
typedef struct attribute
{
    /// @brief Índice no pool de constantes para o nome do atributo (CONSTANT_UTF8).
    u2 attribute_name_index;

    /// @brief Tamanho dos dados do atributo em bytes (excluindo os 6 bytes iniciais).
    u4 attribute_length;

    /// @brief Informações específicas do tipo de atributo.
    /// @see attribute_info para estruturas específicas por tipo.
    attribute_info info;
} attribute;

#endif