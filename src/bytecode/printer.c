#include <stdio.h>
#include "bytecode/opcodes.h"
#include "bytecode/printer.h"

void print_opcodes(const uint8_t *code, size_t length) {
    printf("Opcodes (categoria 1 e 2):\n");
    for (size_t i = 0; i < length; i++) {
        const OpcodeInfo *info = get_opcode_info(code[i]);
        if (info && (info->category == 1 || info->category == 2)) {
            printf("  %04zu: %-10s (cat %d)\n", i, info->name, info->category);
        }
    }
}
