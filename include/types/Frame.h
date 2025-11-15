#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include "uinteger.h"
#include "constants.h"
#include "Classfile.h"
#include "MethodArea.h"

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

typedef union java_type
{
    union reference
    {
        struct ObjectRef
        {
            ClassFile *_class;
            struct Field
            {
                const char *name;
                const char *descriptor;
                union java_type *value;
            } *fields;
        } *object_ref;
        primitive_type *array_ref;
    } ref;
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
    u4 pc;
    java_type *local_variables;
    OperandStack operand_stack;
    struct Frame *previous_frame;
    Method *method;
    Class *class;
    MethodArea *method_area;
} Frame;

#endif