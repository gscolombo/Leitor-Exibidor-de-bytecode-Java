#ifndef BYTECODE_IMPL_CONTROL_H
#define BYTECODE_IMPL_CONTROL_H

#include "Frame.h"
#include "interpreter.h"
#include "bytecode/utils.h"

void _goto(Frame *);
void tableswitch(Frame *);
void ireturn(Frame *);
void _return(Frame *);

#endif
