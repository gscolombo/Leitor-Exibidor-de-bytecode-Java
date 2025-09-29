#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <wchar.h>

#include "stackmap.h"
#include "annotation.h"

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

static const struct
{
    attribute_name attr;
    const char *str;
} conversion[] = {
    {ConstantValue, "ConstantValue"},
    {Code, "Code"},
    {StackMapTable, "StackMapTable"}};

const attribute_name *convert_attr_name(const wchar_t *);

typedef struct attribute_info
{
    u2 attribute_name_index;
    u4 attribute_length;
    union
    {
        struct
        {
            u2 constantvalue_index;
        } ConstantValue;
        struct
        {
            u2 max_stack;
            u2 max_locals;
            u4 code_length;
            u1 *code;
            u2 exception_table_length;
            struct
            {
                u2 start_pc;
                u2 end_pc;
                u2 handler_pc;
                u2 catch_type;
            } *exception_table;
            u2 attributes_count;
            struct attribute_info *attributes;
        } Code;
        struct
        {
            u2 number_of_entries;
            stack_map_frame *entries;
        } StackMapTable;
        struct
        {
            u2 number_of_exceptions;
            u2 *exception_index_table;
        } Exceptions;
        struct
        {
            u2 number_of_classes;
            struct
            {
                u2 inner_class_info_index;
                u2 outer_class_info_index;
                u2 inner_name_index;
                u2 inner_class_access_flags;
            } *classes;
        } InnerClasses;
        struct
        {
            u2 class_index;
            u2 method_index;
        } EnclosingMethod;
        struct
        {
            u2 signature_index;
        } Signature;
        struct
        {
            u2 sourcefile_index;
        } SourceFile;
        struct
        {
            u1 *debug_extension;
        } SourceDebugExtension;
        struct
        {
            u2 line_number_table_length;
            struct
            {
                u2 start_pc;
                u2 line_number;
            } *line_number_table;
        } LineNumberTable;
        struct
        {
            u2 local_variable_table_length;
            struct
            {
                u2 start_pc;
                u2 length;
                u2 name_index;
                u2 descriptor_index;
                u2 index;
            } *local_variable_table;
        } LocalVariableTable;
        struct
        {
            u2 local_variable_type_table_length;
            struct
            {
                u2 start_pc;
                u2 length;
                u2 name_index;
                u2 signature_index;
                u2 index;
            } *local_variable_type_table;
        } LocalVariableTypeTable;
        struct
        {
            u2 num_annotations;
            annotation *annotations;
        } RuntimeVisibleAnnotations, RuntimeInvisibleAnnotations;
        struct
        {
            u2 num_parameters;
            struct
            {
                u2 num_annotations;
                annotation *annotation;
            } *parameter_annotations;
        } RuntimeVisibleParameterAnnotations, RuntimeInvisibleParameterAnnotations;
        struct
        {
            element_value default_value;
        } AnnotationDefault;
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
        struct
        {
            u1 parameters_count;
            struct
            {
                u2 name_index;
                u2 access_flags;
            } *parameters;
        } MethodParameters;
    } info;
} attribute;

#endif