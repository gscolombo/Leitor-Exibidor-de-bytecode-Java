#ifndef TYPES_ATTRIBUTES_ATTRIBUTES_H
#define TYPES_ATTRIBUTES_ATTRIBUTES_H

#include "uinteger.h"

/// @brief União para informações específicas de cada tipo de atributo.
typedef union attribute_info
{
    /// @brief Informação de um atributo ConstantValue que representa o valor de uma expressão constante.
    struct ConstantValue
    {
        /// @brief Índice na constant pool que contém o valor constante (deve ser um índice válido para um campo Long, Double, Float, Integer, String)
        u2 constantvalue_index;
    } ConstantValue;

    /// @brief Informação de um atributo Code contendo as instruções JVM e informações auxiliares de um método.
    struct Code
    {
        /// @brief Número máximo de valores na pilha de operandos durante a execução deste método
        u2 max_stack;
        /// @brief Número de variáveis locais no array de variáveis locais alocado para este método
        u2 max_locals;
        /// @brief Número de bytes no array de código para este método
        u4 code_length;
        /// @brief Array de bytes que contém o bytecode real do método
        u1 *code;
        /// @brief Número de entradas na tabela de exceções
        u2 exception_table_length;
        /// @brief Tabela de exceções que define os handlers de exceção
        struct exception_table
        {
            /// @brief Índice do início do range de código coberto pelo handler (inclusive)
            u2 start_pc;
            /// @brief Índice do fim do range de código coberto pelo handler (exclusive)
            u2 end_pc;
            /// @brief Índice do início do código do handler
            u2 handler_pc;
            /// @brief Índice na constant pool do tipo de exceção capturado (0 para qualquer exceção)
            u2 catch_type;
        } *exception_table;
        /// @brief Número de atributos adicionais associados ao código
        u2 attributes_count;
        /// @brief Array de atributos adicionais do código (como LineNumberTable, LocalVariableTable, etc.)
        struct attribute *attributes;
    } Code;

    /// @brief Informação de um atributo StackMapTable para verificação de tipo em arquivos .class de versão maior ou igual a 50.
    struct StackMapTable
    {
        /// @brief Número de entradas na tabela de stack map frames
        u2 number_of_entries;
        /// @brief Array de stack map frames que definem o estado da pilha em pontos específicos do código
        union stack_map_frame
        {
            // Stack map frame types would be defined here
        } *entries;
    } StackMapTable;

    /// @brief Informação de um atributo Exceptions contendo possíveis exceções que são lançadas por um método.
    struct Exceptions
    {
        /// @brief Número de exceções na tabela
        u2 number_of_exceptions;
        /// @brief Array de índices na constant pool para classes de exceção (devem ser classes Throwable)
        u2 *exception_index_table;
    } Exceptions;

    /// @brief Informação de um atributo InnerClasses com classes aninhadas da respectiva classe (_i.e._, que não fazem parte de um pacote).
    struct InnerClasses
    {
        /// @brief Número de classes internas definidas
        u2 number_of_classes;
        /// @brief Array de informações sobre classes internas
        struct classes
        {
            /// @brief Índice na constant pool para a classe interna
            u2 inner_class_info_index;
            /// @brief Índice na constant pool para a classe externa (0 se a classe for anônima)
            u2 outer_class_info_index;
            /// @brief Índice na constant pool para o nome simples da classe interna (0 se a classe for anônima)
            u2 inner_name_index;
            /// @brief Máscara de bits com flags de acesso da classe interna (public, private, protected, static, final, etc.)
            u2 inner_class_access_flags;
        } *classes;
    } InnerClasses;

    /// @brief Informação de um atributo EnclosingMethod representando o método que declara uma classe local ou anônima.
    struct EnclosingMethod
    {
        /// @brief Índice na constant pool para a classe que contém a declaração da classe atual
        u2 class_index;
        /// @brief Índice na constant pool para o método que contém a declaração (0 se a classe não for local a um método)
        u2 method_index;
    } EnclosingMethod;

    /// @brief Informação de um atributo Signature contendo declarações com tipos além daqueles definidos na JVM.
    struct Signature
    {
        /// @brief Índice na constant pool para uma string UTF-8 contendo a assinatura do tipo
        u2 signature_index;
    } Signature;

    /// @brief Informação de um atributo SourceFile com o nome do arquivo `.java` associado ao _classfile_ atual.
    struct SourceFile
    {
        /// @brief Índice na constant pool para uma string UTF-8 contendo o nome do arquivo fonte
        u2 sourcefile_index;
    } SourceFile;

    /// @brief Informção de um atributo SourceDebugExtension com informações para _debugging_ extendidas.
    struct SourceDebugExtension
    {
        /// @brief Array de bytes contendo informações extendidas de debug em formato indefinido
        u1 *debug_extension;
    } SourceDebugExtension;

    /// @brief Informação de um atributo LineNumberTable associando linhas do _bytecode_ com as linhas do código-fonte Java para _debugging_.
    struct LineNumberTable
    {
        /// @brief Número de entradas na tabela de linhas
        u2 line_number_table_length;
        /// @brief Array de mapeamentos entre bytecode e linhas do fonte
        struct line_number_table
        {
            /// @brief Índice no array de código onde uma nova linha no fonte começa
            u2 start_pc;
            /// @brief Número da linha correspondente no arquivo fonte
            u2 line_number;
        } *line_number_table;
    } LineNumberTable;

    /// @brief Informação de um atributo LocalVariableTable para _debuggers_ determinarem com o valor de uma dada variável local durante execução de um método.
    struct LocalVariableTable
    {
        /// @brief Número de entradas na tabela de variáveis locais
        u2 local_variable_table_length;
        /// @brief Array de informações sobre variáveis locais
        struct local_variable_table
        {
            /// @brief Índice no array de código onde a variável começa a ser válida
            u2 start_pc;
            /// @brief Comprimento do range onde a variável é válida (em bytes de código)
            u2 length;
            /// @brief Índice na constant pool para o nome da variável local
            u2 name_index;
            /// @brief Índice na constant pool para o descritor de campo da variável local
            u2 descriptor_index;
            /// @brief Índice no array de variáveis locais onde esta variável está armazenada
            u2 index;
        } *local_variable_table;
    } LocalVariableTable;

    /// @brief Informação de um atributo LocalVariableTable para _debuggers_ determinarem com o tipo de uma dada variável local durante execução de um método.
    struct LocalVariableTypeTable
    {
        /// @brief Número de entradas na tabela de tipos de variáveis locais
        u2 local_variable_type_table_length;
        /// @brief Array de informações sobre tipos genéricos de variáveis locais
        struct local_variable_type_table
        {
            /// @brief Índice no array de código onde a variável começa a ser válida
            u2 start_pc;
            /// @brief Comprimento do range onde a variável é válida (em bytes de código)
            u2 length;
            /// @brief Índice na constant pool para o nome da variável local
            u2 name_index;
            /// @brief Índice na constant pool para a assinatura de campo da variável local
            u2 signature_index;
            /// @brief Índice no array de variáveis locais onde esta variável está armazenada
            u2 index;
        } *local_variable_type_table;
    } LocalVariableTypeTable;

    /// @brief Informação de um atributo RuntimeVisibleAnnotations ou RuntimeInvisibleAnnotations representando anotações de uma classe, campo ou método.
    struct RuntimeAnnotations
    {
        /// @brief Número de anotações definidas
        u2 num_annotations;
        /// @brief Array de estruturas de anotações
        struct annotation
        {
            // Annotation structure would be defined here
        } *annotations;
    } RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;

    /// @brief Informação de um atributo RuntimeVisibleAnnotations ou RuntimeInvisibleAnnotations representando parâmetros de anotações de uma classe, campo ou método.
    struct RuntimeParameterAnnotations
    {
        /// @brief Número de parâmetros que possuem anotações
        u2 num_parameters;
        /// @brief Array de anotações por parâmetro
        struct
        {
            /// @brief Número de anotações para este parâmetro
            u2 num_annotations;
            /// @brief Array de anotações aplicadas a este parâmetro
            struct annotation *annotation;
        } *parameter_annotations;
    } RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;

    /// @brief Informação de um atributo AnnotationDefault representando o valor padrão de uma anotação de uma classe, campo ou método.
    struct AnnotationDefault
    {
        /// @brief Valor padrão da anotação quando não explicitamente especificado
        struct element_value
        {
            // Element value structure would be defined here
        } default_value;
    } AnnotationDefault;

    /// @brief Informação de um atributo BootstrapMethods contendo especificações para um método _bootstrap_ referenciado por uma instrução _invokedynamic_.
    struct BootstrapMethods
    {
        /// @brief Número de métodos bootstrap definidos
        u2 num_bootstrap_methods;
        /// @brief Array de métodos bootstrap
        struct bootstrap_method
        {
            /// @brief Índice na constant pool para um método handle válido
            u2 bootstrap_method_ref;
            /// @brief Número de argumentos bootstrap
            u2 num_bootstrap_arguments;
            /// @brief Array de índices na constant pool para argumentos bootstrap (devem ser constantes estáticas)
            u2 *bootstrap_arguments;
        } *bootstrap_methods;
    } BootstrapMethods;

    /// @brief Informação de um atributo MethodParameters com informações sobre parâmetros formais de um método.
    struct MethodParameters
    {
        /// @brief Número de parâmetros no método
        u1 parameters_count;
        /// @brief Array de informações sobre cada parâmetro
        struct parameter
        {
            /// @brief Índice na constant pool para o nome do parâmetro (0 se o parâmetro não tem nome)
            u2 name_index;
            /// @brief Flags de acesso do parâmetro (final, synthetic, mandated)
            u2 access_flags;
        } *parameters;
    } MethodParameters;
} attribute_info;

#endif