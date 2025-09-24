#ifndef __WRITER_H__
#define __WRITER_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "types.h"
#include "tags.h"
#include "utils.h"
#include "wchar.h"


void show_constant(u2, u2, cp_info*);

void show_classfile(ClassFile*);

#endif