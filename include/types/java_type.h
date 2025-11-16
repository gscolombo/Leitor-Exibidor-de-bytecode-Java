#ifndef JAVA_TYPE_H
#define JAVA_TYPE_H

#include "uinteger.h"
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

typedef struct ObjectRef
{
    struct ClassImpl *_class;
    struct
    {
        const char *name;
        const char *type;
        u2 access_flags;
        union
        {
            primitive_type p;
            union reference *r;
        } value;
    } *fields;
} ObjectRef;

typedef struct ArrayRef
{
    u4 arraylength;
    u1 dims;
    void *values; // If dims > 1, it must point to an ArrayRef struct
} ArrayRef;

typedef union reference
{
    ObjectRef object_ref;
    union
    {
        char *string;
        ArrayRef array;
    } array_ref;
} reference;

typedef union java_type
{
    reference ref;
    primitive_type t;
} java_type;

#endif