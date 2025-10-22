#ifndef TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H
#define TYPES_ATTRIBUTES_ATTRIBUTE_INFO_H


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
    /**
     * @note
     * @parblock
     * Alguns atributos necessários para suporte a debugging e novos mecanismos da JVM 8, estão fora do escopo deste projeto 
     * e não serão implementados.\n
     * Contudo, suas estruturas básicas foram definidas para fins de documentação e facilitar uma eventual implementação futura.
     * 
     * Os atributos **não** considerados são:
     * - AnnotationDefault
     * - LineNumberTable
     * - LocalVariableTable
     * - LocalVariableTypeTable
     * - Deprecated
     * - SourceDebugExtension
     * - Signature
     * - RuntimeVisibleAnnotations
     * - RuntimeInvisibleAnnotations
     * - RuntimeVisibleParameterAnnotations
     * - RuntimeInvisibleParameterAnnotations
     * - EnclosingMethod
     * - BootstrapMethods
     * - MethodParameters
     * @endparblock
     */
    attribute_info info;
} attribute;

#endif