#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "Frame.h"
#include "interpreter.h"
#include "cp/writer.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

void iconst_i(Frame *);
void fconst_f(Frame *);

void bipush(Frame *);
void sipush(Frame *);

void ldc(Frame *);

void _load(Frame *);

void if_cond(Frame *);
void if_icmp_cond(Frame *);
void _goto(Frame *);

void iadd(Frame *);
void isub(Frame *);
void imul(Frame *);
void iinc(Frame *);

void i2T(Frame *);

void iload_n(Frame *);
void iaload(Frame *);
void aload_n(Frame *);

void Taload(Frame *);

void istore(Frame *);
void astore(Frame *);

void istore_n(Frame *);
void astore_n(Frame *);

void Tastore(Frame *);

void dup(Frame *);
void dup2(Frame *);

void tableswitch(Frame *);

void getstatic(Frame *);
void invokespecial(Frame *);
void invokevirtual(Frame *);
void invokestatic(Frame *);

void newarray(Frame *);
void _arraylength(Frame *);

void ireturn(Frame *);
void _return(Frame *);

#endif