#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "Frame.h"
#include "interpreter.h"
#include "cp/writer.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

void iconst_i(Frame *);
void bipush(Frame *);
void ldc(Frame *);

void if_cond(Frame *);
void if_icmp_cond(Frame *);
void _goto(Frame *);

void iadd(Frame *);
void isub(Frame *);
void imul(Frame *);
void iinc(Frame *);

void iload_n(Frame *);
void aload_0(Frame *);
void istore_n(Frame *);

void getstatic(Frame *);
void invokespecial(Frame *);
void invokevirtual(Frame *);
void invokestatic(Frame *);

void tableswitch(Frame *);

void ireturn(Frame *);
void _return(Frame *);

#endif