#ifndef TYPES_INFO_H
#define TYPES_INFO_H

#include "uinteger.h"
#include "attributes.h"

typedef struct FlagMap
{
    u2 flag;
    const char* name;
} FlagMap;

typedef struct member_info
{
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute *attributes;
} member_info;

#endif