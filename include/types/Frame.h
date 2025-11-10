#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include "uinteger.h"
#include "constants.h"
#include "Classfile.h"

typedef union
{
    int8_t byte;
    int16_t _short;
    int32_t _int;
    int64_t _long;
    u2 _char;
    float _float;
    double _double;
    bool boolean;
    u4 *returnAddress;
} primitive_type;

typedef union
{
    ClassFile *object_ref;
    primitive_type *array_ref;
} reference;

typedef union
{
    reference ref;
    primitive_type t;
} java_type;

typedef struct
{
    int16_t top;
    u2 capacity;
    java_type *stack;
} OperandStack;

typedef struct Frame
{
    u2 max_locals;
    java_type *local_variables;
    u2 max_stack;
    OperandStack operand_stack;
    cp_info *runtime_cp;
    struct Frame *previous_frame;
    member_info *method;
    u1 *method_code;
    u4 method_code_length;
} Frame;

#endif