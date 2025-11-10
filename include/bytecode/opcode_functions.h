#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "Frame.h"
#include "interpreter.h"
#include "cp/writer.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

void aload_0(Frame *);

void iload_n(Frame *);

void ldc(Frame *);

void iconst_i(Frame *);

void getstatic(Frame *);
void invokespecial(Frame *);
void invokevirtual(Frame *);
void invokestatic(Frame *);

void tableswitch(Frame *);

void ireturn(Frame *);
void _return(Frame *);


#endif