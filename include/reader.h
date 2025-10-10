#ifndef READER_H_
#define READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Classfile.h"
#include "utils.h"
#include "parser.h"
#include "ref.h"

#define LE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 1 : 0)

FILE *open_classfile(const char *);

ClassFile read_classfile(FILE *);

u1 read_u1(FILE *);
u2 read_u2(FILE *);
u4 read_u4(FILE *);

void read_attributes(cp_info *, u2, FILE *, attribute *);

#endif