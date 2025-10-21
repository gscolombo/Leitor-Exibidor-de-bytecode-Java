#ifndef TYPES_CP_CONSTANTS_H
#define TYPES_CP_CONSTANTS_H

/* Tags de constantes */
#define CONSTANT_Class 7
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_String 8
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_Long 5
#define CONSTANT_Double 6
#define CONSTANT_NameAndType 12
#define CONSTANT_UTF8 1
#define CONSTANT_MethodHandle 15
#define CONSTANT_MethodType 16
#define CONSTANT_InvokeDynamic 18

#include "uinteger.h"

/**
 * União dos diferentes
 * tipos de constantes apresentados
 * na especificação da JVM 8.
 */
typedef union Constant
{
    /// @brief Representa uma classe ou interface.
    struct Class
    {
        /// @brief Índice do _pool_ de constantes para uma constante UTF8 com o nome da classe/interface.
        u2 name_index;
    } Class;
    /// @brief Representa um campo, método ou método de interface de uma classe.
    struct Ref
    {
        /// @brief Índice do _pool_ de constantes para uma constante de classe (Class).
        u2 class_index;
        /// @brief Índice do _pool_ de constantes para uma constante (NameAndType) com informações específicas de um campo ou método de classe.
        u2 name_and_type_index;
    } Ref;
    /// @brief Representa um objeto constante do tipo `String`.
    struct String
    {
        /// @brief Índice do _pool_ de constantes para uma constante UTF8 com o valor da _string_.
        u2 string_index;
    } String;
    /// @brief Representa um valor numérico constante de 4 bytes (`int` ou `float`).
    struct _4Bn
    {
        /// @brief Valor em bytes da constante.
        u4 bytes;
        /// @brief União para armazenar o valor inteiro ou de ponto-flutuante constante.
        union
        {
            /// @brief Inteiro decodificado a partir dos bytes da constante.
            int i;
            /// @brief Número de ponto-flutuante decodificado a partir dos bytes da constante.
            float f;
        } number;
    } _4Bn;

    /// @brief Representa um valor numérico constante de 8 bytes (`long` ou `double`).
    struct _8Bn
    {
        /// @brief Primeiros 4 bytes (superiores) do valor da constante.
        u4 high_bytes;
        /// @brief Últimos 4 bytes (inferiores) do valor da constante.
        u4 low_bytes;
        /// @brief União para armazenar o valor inteiro ou de ponto-flutuante de 8 bytes constante.
        union
        {
            /// @brief Inteiro de 8 bytes decodificado a partir dos bytes da constante.
            long l;
            /// @brief Número de ponto-flutuante de precisão dupla decodificado a partir dos bytes da constante.
            double d;
        } number;
    } _8Bn;

    /// @brief Representa constantes com informações específicas de um campo ou método.
    struct NameAndType
    {
        /// @brief Índice do _pool_ de constantes para uma constante UTF8 com o nome do campo/método.
        u2 name_index;
        /// @brief Índice do _pool_ de constantes para uma constante UTF8 com o descritor do campo/método.
        u2 descriptor_index;
    } NameAndType;

    /// @brief Representa valores de _string_ constantes.
    struct UTF8
    {
        /// @brief Número de bytes do valor constante.
        u2 length;
        /// @brief Bytes da _string_ constante.
        u1 *bytes;
        /// @brief _String_ decodificada do formato UTF-8 modificado
        wchar_t *str;
    } UTF8;

    /// @brief Representa um manipulador de método.
    struct MethodHandle
    {
        /// @brief Valor constante de 1 a 9 que denota o tipo do manipulador.
        u1 reference_kind;
        /// @brief  Índice do _pool_ de constantes para uma constante Ref.
        u2 reference_index;
    } MethodHandle;

    /// @brief Representa o tipo de um método.
    struct MethodType
    {
        /// @brief Índice do _pool_ de constantes para uma constante UTF8 com o descritor do método.
        u2 descriptor_index;
    } MethodType;

    /// @brief Especifica o método _bootstrap_ utilizado por uma instrução _invokedynamic_.
    struct InvokeDynamic
    {
        /// @brief Índice da tabela de métodos _bootstrap_ (BootstrapMethod).
        u2 bootstrap_method_attr_index;
        /// @brief Índice do _pool_ de constantes para uma constante de campo, método ou interface de método (NameAndType).
        u2 name_and_type_index;
    } InvokeDynamic;
} Constant;

/**
 * Estrutura para representação de uma
 * constante no pool de constantes.
 */
typedef struct cp_info
{
    u1 tag;
    Constant info;
} cp_info;

#endif