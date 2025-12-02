#ifndef BYTECODE_IMPL_LOADS_H
#define BYTECODE_IMPL_LOADS_H

#include "Frame.h"
#include "interpreter.h"

void _load(Frame *);
void iload_n(Frame *);
void lload_n(Frame *);
void dload_n(Frame *);
void aload_n(Frame *);
void Taload(Frame *);

#endif