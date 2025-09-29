#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "types.h"
#include "tags.h"
#include "utils.h"
#include "wchar.h"

struct RefInfo
{
    wchar_t *cls, *cls_name, *cls_type;
    u2 cls_index, name_and_type_index;
};

void show_constant(u2, u2, cp_info *);

void show_classfile(ClassFile *);

struct RefInfo get_ref_info(const cp_info *, u2);

void show_class_access_flags(const ClassFile *);

#endif