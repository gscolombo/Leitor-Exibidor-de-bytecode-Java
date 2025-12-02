#ifndef BYTECODE_IMPL_REFERENCES_H
#define BYTECODE_IMPL_REFERENCES_H

#include "Frame.h"
#include "interpreter.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

void getstatic(Frame *);
void putstatic(Frame *);
void getfield(Frame *);
void putfield(Frame *);
void invokespecial(Frame *);
void invokevirtual(Frame *);
void invokestatic(Frame *);
void invokeinterface(Frame *);
void newarray(Frame *);
void anewarray(Frame *);
void _arraylength(Frame *);
void new(Frame *);

#endif
