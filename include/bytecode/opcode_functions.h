#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "Thread.h"
#include "interpreter.h"
#include "cp/writer.h"

void aload_0(Thread *);

void ldc(Thread *);

void getstatic(Thread *);
void invokespecial(Thread *);
void invokevirtual(Thread *);

void _return(Thread *);


#endif