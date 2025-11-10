#ifndef OPCODE_INFO_H
#define OPCODE_INFO_H

#include "uinteger.h"
#include "Frame.h"

typedef struct
{
    const char *mnemonic;
    void (*exec)(Frame *);
} OpcodeInfo;

#endif