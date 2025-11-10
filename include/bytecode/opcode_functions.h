#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "Thread.h"
#include "interpreter.h"
#include "cp/writer.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

void aload_0(Thread *);

void iload_n(Thread *);

void ldc(Thread *);

void iconst_i(Thread *);

void getstatic(Thread *);
void invokespecial(Thread *);
void invokevirtual(Thread *);
void invokestatic(Thread *);

void tableswitch(Thread *);

void ireturn(Thread *);
void _return(Thread *);


#endif