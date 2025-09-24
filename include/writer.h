#ifndef __WRITER_H__
#define __WRITER_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "types.h"
#include "tags.h"
#include "utils.h"

wchar_t* decode_modified_utf8_str(u2, const u1*);

void show_constant(u2, u2, cp_info*);

void show_classfile(ClassFile*);

#endif