#ifndef __CP_CONSTANTS_H__
#define __CP_CONSTANTS_H__

#include "types.h"

typedef union Constant {
    struct {
        u2 name_index;
    } Class;
    struct {
        u2 class_index;
        u2 name_and_type_index;
    } _Ref;
    struct {
        u2 string_index;
    } String;
    struct {
        u4 bytes;
    } _4Bn;
    struct {
        u4 high_bytes;
        u4 low_bytes;
    } _8Bn;
    struct {
        u2 name_index;
        u2 descriptor_index;
    } NameAndType;
    struct {
        u2 length;
        u1 *bytes;   
        wchar_t *str;
    } UTF8;
    struct {
        u1 reference_kind;
        u2 reference_index;
    } MethodHandle;
    struct {
        u2 descriptor_index;
    } MethodType;
    struct {
        u2 bootstrap_method_attr_index;
        u2 name_and_type_index;
    } InvokeDynamic;
} Constant;

typedef struct cp_info {
    u1 tag;
    Constant info;
} cp_info;

#endif