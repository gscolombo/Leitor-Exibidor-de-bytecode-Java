#ifndef TYPES_ATTRIBUTES_ATTRIBUTES_H
#define TYPES_ATTRIBUTES_ATTRIBUTES_H

#include "uinteger.h"
#include "inner_fields.h"

struct ConstantValue
{
    u2 constantvalue_index;
};

struct Code
{
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1 *code;
    u2 exception_table_length;
    exception_table *exception_table;
    u2 attributes_count;
    struct attribute *attributes;
};

struct StackMapTable
{
    u2 number_of_entries;
    stack_map_frame *entries;
};

struct Exceptions
{
    u2 number_of_exceptions;
    u2 *exception_index_table;
};

struct InnerClasses
{
    u2 number_of_classes;
    classes *classes;
};

struct EnclosingMethod
{
    u2 class_index;
    u2 method_index;
};

struct Signature
{
    u2 signature_index;
};

struct SourceFile
{
    u2 sourcefile_index;
};

struct SourceDebugExtension
{
    u1 *debug_extension;
};

struct LineNumberTable
{
    u2 line_number_table_length;
    line_number_table *line_number_table;
};

struct LocalVariableTable
{
    u2 local_variable_table_length;
    local_variable_table *local_variable_table;
};

struct LocalVariableTypeTable
{
    u2 local_variable_type_table_length;
    local_variable_type_table *local_variable_type_table;
};

struct RuntimeAnnotations
{
    u2 num_annotations;
    annotation *annotations;
};

struct RuntimeParameterAnnotations
{
    u2 num_parameters;
    struct
    {
        u2 num_annotations;
        annotation *annotation;
    } *parameter_annotations;
};

struct AnnotationDefault
{
    element_value default_value;
};

struct BootstrapMethods
{
    u2 num_bootstrap_methods;
    bootstrap_method *bootstrap_methods;
};

struct MethodParameters
{
    u1 parameters_count;
    parameter *parameters;
};

/// @brief União para informações específicas de cada tipo de atributo.
typedef union attribute_info
{
    /// @brief Informação de um atributo ConstantValue que representa o valor de uma expressão constante.
    struct ConstantValue ConstantValue;
    /// @brief Informação de um atributo Code contendo as instruções JVM e informações auxiliares de um método.
    struct Code Code;
    /// @brief Informação de um atributo StackMapTable para verificação de tipo em arquivos .class de versão maior ou igual a 50.
    struct StackMapTable StackMapTable;
    /// @brief Informação de um atributo Exceptions contendo possíveis exceções que são lançadas por um método.
    struct Exceptions Exceptions;
    /// @brief Informação de um atributo InnerClasses com classes aninhadas da respectiva classe (_i.e._, que não fazem parte de um pacote).
    struct InnerClasses InnerClasses;
    /// @brief Informação de um atributo EnclosingMethod representando o método que declara uma classe local ou anônima.
    struct EnclosingMethod EnclosingMethod;
    /// @brief Informação de um atributo Signature contendo declarações com tipos além daqueles definidos na JVM.
    struct Signature Signature;
    /// @brief Informação de um atributo SourceFile com o nome do arquivo `.java` associado ao _classfile_ atual.
    struct SourceFile SourceFile;
    /// @brief Informção de um atributo SourceDebugExtension com informações para _debugging_ extendidas.
    struct SourceDebugExtension SourceDebugExtension;
    /// @brief Informação de um atributo LineNumberTable associando linhas do _bytecode_ com as linhas do código-fonte Java para _debugging_.
    struct LineNumberTable LineNumberTable;
    /// @brief Informação de um atributo LocalVariableTable para _debuggers_ determinarem com o valor de uma dada variável local durante execução de um método.
    struct LocalVariableTable LocalVariableTable;
    /// @brief Informação de um atributo LocalVariableTable para _debuggers_ determinarem com o tipo de uma dada variável local durante execução de um método.
    struct LocalVariableTypeTable LocalVariableTypeTable;
    /// @brief Informação de um atributo RuntimeVisibleAnnotations ou RuntimeInvisibleAnnotations representando anotações de uma classe, campo ou método.
    struct RuntimeAnnotations RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;
    /// @brief Informação de um atributo RuntimeVisibleAnnotations ou RuntimeInvisibleAnnotations representando parâmetros de anotações de uma classe, campo ou método.
    struct RuntimeParameterAnnotations RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;
    /// @brief Informação de um atributo AnnotationDefault representando o valor padrão de uma anotação de uma classe, campo ou método.
    struct AnnotationDefault AnnotationDefault;
    /// @brief Informação de um atributo BootstrapMethods contendo especificações para um método _bootstrap_ referenciado por uma instrução _invokedynamic_.
    struct BootstrapMethods BootstrapMethods;
    /// @brief Informação de um atributo MethodParameters com informações sobre parâmetros formais de um método.
    struct MethodParameters MethodParameters;
} attribute_info;

#endif