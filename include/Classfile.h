#ifndef CLASSFILE_H_
#define CLASSFILE_H_

#include <stdlib.h>

#include "uinteger.h"
#include "ref.h"
#include "constants.h"

typedef struct ClassFile
{
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info *constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    u2 *interfaces;
    u2 fields_count;
    field_info *fields;
    u2 methods_count;
    method_info *methods;
    u2 attributes_count;
    attribute *attributes;
} ClassFile;

void free_classfile(ClassFile *);
void free_attributes(cp_info*, u2, attribute*);
#endif