#ifndef REFERENCES_FIELDS_H
#define REFERENCES_FIELDS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "ref.h"
#include "utils.h"
#include "wchar.h"

void show_fields(const ClassFile *);

char *get_field_access_flags_names(u2);

wchar_t *get_field_full_name(const wchar_t *, const wchar_t *, u2, const ClassFile *);

#endif