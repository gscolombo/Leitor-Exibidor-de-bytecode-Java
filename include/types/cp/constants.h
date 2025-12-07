/**
 * @file include/types/cp/constants.h
 * @brief Definições das constantes do Constant Pool e estruturas que representam cada tipo de entrada.
 *
 * O constant pool é uma das partes mais importantes do formato `.class`.
 * Ele contém:
 *  - nomes de classes
 *  - tipos
 *  - literais numéricos
 *  - strings
 *  - descritores
 *  - referências a fields, methods e interfaces
 *  - metadados adicionais
 *
 * Este arquivo define os valores das tags (`CONSTANT_*`) usados no constant pool
 * e as estruturas que armazenam cada variante de entrada, conforme a JVM Specification.
 */

#ifndef TYPES_CP_CONSTANTS_H
#define TYPES_CP_CONSTANTS_H

/* ============================================================
 *                     CONSTANT POOL TAGS
 * ============================================================*/

/** @brief Entrada CONSTANT_Class: referencia um name_index para uma string UTF8. */
#define CONSTANT_Class 7

/** @brief Entrada CONSTANT_Fieldref: referência a um field via class_index + name_and_type_index. */
#define CONSTANT_Fieldref 9

/** @brief Entrada CONSTANT_Methodref: referência a método de classe. */
#define CONSTANT_Methodref 10

/** @brief Entrada CONSTANT_InterfaceMethodref: referência a método de interface. */
#define CONSTANT_InterfaceMethodref 11

/** @brief Entrada CONSTANT_String: referência a um literal de string (índice para UTF8). */
#define CONSTANT_String 8

/** @brief Entrada CONSTANT_Integer: literal inteiro de 32 bits. */
#define CONSTANT_Integer 3

/** @brief Entrada CONSTANT_Float: literal float de 32 bits (IEEE754). */
#define CONSTANT_Float 4

/** @brief Entrada CONSTANT_Long: literal long de 64 bits (ocupa duas entradas). */
#define CONSTANT_Long 5

/** @brief Entrada CONSTANT_Double: literal double de 64 bits (ocupa duas entradas). */
#define CONSTANT_Double 6

/** @brief Entrada CONSTANT_NameAndType: par (nome, descritor). */
#define CONSTANT_NameAndType 12

/** @brief Entrada CONSTANT_UTF8: string UTF-8 (Modified UTF8). */
#define CONSTANT_UTF8 1

/** @brief Entrada CONSTANT_MethodHandle: metadado para invokedynamic. */
#define CONSTANT_MethodHandle 15

/** @brief Entrada CONSTANT_MethodType: descritor de método usado em invokedynamic. */
#define CONSTANT_MethodType 16

/** @brief Entrada CONSTANT_InvokeDynamic: referência dinâmica (lambda, bootstrap method). */
#define CONSTANT_InvokeDynamic 18


#include "uinteger.h"

/**
 * @union Constant
 * @brief Armazena o conteúdo real da entrada do constant pool dependendo da tag.
 *
 * Cada campo do union corresponde a um tipo de entrada permitido no constant pool:
 * - CONSTANT_Class
 * - CONSTANT_Fieldref / Methodref / InterfaceMethodref
 * - CONSTANT_String
 * - CONSTANT_Integer / Float
 * - CONSTANT_Long / Double
 * - CONSTANT_NameAndType
 * - CONSTANT_UTF8
 * - CONSTANT_MethodHandle
 * - CONSTANT_MethodType
 * - CONSTANT_InvokeDynamic
 *
 * Os campos seguem rigorosamente o formato especificado pela JVM.
 */
typedef union Constant
{
    /** @brief CONSTANT_Class: contém índice para o nome da classe (UTF8). */
    struct
    {
        u2 name_index;
    } Class;

    /** @brief Fieldref / Methodref / InterfaceMethodref. */
    struct
    {
        u2 class_index;          /**< Índice para CONSTANT_Class */
        u2 name_and_type_index;  /**< Índice para CONSTANT_NameAndType */
    } Ref;

    /** @brief CONSTANT_String: índice para uma entrada UTF8. */
    struct
    {
        u2 string_index;
    } String;

    /** @brief CONSTANT_Integer e CONSTANT_Float (4 bytes). */
    struct
    {
        u4 bytes; /**< Representação bruta */
        union
        {
            int i;     /**< CONSTANT_Integer */
            float f;   /**< CONSTANT_Float */
        } number;
    } _4Bn;

    /** @brief CONSTANT_Long e CONSTANT_Double (8 bytes combinados). */
    struct
    {
        u4 high_bytes; /**< parte alta dos 64 bits */
        u4 low_bytes;  /**< parte baixa dos 64 bits */
        union
        {
            long l;      /**< CONSTANT_Long */
            double d;    /**< CONSTANT_Double */
        } number;
    } _8Bn;

    /** @brief CONSTANT_NameAndType: (nome, descritor). */
    struct
    {
        u2 name_index;        /**< Índice de UTF8 para o nome */
        u2 descriptor_index;  /**< Índice de UTF8 para o descritor */
    } NameAndType;

    /** @brief CONSTANT_UTF8: string UTF-8 modificada (JVM). */
    struct
    {
        u2 length;     /**< Número de bytes */
        u1 *bytes;     /**< Bytes brutos (Modified UTF-8) */
        wchar_t *str;  /**< String decodificada (wide) */
    } UTF8;

    /** @brief CONSTANT_MethodHandle. */
    struct
    {
        u1 reference_kind;
        u2 reference_index;
    } MethodHandle;

    /** @brief CONSTANT_MethodType. */
    struct
    {
        u2 descriptor_index;
    } MethodType;

    /** @brief CONSTANT_InvokeDynamic. */
    struct
    {
        u2 bootstrap_method_attr_index;
        u2 name_and_type_index;
    } InvokeDynamic;

} Constant;

/**
 * @struct cp_info
 * @brief Estrutura que representa uma entrada do constant pool.
 *
 * Cada entrada contém:
 * - um campo `tag` indicando o tipo da constante (`CONSTANT_*`)
 * - um `union Constant` contendo os dados apropriados para a tag
 */
typedef struct cp_info
{
    u1 tag;        /**< Tag indicando o tipo da constante (CONSTANT_*). */
    Constant info; /**< Conteúdo da entrada específico para cada tag. */
} cp_info;

#endif /* TYPES_CP_CONSTANTS_H */
