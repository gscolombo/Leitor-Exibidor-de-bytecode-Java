#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "ref.h"
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

void show_methods(const ClassFile *);
char *get_method_access_flags_names(u2);
wchar_t *get_full_method_name(wchar_t *, const wchar_t *, u2, const ClassFile *);
void parse_method_descriptors(wchar_t *, const wchar_t **, size_t *);

#endif