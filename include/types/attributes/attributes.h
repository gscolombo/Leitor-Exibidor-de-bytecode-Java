/**
 * @file include/types/attributes/attributes.h
 * @brief Estruturas e enumerações que representam atributos da JVM presentes em fields, methods e classes.
 *
 * A JVM Specification define diversos atributos que podem aparecer em:
 *  - fields
 *  - methods
 *  - Code (dentro de um método)
 *  - ClassFile
 *
 * Cada atributo é identificado por um nome (UTF8) no constant pool e possui
 * um formato específico. Este arquivo define:
 *
 * 1. O enum `attribute_name`, que lista todos os atributos suportados.
 * 2. Estruturas auxiliares usadas por atributos específicos.
 * 3. A estrutura principal `attribute`, que contém o valor do atributo
 *    como uma união (`union`) de todos os tipos possíveis.
 *
 * O parser seleciona a variante correta do `union` com base no nome do atributo.
 */

#ifndef TYPES_ATTRIBUTES_ATTRIBUTES_H
#define TYPES_ATTRIBUTES_ATTRIBUTES_H

#include <wchar.h>

#include "uinteger.h"
#include "stackmap.h"
#include "annotation.h"

/* ============================================================
 *                    ENUM DE NOMES DE ATRIBUTOS
 * ============================================================*/

/**
 * @enum attribute_name
 * @brief Enumeração para identificação dos tipos de atributos suportados.
 *
 * O parser converte o nome UTF8 do atributo (obtido do constant pool)
 * para um dos valores desta enum via `convert_attr_name()`.
 *
 * Exemplos de atributos definidos pela JVM:
 *  - Code
 *  - LineNumberTable
 *  - LocalVariableTable
 *  - Exceptions
 *  - Signature
 *  - RuntimeVisibleAnnotations
 */
typedef enum attribute_name
{
    ConstantValue,
    Code,
    StackMapTable,
    Exceptions,
    InnerClasses,
    EnclosingMethod,
    Synthetic,
    Signature,
    SourceFile,
    SourceDebugExtension,
    LineNumberTable,
    LocalVariableTable,
    LocalVariableTypeTable,
    Deprecated,
    RuntimeVisibleAnnotations,
    RuntimeInvisibleAnnotations,
    RuntimeVisibleParameterAnnotations,
    RuntimeInvisibleParameterAnnotations,
    AnnotationDefault,
    BootstrapMethods,
    MethodParameters
} attribute_name;


/* ============================================================
 *               ESTRUTURAS AUXILIARES PARA ATRIBUTOS
 * ============================================================*/

/**
 * @struct line_number_table
 * @brief Mapeamento entre bytecode index (start_pc) e número da linha no código fonte.
 *
 * Pertence ao atributo LineNumberTable.
 */
typedef struct line_number_table
{
    u2 start_pc;     /**< índice do bytecode onde a linha começa */
    u2 line_number;  /**< número da linha no código fonte */
} line_number_table;

/**
 * @struct exception_table
 * @brief Entrada da tabela de exceções do atributo Code.
 *
 * Cada entrada define um bloco try/catch.
 */
typedef struct exception_table
{
    u2 start_pc;    /**< início do bloco protegido */
    u2 end_pc;      /**< fim do bloco protegido */
    u2 handler_pc;  /**< endereço do handler */
    u2 catch_type;  /**< índice para CONSTANT_Class da exceção (ou 0 para catch-all) */
} exception_table;

/**
 * @struct classes
 * @brief Representa uma entrada do atributo InnerClasses.
 */
typedef struct classes
{
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
} classes;

/**
 * @struct local_variable_table
 * @brief Entrada do atributo LocalVariableTable dentro de Code.
 */
typedef struct local_variable_table
{
    u2 start_pc;
    u2 length;
    u2 name_index;        /**< índice UTF8 para nome da variável */
    u2 descriptor_index;  /**< tipo da variável */
    u2 index;             /**< slot do local variable frame */
} local_variable_table;

/**
 * @struct local_variable_type_table
 * @brief Entrada do atributo LocalVariableTypeTable.
 */
typedef struct local_variable_type_table
{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 signature_index;   /**< assinatura genérica (ex: List<String>) */
    u2 index;
} local_variable_type_table;

/**
 * @struct parameter
 * @brief Representa um parâmetro do atributo MethodParameters.
 */
typedef struct parameter
{
    u2 name_index;
    u2 access_flags;
} parameter;


/* ============================================================
 *                        ATRIBUTO PRINCIPAL
 * ============================================================*/

/**
 * @struct attribute
 * @brief Representa um atributo genérico da JVM.
 *
 * Cada atributo começa com:
 *  - attribute_name_index (u2)
 *  - attribute_length (u4)
 *
 * Depois, o formato interno depende do nome do atributo,
 * armazenado aqui no `union info`.
 */
typedef struct attribute
{
    u2 attribute_name_index; /**< Índice no constant pool para o nome do atributo. */
    u4 attribute_length;     /**< Tamanho dos dados (não inclui os 6 bytes iniciais). */

    /**
     * @union info
     * @brief Conteúdo do atributo, variando conforme seu tipo.
     */
    union
    {
        /** ConstantValue: usado por fields com valor fixo. */
        struct
        {
            u2 constantvalue_index;
        } ConstantValue;

        /**
         * @brief Atributo Code: contém o bytecode, exceções e sub-atributos.
         */
        struct
        {
            u2 max_stack;
            u2 max_locals;
            u4 code_length;
            u1 *code;                   /**< bytecode */
            u2 exception_table_length;
            exception_table *exception_table;
            u2 attributes_count;        /**< sub-attributes */
            struct attribute *attributes;
        } Code;

        /** StackMapTable: usado para verificação de tipo no bytecode. */
        struct
        {
            u2 number_of_entries;
            stack_map_frame *entries;
        } StackMapTable;

        /** Lista de exceções declaradas em um método. */
        struct
        {
            u2 number_of_exceptions;
            u2 *exception_index_table;
        } Exceptions;

        /** Atributo InnerClasses. */
        struct
        {
            u2 number_of_classes;
            classes *classes;
        } InnerClasses;

        /** EnclosingMethod: indica método e classe externa. */
        struct
        {
            u2 class_index;
            u2 method_index;
        } EnclosingMethod;

        /** Atributo Signature: tipo genérico completo (para generics). */
        struct
        {
            u2 signature_index;
        } Signature;

        /** Indica o arquivo de origem (.java). */
        struct
        {
            u2 sourcefile_index;
        } SourceFile;

        /** Debug extension: dados arbitrários. */
        struct
        {
            u1 *debug_extension;
        } SourceDebugExtension;

        /** Tabela de linhas (Code → linhas no fonte). */
        struct
        {
            u2 line_number_table_length;
            line_number_table *line_number_table;
        } LineNumberTable;

        /** Tabela de variáveis locais. */
        struct
        {
            u2 local_variable_table_length;
            local_variable_table *local_variable_table;
        } LocalVariableTable;

        /** Tabela de tipos de variáveis locais. */
        struct
        {
            u2 local_variable_type_table_length;
            local_variable_type_table *local_variable_type_table;
        } LocalVariableTypeTable;

        /** Anotações visíveis/invisíveis. */
        struct
        {
            u2 num_annotations;
            annotation *annotations;
        } RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;

        /** Anotações por parâmetro (visível/invisível). */
        struct
        {
            u2 num_parameters;
            struct
            {
                u2 num_annotations;
                annotation *annotation;
            } *parameter_annotations;
        } RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;

        /** Valor padrão de uma anotação. */
        struct
        {
            element_value default_value;
        } AnnotationDefault;

        /** BootstrapMethods: usado em invokedynamic/lambdas. */
        struct
        {
            u2 num_bootstrap_methods;
            struct
            {
                u2 bootstrap_method_ref;
                u2 num_bootstrap_arguments;
                u2 *bootstrap_arguments;
            } *bootstrap_methods;
        } BootstrapMethods;

        /** Parâmetros do método (nome e flags). */
        struct
        {
            u1 parameters_count;
            parameter *parameters;
        } MethodParameters;

    } info;

} attribute;

#endif /* TYPES_ATTRIBUTES_ATTRIBUTES_H */
