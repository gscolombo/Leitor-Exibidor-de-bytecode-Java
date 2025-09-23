#ifndef __CP_PARSER_H__
#define __CP_PARSER_H_

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "tags.h"

cp_info* parse_constant_pool(FILE*, u2);

#endif