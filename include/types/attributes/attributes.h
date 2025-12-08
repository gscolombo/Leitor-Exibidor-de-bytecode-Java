/**
 * @file attributes.h
 * @brief Representações das diferentes estruturas de atributos encontradas em arquivos `.class` (JVM 8).
 *
 * Esta unidade define a união `attribute_info` que contém todas as possíveis
 * representações específicas de atributos (Code, ConstantValue, Exceptions, InnerClasses, etc.)
 * conforme a especificação da JVM 8. Cada membro da união corresponde aos dados
 * associados a um tipo de atributo particular.
 *
 * Observações:
 * - Nem todos os campos complexos (por exemplo, estruturas de annotation) estão
 *   totalmente implementados — muitos aparecem como placeholders para sinalizar
 *   onde a implementação adicional deveria ocorrer.
 * - Todas as alocações feitas durante o parsing devem ser liberadas por rotinas
 *   adequadas (ex.: `free_attributes()`).
 */

#ifndef TYPES_ATTRIBUTES_ATTRIBUTES_H
#define TYPES_ATTRIBUTES_ATTRIBUTES_H

#include "uinteger.h"

/// @brief União contendo as informações específicas para cada tipo de atributo do classfile.
typedef union attribute_info
{
    /* --------------------------------------------------------------------- */
    /* ConstantValue                                                         */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo ConstantValue.
     *
     * Representa o valor constante associado a um campo estático final,
     * apontado por um índice no constant pool.
     */
    struct ConstantValue
    {
        /** Índice no constant pool apontando para o valor constante. */
        u2 constantvalue_index;
    } ConstantValue;

    /* --------------------------------------------------------------------- */
    /* Code                                                                  */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo Code associado a métodos contendo bytecode.
     *
     * Contém o bytecode do método, tabelas de exceção e atributos aninhados (por ex. LineNumberTable).
     */
    struct Code
    {
        /** Máximo de níveis na pilha de operandos durante execução (max_stack). */
        u2 max_stack;

        /** Número máximo de variáveis locais (max_locals). */
        u2 max_locals;

        /** Comprimento do array de bytecode (em bytes). */
        u4 code_length;

        /** Ponteiro para os bytes do bytecode (array de u1). */
        u1 *code;

        /** Número de entradas na tabela de exceções. */
        u2 exception_table_length;

        /**
         * @brief Tabela de exceções (cada entrada define um handler).
         *
         * Cada item contém:
         * - start_pc, end_pc: intervalo coberto pelo handler;
         * - handler_pc: início do handler;
         * - catch_type: índice no constant pool do tipo capturado (0 = catch-all).
         */
        struct exception_table
        {
            u2 start_pc;
            u2 end_pc;
            u2 handler_pc;
            u2 catch_type;
        } *exception_table;

        /** Número de atributos aninhados dentro do atributo Code. */
        u2 attributes_count;

        /**
         * @brief Array de atributos aninhados (LineNumberTable, LocalVariableTable, etc.).
         *
         * Este campo referencia um vetor de `attribute` ou `attribute_info` conforme a implementação do projeto.
         */
        struct attribute *attributes;
    } Code;

    /* --------------------------------------------------------------------- */
    /* StackMapTable                                                         */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo StackMapTable usado pelo verificador de tipos.
     *
     * Implementação parcial — as estruturas internas de stack_map_frame permanecem como placeholder.
     */
    struct StackMapTable
    {
        /** Número de entradas na tabela de frames. */
        u2 number_of_entries;

        /**
         * @brief Ponteiro para as entradas da StackMapTable.
         *
         * Implementação detalhada de stack_map_frame não fornecida aqui.
         */
        struct stack_map_frame
        {
            /* Placeholder para representação de stack map frames. */
        } *entries;
    } StackMapTable;

    /* --------------------------------------------------------------------- */
    /* Exceptions                                                             */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo Exceptions listando exceções lançadas por um método.
     */
    struct Exceptions
    {
        /** Número de exceções listadas. */
        u2 number_of_exceptions;

        /** Array de índices no constant pool apontando para classes Throwable. */
        u2 *exception_index_table;
    } Exceptions;

    /* --------------------------------------------------------------------- */
    /* InnerClasses                                                           */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo InnerClasses descrevendo classes internas.
     */
    struct InnerClasses
    {
        /** Número de classes internas descritas. */
        u2 number_of_classes;

        /**
         * @brief Array com informações sobre cada classe interna.
         *
         * Para cada entrada:
         * - inner_class_info_index: índice para a classe interna;
         * - outer_class_info_index: índice da classe externa (0 se anônima);
         * - inner_name_index: índice para nome simples (0 se anônima);
         * - inner_class_access_flags: flags de acesso da classe interna.
         */
        struct classes
        {
            u2 inner_class_info_index;
            u2 outer_class_info_index;
            u2 inner_name_index;
            u2 inner_class_access_flags;
        } *classes;
    } InnerClasses;

    /* --------------------------------------------------------------------- */
    /* EnclosingMethod                                                        */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo EnclosingMethod usado para classes locais/anônimas.
     */
    struct EnclosingMethod
    {
        /** Índice no constant pool para a classe que contém a declaração. */
        u2 class_index;

        /**
         * @brief Índice no constant pool para o método que contém a declaração.
         *
         * Valor 0 indica que a classe não é local a um método.
         */
        u2 method_index;
    } EnclosingMethod;

    /* --------------------------------------------------------------------- */
    /* Signature                                                              */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo Signature que contém tipos genéricos/avançados.
     */
    struct Signature
    {
        /** Índice no constant pool para a string UTF-8 com a assinatura. */
        u2 signature_index;
    } Signature;

    /* --------------------------------------------------------------------- */
    /* SourceFile / SourceDebugExtension                                      */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo SourceFile indicando o nome do arquivo fonte.
     */
    struct SourceFile
    {
        u2 sourcefile_index; /**< Índice no constant pool para o nome do arquivo .java */
    } SourceFile;

    /**
     * @brief Informação do atributo SourceDebugExtension com dados de debug adicionais.
     */
    struct SourceDebugExtension
    {
        u1 *debug_extension; /**< Bytes contendo informações de debug estendidas */
    } SourceDebugExtension;

    /* --------------------------------------------------------------------- */
    /* LineNumberTable                                                        */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo LineNumberTable para mapeamento bytecode → número de linha.
     */
    struct LineNumberTable
    {
        u2 line_number_table_length;

        /**
         * @brief Array de entradas (start_pc, line_number).
         *
         * Cada entrada indica que a instrução em start_pc corresponde àquela linha no fonte.
         */
        struct line_number_table
        {
            u2 start_pc;
            u2 line_number;
        } *line_number_table;
    } LineNumberTable;

    /* --------------------------------------------------------------------- */
    /* LocalVariableTable / LocalVariableTypeTable                            */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo LocalVariableTable (debug).
     */
    struct LocalVariableTable
    {
        u2 local_variable_table_length;

        struct local_variable_table
        {
            u2 start_pc;
            u2 length;
            u2 name_index;
            u2 descriptor_index;
            u2 index;
        } *local_variable_table;
    } LocalVariableTable;

    /**
     * @brief Informação do atributo LocalVariableTypeTable (debug para genéricos).
     */
    struct LocalVariableTypeTable
    {
        u2 local_variable_type_table_length;

        struct local_variable_type_table
        {
            u2 start_pc;
            u2 length;
            u2 name_index;
            u2 signature_index;
            u2 index;
        } *local_variable_type_table;
    } LocalVariableTypeTable;

    /* --------------------------------------------------------------------- */
    /* Runtime Annotations / Parameter Annotations / AnnotationDefault         */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação de atributos de anotações (visíveis em tempo de execução ou não).
     *
     * Estruturas internas de annotation são placeholders; implementar conforme necessário.
     */
    struct RuntimeAnnotations
    {
        u2 num_annotations;
        struct annotation
        {
            /* Placeholder para campos de annotation conforme JVM spec */
        } *annotations;
    } RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;

    /**
     * @brief Informação de anotações por parâmetro (visíveis/invisíveis).
     */
    struct RuntimeParameterAnnotations
    {
        u2 num_parameters;
        struct
        {
            u2 num_annotations;
            struct annotation *annotation; /* Ponteiro para array de annotations */
        } *parameter_annotations;
    } RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;

    /**
     * @brief Informação do atributo AnnotationDefault (valor padrão de uma annotation element).
     */
    struct AnnotationDefault
    {
        struct element_value
        {
            /* Placeholder para estrutura element_value conforme a especificação */
        } default_value;
    } AnnotationDefault;

    /* --------------------------------------------------------------------- */
    /* BootstrapMethods / MethodParameters                                    */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Informação do atributo BootstrapMethods (usado por invokedynamic).
     */
    struct BootstrapMethods
    {
        u2 num_bootstrap_methods;

        struct bootstrap_method
        {
            u2 bootstrap_method_ref;      /**< Índice para um MethodHandle */
            u2 num_bootstrap_arguments;   /**< Quantidade de argumentos bootstrap */
            u2 *bootstrap_arguments;      /**< Índices no constant pool para argumentos */
        } *bootstrap_methods;
    } BootstrapMethods;

    /**
     * @brief Informação do atributo MethodParameters contendo metadados de parâmetros.
     */
    struct MethodParameters
    {
        u1 parameters_count;

        struct parameter
        {
            u2 name_index;   /**< Índice no constant pool para o nome do parâmetro (0 se ausente) */
            u2 access_flags; /**< Flags do parâmetro (final, synthetic, mandated) */
        } *parameters;
    } MethodParameters;

} attribute_info;

#endif /* TYPES_ATTRIBUTES_ATTRIBUTES_H */
