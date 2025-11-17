#ifndef BYTECODE_IMPL_STORES_H
#define BYTECODE_IMPL_STORES_H

#include "Frame.h"
#include "interpreter.h"

void istore(Frame *);
void astore(Frame *);

void istore_n(Frame *);
void astore_n(Frame *);

void Tastore(Frame *);

#endif