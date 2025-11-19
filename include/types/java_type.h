#ifndef JAVA_TYPE_H
#define JAVA_TYPE_H

#include "uinteger.h"
#include "MethodArea.h"

typedef enum cat
{
    CAT1,
    CAT2
} cat;

typedef enum type_enum
{
    BYTE,
    SHORT,
    INT,
    LONG,
    CHAR,
    FLOAT,
    DOUBLE,
    BOOLEAN,
    RETURN_ADDRESS,
    REFERENCE
} type_enum;

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

typedef struct ArrayRef
{
    u1 t;
    u4 arraylength;
    u1 dims;
    void *values; // If dims > 1, it must point to a reference
} ArrayRef;

typedef union reference
{
    struct ClassImpl *object_ref;
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

typedef struct dtype
{
    cat cat;
    java_type value;
} dtype;

#endif