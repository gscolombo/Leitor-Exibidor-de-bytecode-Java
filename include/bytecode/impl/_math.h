#ifndef BYTECODE_IMPL_MATH_H
#define BYTECODE_IMPL_MATH_H

#include "interpreter.h"
#include "Frame.h"

void iadd(Frame *);
void isub(Frame *);
void imul(Frame *);
void iinc(Frame *);

void dadd(Frame *);
void dsub(Frame *);
void dmul(Frame *);
void ddiv(Frame *);
void dneg(Frame *);
void drem(Frame *);

#endif
