#ifndef TYPES_CP_CONSTANTS_H
#define TYPES_CP_CONSTANTS_H

/* Tags de constantes */
#define CONSTANT_Class 7
#define CONSTANT_Fieldref 9
#define CONSTANT_Methodref 10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_String 8
#define CONSTANT_Integer 3
#define CONSTANT_Float 4
#define CONSTANT_Long 5
#define CONSTANT_Double 6
#define CONSTANT_NameAndType 12
#define CONSTANT_UTF8 1
#define CONSTANT_MethodHandle 15
#define CONSTANT_MethodType 16
#define CONSTANT_InvokeDynamic 18

#include "uinteger.h"

/**
 * União dos diferentes
 * tipos de constantes apresentados
 * na especificação da JVM 8.
 */
typedef union Constant
{
    struct
    {
        u2 name_index;
    } Class;
    struct
    {
        u2 class_index;
        u2 name_and_type_index;
    } Ref;
    struct
    {
        u2 string_index;
    } String;
    struct
    {
        u4 bytes;
        union
        {
            int i;
            float f;
        } number;
    } _4Bn;
    struct
    {
        u4 high_bytes;
        u4 low_bytes;
        union
        {
            long l;
            double d;
        } number;
    } _8Bn;
    struct
    {
        u2 name_index;
        u2 descriptor_index;
    } NameAndType;
    struct
    {
        u2 length;
        u1 *bytes;
        wchar_t *str;
    } UTF8;
    struct
    {
        u1 reference_kind;
        u2 reference_index;
    } MethodHandle;
    struct
    {
        u2 descriptor_index;
    } MethodType;
    struct
    {
        u2 bootstrap_method_attr_index;
        u2 name_and_type_index;
    } InvokeDynamic;
} Constant;

/**
 * Estrutura para representação de uma
 * constante no pool de constantes.
 */
typedef struct cp_info
{
    u1 tag;
    Constant info;
} cp_info;

#endif