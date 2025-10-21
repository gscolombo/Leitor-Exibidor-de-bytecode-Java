#ifndef TYPES_ATTRIBUTES_ATTRIBUTES_H
#define TYPES_ATTRIBUTES_ATTRIBUTES_H

#include "uinteger.h"
#include "inner_fields.h"

/**
 * Estruturas de atributos específicos,
 * de acordo com a especificação da JVM 8.
 */

typedef struct
{
    u2 constantvalue_index;
} _ConstantValue;

/**
 * Para lidar com a referência circular, os campos de atributos
 * do atributo Code são declarados na estrutura geral de um atributo.
 */
typedef struct
{
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1 *code;
    u2 exception_table_length;
    exception_table *exception_table;
} _Code;

typedef struct
{
    u2 number_of_entries;
    stack_map_frame *entries;
} _StackMapTable;

typedef struct
{
    u2 number_of_exceptions;
    u2 *exception_index_table;
} _Exceptions;

typedef struct
{
    u2 number_of_classes;
    classes *classes;
} _InnerClasses;

typedef struct
{
    u2 class_index;
    u2 method_index;
} _EnclosingMethod;

typedef struct
{
    u2 signature_index;
} _Signature;

typedef struct
{
    u2 sourcefile_index;
} _SourceFile;

typedef struct
{
    u1 *debug_extension;
} _SourceDebugExtension;

typedef struct
{
    u2 line_number_table_length;
    line_number_table *line_number_table;
} _LineNumberTable;

typedef struct
{
    u2 local_variable_table_length;
    local_variable_table *local_variable_table;
} _LocalVariableTable;

typedef struct
{
    u2 local_variable_type_table_length;
    local_variable_type_table *local_variable_type_table;
} _LocalVariableTypeTable;

typedef struct
{
    u2 num_annotations;
    annotation *annotations;
} _RuntimeAnnotation;

typedef struct
{
    u2 num_parameters;
    struct
    {
        u2 num_annotations;
        annotation *annotation;
    } *parameter_annotations;
} _RuntimeParemeterAnnotations;

typedef struct
{
    element_value default_value;
} _AnnotationDefault;

typedef struct
{
    u2 num_bootstrap_methods;
    bootstrap_method *bootstrap_methods;
} _BootstrapMethod;

typedef struct
{
    u1 parameters_count;
    parameter *parameters;
} _MethodParameters;

#endif