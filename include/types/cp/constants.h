/**
 * @file constants.h
 * @brief Definições de constantes e estruturas relacionadas ao Constant Pool da JVM.
 *
 * Este arquivo contém:
 * - as tags identificadoras de cada tipo de constante do Constant Pool;
 * - a união `Constant`, que modela cada tipo de entrada possível;
 * - a estrutura `cp_info`, que representa uma entrada real na constant pool de um `.class`.
 *
 * Segue fielmente a especificação da JVM 8 (Java SE 8 Edition).
 */

#ifndef TYPES_CP_CONSTANTS_H
#define TYPES_CP_CONSTANTS_H

/* -------------------------------------------------------------------------- */
/* Tags de constantes conforme especificação JVM 8                            */
/* -------------------------------------------------------------------------- */

#define CONSTANT_Class                 7
#define CONSTANT_Fieldref              9
#define CONSTANT_Methodref             10
#define CONSTANT_InterfaceMethodref    11
#define CONSTANT_String                8
#define CONSTANT_Integer               3
#define CONSTANT_Float                 4
#define CONSTANT_Long                  5
#define CONSTANT_Double                6
#define CONSTANT_NameAndType           12
#define CONSTANT_UTF8                  1
#define CONSTANT_MethodHandle          15
#define CONSTANT_MethodType            16
#define CONSTANT_InvokeDynamic         18

#include "uinteger.h"

/**
 * @union Constant
 * @brief União contendo representações específicas para cada tipo do Constant Pool.
 *
 * Cada entrada de `cp_info` contém:
 * - uma tag que indica o tipo de constante;
 * - um valor interpretado por meio desta união.
 *
 * A interpretação correta depende da `tag`.
 */
typedef union Constant
{
    /* ------------------------------ CONSTANT_Class ------------------------------ */
    struct Class
    {
        /** Índice para CONSTANT_Utf8 contendo o nome interno da classe. */
        u2 name_index;
    } Class;

    /* --------------- CONSTANT_Fieldref, Methodref, InterfaceMethodref --------------- */
    struct Ref
    {
        /** Índice para CONSTANT_Class descrevendo a classe que define o membro. */
        u2 class_index;
        /** Índice para CONSTANT_NameAndType descrevendo nome + descritor do membro. */
        u2 name_and_type_index;
    } Ref;

    /* ------------------------------ CONSTANT_String ------------------------------ */
    struct String
    {
        /** Índice para CONSTANT_Utf8 contendo o valor textual da string. */
        u2 string_index;
    } String;

    /* ------------------ CONSTANT_Integer e CONSTANT_Float (32 bits) ------------------ */
    struct _4Bn
    {
        /** Representação em bytes (como lido do arquivo `.class`). */
        u4 bytes;

        /**
         * Interpretação dos bytes como int ou float.
         * A conversão final é feita após a leitura.
         */
        union
        {
            int   i; /**< inteiro de 32 bits */
            float f; /**< ponto flutuante de 32 bits */
        } number;

    } _4Bn;

    /* ------------------- CONSTANT_Long e CONSTANT_Double (64 bits) ------------------- */
    struct _8Bn
    {
        /** Parte alta do valor (primeiros 4 bytes). */
        u4 high_bytes;
        /** Parte baixa do valor (últimos 4 bytes). */
        u4 low_bytes;

        /**
         * Interpretação conjunta como long ou double.
         * A união evita conversões repetitivas.
         */
        union
        {
            long   l; /**< valor inteiro de 64 bits */
            double d; /**< valor double de 64 bits */
        } number;

    } _8Bn;

    /* --------------------------- CONSTANT_NameAndType --------------------------- */
    struct NameAndType
    {
        /** Índice para CONSTANT_Utf8 contendo o nome. */
        u2 name_index;

        /** Índice para CONSTANT_Utf8 contendo o descritor. */
        u2 descriptor_index;
    } NameAndType;

    /* ------------------------------ CONSTANT_UTF8 ------------------------------ */
    struct UTF8
    {
        /** Número de bytes do texto UTF-8 modificado. */
        u2 length;

        /** Bytes crus conforme armazenados no arquivo `.class`. */
        u1 *bytes;

        /** String decodificada já em UTF-8 padrão. */
        char *str;
    } UTF8;

    /* --------------------------- CONSTANT_MethodHandle --------------------------- */
    struct MethodHandle
    {
        /** Tipo do manipulador (1 a 9), conforme enum da JVM. */
        u1 reference_kind;

        /** Índice para uma entrada Ref. */
        u2 reference_index;
    } MethodHandle;

    /* ----------------------------- CONSTANT_MethodType ---------------------------- */
    struct MethodType
    {
        /** Índice para CONSTANT_Utf8 contendo o descritor do método. */
        u2 descriptor_index;
    } MethodType;

    /* -------------------------- CONSTANT_InvokeDynamic --------------------------- */
    struct InvokeDynamic
    {
        /** Índice para a tabela BootstrapMethods. */
        u2 bootstrap_method_attr_index;

        /** Índice para CONSTANT_NameAndType associado ao bootstrap. */
        u2 name_and_type_index;
    } InvokeDynamic;

} Constant;

/**
 * @struct cp_info
 * @brief Entrada do Constant Pool de um arquivo `.class`.
 *
 * Cada entrada tem:
 * - uma `tag` que indica o tipo;
 * - uma união `info` com os campos corretos para essa `tag`.
 *
 * O Constant Pool é um dos componentes mais importantes da JVM, usado para
 * resolução de símbolos, nomes, tipos, literais, etc.
 */
typedef struct cp_info
{
    /**
     * @brief Tag que indica qual variante do Constant Pool esta entrada representa.
     *
     * Valores possíveis (macros `CONSTANT_*`):
     * - CONSTANT_Class (7)
     * - CONSTANT_Fieldref (9)
     * - CONSTANT_Methodref (10)
     * - CONSTANT_InterfaceMethodref (11)
     * - CONSTANT_String (8)
     * - CONSTANT_Integer (3)
     * - CONSTANT_Float (4)
     * - CONSTANT_Long (5)
     * - CONSTANT_Double (6)
     * - CONSTANT_NameAndType (12)
     * - CONSTANT_UTF8 (1)
     * - CONSTANT_MethodHandle (15)
     * - CONSTANT_MethodType (16)
     * - CONSTANT_InvokeDynamic (18)
     */
    u1 tag;

    /**
     * @brief União que representa o conteúdo específico do tipo identificado por `tag`.
     *
     * A interpretação correta depende exclusivamente da `tag`.
     * @see Constant
     */
    Constant info;

} cp_info;

#endif /* TYPES_CP_CONSTANTS_H */
