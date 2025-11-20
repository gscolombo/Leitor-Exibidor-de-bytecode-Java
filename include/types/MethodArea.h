#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include <stdio.h>

#include "attributes.h"
#include "Classfile.h"
#include "java_type.h"

typedef struct FieldImpl
{
    char *name;
    char *type;
    u2 access_flags;
    dtype value;
    attribute *attrs;
} Field;

typedef struct MethodImpl
{
    char *name;
    char *descriptor;
    char *params;
    char *rettype;
    u2 access_flags;
    struct
    {
        u2 nargs;
        u2 max_stack;
        u2 max_locals;
        u4 code_length;
        u1 *code;
    } bytecode;
    u4 ref_count;
    void **refs;
} Method;

typedef struct
{
    int type;
    union constant
    {
        char *strref;
        int32_t i;
        int64_t l;
        float f;
        double d;
    } value;
} RuntimeConstant;

typedef struct ClassImpl
{
    char *name;
    char *super;
    u2 super_index;
    u2 access_flags;
    u2 constants_count;
    RuntimeConstant *runtime_cp;
    u2 field_count;
    Field *fields;
    u2 method_count;
    Method *methods;
    // TODO: Handle interfaces
} Class;

typedef struct
{
    size_t num_classes;
    Class *classes;
} MethodArea;

#endif