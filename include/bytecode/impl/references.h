#ifndef BYTECODE_IMPL_REFERENCES_H
#define BYTECODE_IMPL_REFERENCES_H

#include "Frame.h"
#include "interpreter.h"
#include "bootstrap_loader.h"

void getstatic(Frame *);
void invokespecial(Frame *);
void invokevirtual(Frame *);
void invokestatic(Frame *);
void newarray(Frame *);
void _arraylength(Frame *);

#endif
