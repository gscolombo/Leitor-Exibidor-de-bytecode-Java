#ifndef __READER_H__
#define __READER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Classfile.h"
#include "utils.h"
#include "cp/parser.h"
#include "types.h"

#define _LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)

FILE *open_classfile(char *);

ClassFile *read_classfile(FILE *);

u1 read_u1(FILE *);
u2 read_u2(FILE *);
u4 read_u4(FILE *);

void read_attributes(cp_info *, unsigned int, FILE *, attribute *);

#endif