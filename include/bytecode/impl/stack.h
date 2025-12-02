#ifndef BYTECODE_IMPL_STACK_H
#define BYTECODE_IMPL_STACK_H

#include "Frame.h"
#include "interpreter.h"

void pop_(Frame *);
void dup(Frame *);
void dup2(Frame *);

#endif
