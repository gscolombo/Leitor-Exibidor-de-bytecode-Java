#ifndef BYTECODE_IMPL_CONSTANTS_H
#define BYTECODE_IMPL_CONSTANTS_H

#include "Frame.h"
#include "interpreter.h"

void iconst_i(Frame *);
void fconst_f(Frame *);

void bipush(Frame *);
void sipush(Frame *);

void ldc_(Frame *);

#endif