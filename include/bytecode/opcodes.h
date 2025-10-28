#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t u1;
typedef uint32_t u4;
typedef uint16_t u2;


typedef struct {
    u1 opcode;
    const char* name;
    int category;
} OpcodeInfo;

const OpcodeInfo* get_opcode_info(u1 opcode);

void show_opcodes(const u1 *code, u4 length);

#endif
