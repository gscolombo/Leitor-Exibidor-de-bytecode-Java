#ifndef TYPES_ATTRIBUTES_INNER_FIELDS_H
#define TYPES_ATTRIBUTES_INNER_FIELDS_H

#include "uinteger.h"

/**
 * Estruturas para campos internos de atributos específicos,
 * de acordo com a especificação da JVM 8.
 */

typedef struct
{
    u2 start_pc;
    u2 line_number;
} line_number_table;

typedef struct
{
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
} exception_table;

typedef struct
{
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
} classes;

typedef struct
{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
} local_variable_table;

typedef struct
{
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 signature_index;
    u2 index;
} local_variable_type_table;

typedef struct
{
    u2 name_index;
    u2 access_flags;
} parameter;

typedef struct
{

} annotation;

typedef struct
{

} element_value;

typedef struct
{
    u2 num_annotations;
    annotation *annotation;
} *parameter_annotations;

typedef union stack_map_frame
{
} stack_map_frame;

typedef struct
{
    u2 bootstrap_method_ref;
    u2 num_bootstrap_arguments;
    u2 *bootstrap_arguments;
} bootstrap_method;

#endif