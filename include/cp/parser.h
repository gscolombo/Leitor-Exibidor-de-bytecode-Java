#ifndef __CP_PARSER_H__
#define __CP_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "constants.h"
#include "tags.h"

cp_info* parse_constant_pool(FILE*, u2);

wchar_t* decode_modified_utf8_str(u2, const u1*);

float decode_float_bytes(u4);

long decode_long_bytes(u4, u4);

double decode_double_bytes(u4, u4);

#endif