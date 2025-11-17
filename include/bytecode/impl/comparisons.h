#ifndef BYTECODE_IMPL_COMPARISONS_H
#define BYTECODE_IMPL_COMPARISONS_H

#include "Frame.h"
#include "interpreter.h"

void if_cond(Frame *);
void if_icmp_cond(Frame *);

void dcmp_op(Frame *);

#endif
