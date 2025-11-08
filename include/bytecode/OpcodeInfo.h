#ifndef OPCODE_INFO_H
#define OPCODE_INFO_H

#include "uinteger.h"
#include "Thread.h"

typedef struct
{
    const char *mnemonic;
    void (*exec)(Thread *);
} OpcodeInfo;

#endif