#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;


typedef struct attribute_info {
    u2 attribute_name_index;
    u4 attribute_length;
    u2 *info;
} attribute_info;

typedef struct field_info {
    u2 *access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info *attributes;
} field_info;

typedef struct method_info {
    u2 *access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    attribute_info *attributes;
} method_info;

#endif